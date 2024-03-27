#include <vxWorks.h>
#include <stdio.h>
#include <openssl/pem.h>
#include <ipssh.h>
#include <ipssh_sftp_clt.h>

STATUS
sftpget(const char *host, UINT16 port, const char *keyfile, const char *user, const char *pass, const char *from, const char *to)
{
	struct Ipssh_conn_st *conn;
	BIO *bio;
	EVP_PKEY *key;
	STATUS status;

	status = OK;
	bio = NULL;
	key = NULL;
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
		printf("failed to open a sftp connectionn\n");
		goto error;
	}

	if (ipssh_sftp_clt_get(conn, from, to) != IPCOM_SUCCESS) {
		printf("failed to get the file\n");
		goto error;
	}

	printf("file downloaded successfully\n");

	if (0) {
	error:
		status = ERROR;
	}

	if (conn)
		ipssh_sftp_clt_close(conn);

	if (bio)
		BIO_free(bio);

	if (key)
		EVP_PKEY_free(key);

	return status;
}
