#include <vxWorks.h>
#include <stdio.h>
#include <openssl/pem.h>
#include <ipssh.h>
#include <ipssh_sftp_clt.h>

typedef struct {
	struct Ipssh_conn_st *conn;
	EVP_PKEY *key;
} SFTP_CTX;

void
sftpclose(SFTP_CTX *ctx)
{
	if (!ctx)
		return;

	if (ctx->conn)
		ipssh_sftp_clt_close(ctx->conn);

	if (ctx->key)
		EVP_PKEY_free(ctx->key);

	free(ctx);
}

SFTP_CTX *
sftpdial(const char *host, UINT16 port, const char *keyfile, const char *user, const char *pass)
{
	SFTP_CTX *ctx;
	struct Ipssh_conn_st *conn;
	EVP_PKEY *key;
	BIO *bio;

	key = NULL;
	bio = NULL;
	ctx = calloc(1, sizeof(*ctx));
	if (!ctx) {
		printf("failed to allocate a sftp context\n");
		goto error;
	}

	conn = ipssh_conn_new();
	if (!conn) {
		printf("failed to allocate a new connection\n");
		goto error;
	}

	ipssh_sftp_clt_set_userid(conn, user);

	if (keyfile) {
		bio = BIO_new(BIO_s_file());
		if (!bio) {
			printf("failed to allocate memory for BIO\n");
			goto error;
		}

		if (!BIO_read_filename(bio, keyfile)) {
			printf("failed to read key file\n");
			goto error;
		}

		key = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
		if (!key) {
			printf("failed to parse key file\n");
			goto error;
		}

		ipssh_sftp_clt_set_key(conn, key);
	}

	if (pass)
		ipssh_sftp_clt_set_pw(conn, pass);

	if (ipssh_sftp_clt_open(conn, host, port) != IPCOM_SUCCESS) {
		printf("failed to open a sftp connection\n");
		goto error;
	}

	ctx->conn = conn;
	ctx->key = key;

	if (0) {
	error:
		if (ctx) {
			ctx->conn = conn;
			ctx->key = key;
			sftpclose(ctx);
		}

		ctx = NULL;
	}

	if (bio)
		BIO_free(bio);

	return ctx;
}

STATUS
sftpget(SFTP_CTX *ctx, const char *from, const char *to)
{
	struct Ipssh_conn_st *conn;
	STATUS status;

	conn = ctx->conn;
	status = OK;

	if (ipssh_sftp_clt_get(conn, from, to) != IPCOM_SUCCESS) {
		printf("failed to get the file\n");
		goto error;
	}

	printf("file downloaded successfully\n");

	if (0) {
	error:
		status = ERROR;
	}

	return status;
}

STATUS
sftpput(SFTP_CTX *ctx, const char *from, const char *to)
{
	struct Ipssh_conn_st *conn;
	STATUS status;

	conn = ctx->conn;
	status = OK;

	if (ipssh_sftp_clt_put(conn, from, to) != IPCOM_SUCCESS) {
		printf("failed to put the file\n");
		goto error;
	}

	printf("file uploaded successfully\n");

	if (0) {
	error:
		status = ERROR;
	}

	return status;
}

STATUS
sftpcwd(SFTP_CTX *ctx)
{
	char buf[128];
	STATUS status;

	status = OK;
	if (ipssh_sftp_clt_get_cwd(ctx->conn, buf, sizeof(buf)) != IPCOM_SUCCESS) {
		printf("failed to get current working directory\n");
		goto error;
	}

	printf("cwd: \"%s\"\n", buf);

	if (0) {
	error:
		status = ERROR;
	}
	return status;
}

STATUS
sftpcd(SFTP_CTX *ctx, const char *path)
{
	return ipssh_sftp_clt_chdir(ctx->conn, path);
}

STATUS
sftpls(SFTP_CTX *ctx, const char *dir)
{
	struct Ipssh_conn_st *conn;
	Ipssh_dirent *ent;
	void *handle;
	STATUS status;

	conn = ctx->conn;
	status = OK;
	handle = ipssh_sftp_clt_opendir(conn, dir);
	if (!handle) {
		printf("failed to open directory\n");
		goto error;
	}

	while ((ent = ipssh_sftp_clt_readdir(conn, handle))) {
		printf("%s\n", ent->longname);
	}

	if (0) {
	error:
		status = ERROR;
	}

	if (handle)
		ipssh_sftp_clt_closedir(conn, handle);

	return status;
}

STATUS
sftpmkdir(SFTP_CTX *ctx, const char *path, mode_t mode)
{
	return ipssh_sftp_clt_mkdir(ctx->conn, path, mode);
}

STATUS
sftprmdir(SFTP_CTX *ctx, const char *path)
{
	return ipssh_sftp_clt_rmdir(ctx->conn, path);
}

STATUS
sftpmv(SFTP_CTX *ctx, const char *from, const char *to)
{
	return ipssh_sftp_clt_rename(ctx->conn, from, to);
}

STATUS
sftprm(SFTP_CTX *ctx, const char *path)
{
	return ipssh_sftp_clt_unlink(ctx->conn, path);
}
