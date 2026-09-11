#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <tss2/tss2_sys.h>
#include <tss2/tss2_tctildr.h>

#define warn(...) probe(0, __VA_ARGS__)
#define check(...) probe(1, __VA_ARGS__)

void
probe(int fatal, int r, const char *fmt, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if (r != TSS2_RC_SUCCESS) {
		fprintf(stderr, "%s: (error=%d)\n", buf, r);
		if (fatal)
			exit(1);
	}
}

void
read_public(TSS2_SYS_CONTEXT *sys, TPM2_HANDLE handle)
{
	TPM2B_PUBLIC out;
	int r;

	r = Tss2_Sys_ReadPublic(sys, handle, NULL, &out, NULL, NULL, NULL);
	if (r != TSS2_RC_SUCCESS) {
		warn(r, "Failed to read public portion of handle %#x", handle);
		return;
	}

	printf("\nPUBLIC AREA\n");
	printf("Type              %#x\n", out.publicArea.type);
	printf("Name Alg          %#x\n", out.publicArea.nameAlg);
	printf("Object Attributes %#x\n", out.publicArea.objectAttributes);
	printf("Symmetric Alg     %#x\n", out.publicArea.parameters.rsaDetail.symmetric.algorithm);
	printf("RSA key bits      %d\n", out.publicArea.parameters.rsaDetail.keyBits);
	printf("\n");
}

void
gen_primary_key(TSS2_SYS_CONTEXT *sys, TSS2L_SYS_AUTH_COMMAND *cmd_auths_array, TPM2_HANDLE handle)
{
	TPM2B_SENSITIVE_CREATE in_sensitive;
	TPM2B_PUBLIC in_public;
	TPM2B_DATA outside_info;
	TPML_PCR_SELECTION creation_pcr;

	TPM2_HANDLE primary_handle;
	TPM2B_PUBLIC out_public;
	TPM2B_CREATION_DATA creation_data;
	TPM2B_DIGEST creation_hash;
	TPMT_TK_CREATION creation_ticket;
	TPM2B_NAME name;

	int r;

	memset(&in_sensitive, 0, sizeof(in_sensitive));
	memset(&in_public, 0, sizeof(in_public));
	memset(&outside_info, 0, sizeof(outside_info));
	memset(&creation_pcr, 0, sizeof(creation_pcr));

	memset(&primary_handle, 0, sizeof(primary_handle));
	memset(&out_public, 0, sizeof(out_public));
	memset(&creation_data, 0, sizeof(creation_data));
	memset(&creation_hash, 0, sizeof(creation_hash));
	memset(&creation_ticket, 0, sizeof(creation_ticket));
	memset(&name, 0, sizeof(name));

	creation_hash.size = sizeof(creation_hash) - sizeof(uint16_t);
	name.size = sizeof(name) - sizeof(uint16_t);

	in_public.publicArea.type = TPM2_ALG_RSA;
	in_public.publicArea.nameAlg = TPM2_ALG_SHA256;

	in_public.publicArea.objectAttributes |= TPMA_OBJECT_RESTRICTED;
	in_public.publicArea.objectAttributes |= TPMA_OBJECT_USERWITHAUTH;
	in_public.publicArea.objectAttributes |= TPMA_OBJECT_DECRYPT;
	in_public.publicArea.objectAttributes |= TPMA_OBJECT_FIXEDTPM;
	in_public.publicArea.objectAttributes |= TPMA_OBJECT_FIXEDPARENT;
	in_public.publicArea.objectAttributes |= TPMA_OBJECT_SENSITIVEDATAORIGIN;
	in_public.publicArea.authPolicy.size = 0;

	in_public.publicArea.parameters.rsaDetail.symmetric.algorithm = TPM2_ALG_AES;
	in_public.publicArea.parameters.rsaDetail.symmetric.keyBits.aes = 128;
	in_public.publicArea.parameters.rsaDetail.symmetric.mode.aes = TPM2_ALG_CFB;
	in_public.publicArea.parameters.rsaDetail.scheme.scheme = TPM2_ALG_NULL;
	in_public.publicArea.parameters.rsaDetail.keyBits = 2048;
	in_public.publicArea.parameters.rsaDetail.exponent = 0;
	in_public.publicArea.unique.rsa.size = 0;

	// generate a primary key with some parameters
	// tpm2_createprimary
	r = Tss2_Sys_CreatePrimary(sys, TPM2_RH_OWNER,
	                           cmd_auths_array, &in_sensitive,
	                           &in_public, &outside_info, &creation_pcr,
	                           &primary_handle, &out_public, &creation_data,
	                           &creation_hash, &creation_ticket, &name, NULL);
	check(r, "Failed to create primary key");

	// remove the old primary key if it exist, it will error out if it doesn't
	// tpm2_evictcontrol
	r = Tss2_Sys_EvictControl(sys, TPM2_RH_OWNER, handle, cmd_auths_array, handle, NULL);
	warn(r, "WARN: Can't evict control on primary handle");

	// not that we made sure that the old primary key is not there anymore
	// store the generated primary key we just made into a static handle value that we designate
	r = Tss2_Sys_EvictControl(sys, TPM2_RH_OWNER,
	                          primary_handle, cmd_auths_array,
	                          handle, NULL);
	check(r, "Failed to store primary object handle into the NV");

	r = Tss2_Sys_FlushContext(sys, primary_handle);
	warn(r, "WARN: can't flush correct");
}

void
store_data(TSS2_SYS_CONTEXT *sys, TSS2L_SYS_AUTH_COMMAND *cmd_auths_array,
           TPM2_HANDLE primary_handle, TPM2_HANDLE data_handle,
           const void *buf, size_t len)
{
	TPM2_HANDLE object_handle;
	TPM2B_SENSITIVE_CREATE in_sensitive;
	TPM2B_PUBLIC in_public;
	TPM2B_PUBLIC out_public;
	TPM2B_DATA outside_info;
	TPM2B_PRIVATE out_private;
	TPML_PCR_SELECTION creation_pcr;
	TPM2B_CREATION_DATA creation_data;
	TPM2B_DIGEST creation_hash;
	TPMT_TK_CREATION creation_ticket;
	TPM2B_NAME name;

	int r;

	memset(&in_sensitive, 0, sizeof(in_sensitive));
	memset(&in_public, 0, sizeof(in_public));
	memset(&outside_info, 0, sizeof(outside_info));
	memset(&out_public, 0, sizeof(out_public));
	memset(&out_private, 0, sizeof(out_private));
	memset(&creation_data, 0, sizeof(creation_data));
	memset(&creation_pcr, 0, sizeof(creation_pcr));
	memset(&creation_hash, 0, sizeof(creation_hash));
	memset(&creation_ticket, 0, sizeof(creation_ticket));
	memset(&name, 0, sizeof(name));

	out_private.size = sizeof(out_private) - sizeof(uint16_t);
	creation_hash.size = sizeof(creation_hash) - sizeof(uint16_t);
	name.size = sizeof(name) - sizeof(uint16_t);

	in_sensitive.sensitive.data.size = len;
	memcpy(in_sensitive.sensitive.data.buffer, buf, len);
	in_sensitive.size = in_sensitive.sensitive.data.size + sizeof(uint16_t);

	in_public.publicArea.type = TPM2_ALG_KEYEDHASH;
	in_public.publicArea.nameAlg = TPM2_ALG_SHA256;

	in_public.publicArea.objectAttributes |= TPMA_OBJECT_USERWITHAUTH;
	in_public.publicArea.objectAttributes |= TPMA_OBJECT_FIXEDTPM;
	in_public.publicArea.objectAttributes |= TPMA_OBJECT_FIXEDPARENT;

	in_public.publicArea.parameters.rsaDetail.symmetric.algorithm = TPM2_ALG_NULL;
	in_public.publicArea.parameters.rsaDetail.scheme.scheme = TPM2_ALG_NULL;
	in_public.publicArea.parameters.rsaDetail.keyBits = 2048;
	in_public.publicArea.parameters.rsaDetail.exponent = 0;

	// create a child object derived from a primary handle
	do {
	r = Tss2_Sys_Create(sys,
	                    primary_handle,
	                    cmd_auths_array, &in_sensitive,
	                    &in_public, &outside_info, &creation_pcr, &out_private,
	                    &out_public, &creation_data, &creation_hash,
	                    &creation_ticket, NULL);
	} while (r == TPM2_RC_RETRY);

	check(r, "Failed to create key for storing data");

	r = Tss2_Sys_Load(sys,
	                  primary_handle,
	                  cmd_auths_array, &out_private,
	                  &out_public, &object_handle, &name, NULL);
	check(r, "Failed to load handle for storing data");

	// remove the old handle in TPM if it exists
	r = Tss2_Sys_EvictControl(sys, TPM2_RH_OWNER,
	                          data_handle,
	                          cmd_auths_array,
	                          data_handle, NULL);
	warn(r, "WARN: Can't evict control on handle for storing data");

	// store the handle to TPM NV
	r = Tss2_Sys_EvictControl(sys, TPM2_RH_OWNER,
	                          object_handle, cmd_auths_array,
	                          data_handle, NULL);
	check(r, "Failed to store handle into TPM for storing data");

	r = Tss2_Sys_FlushContext(sys, object_handle);
	warn(r, "WARN: Failed to flush context for storing data");
}

void
get_data(TSS2_SYS_CONTEXT *sys, TSS2L_SYS_AUTH_COMMAND *cmd_auths_array, TPM2_HANDLE data_handle, void *buf)
{
	TPM2B_SENSITIVE_DATA out;

	int r;

	memset(&out, 0x0, sizeof(out));
	out.size = sizeof(out) - sizeof(uint16_t);

	r = Tss2_Sys_Unseal(sys, data_handle, cmd_auths_array, &out, NULL);
	check(r, "Failed to unseal data");

	memcpy(buf, out.buffer, out.size);
}

// tests storing string into TPM NV area and reading in back
void
test_store_string(TSS2_SYS_CONTEXT *sys, const char *str)
{
	static const TPM2_HANDLE primary_handle = 0x81000000;
	static const TPM2_HANDLE string_handle = 0x81000001;
	char buf[256];

	TSS2L_SYS_AUTH_COMMAND cmd_auths_array;

	memset(buf, 0, sizeof(buf));
	memset(&cmd_auths_array, 0, sizeof(cmd_auths_array));
	cmd_auths_array.count = 1;
	cmd_auths_array.auths[0].sessionHandle = TPM2_RS_PW;

	// if we ran this program once, then the public area should be valid given a persistent handle
	// so we can read the parameters the key generation
	read_public(sys, primary_handle);

	// this corresponds to tpm2_createprimary and tpm2_create
	// we can generate a primary/child key and store it into a fixed handle of some number
	// then we can use it to identify it later
	gen_primary_key(sys, &cmd_auths_array, primary_handle);
	store_data(sys, &cmd_auths_array, primary_handle, string_handle, str, strlen(str));

	get_data(sys, &cmd_auths_array, string_handle, buf);
	printf("DATA: %s\n", buf);
}

int
main(void)
{
	TSS2_SYS_CONTEXT *sys;
	TSS2_TCTI_CONTEXT *tcti;
	size_t size;
	int r;

	r = Tss2_TctiLdr_Initialize(NULL, &tcti);
	check(r, "Failed to init loader");

	size = Tss2_Sys_GetContextSize(0);
	printf("Sys context size: %zu\n", size);

	sys = calloc(1, size);
	r = Tss2_Sys_Initialize(sys, size, tcti, NULL);
	check(r, "Failed to init sys");

	r = Tss2_Sys_SelfTest(sys, NULL, TPM2_YES, NULL);
	check(r, "Failed to self test");

	test_store_string(sys, "hello world! goodbye world!");

	Tss2_Sys_Shutdown(sys, NULL, TPM2_SU_CLEAR, NULL);
	Tss2_Tcti_Finalize(tcti);
	free(sys);
	free(tcti);

	return 0;
}
