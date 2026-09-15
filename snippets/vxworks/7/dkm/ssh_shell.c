#include <vxWorks.h>
#include <stdio.h>
#include <ipssh.h>

STATUS
ssh_shell(char *addr, char *user, char *pass)
{
	struct Ipssh_conn_st *ssh;
	char line[1024];
	size_t len;
	ssize_t nread;
	STATUS status;

	status = OK;
	ssh = ipsshc_conn_init();
	if (!ssh) {
		printf("Failed to init a connection\n");
		goto error;
	}

	if (ipsshc_set_username(ssh, user) != OK) {
		printf("Failed to set username\n");
		goto error;
	}

	if (ipsshc_set_password(ssh, pass) != OK) {
		printf("Failed to set password\n");
		goto error;
	}

	if (ipsshc_login(ssh, addr, 0, NULL) != OK) {
		printf("Failed to login\n");
		goto error;
	}

	ipsshc_set_read_timeout(ssh, 100);

	for (;;) {
		for (;;) {
			nread = ipsshc_read(ssh, line, sizeof(line), NULL);
			if (nread < 0) {
				if (ipcom_errno == IP_ERRNO_EWOULDBLOCK) {
					ipcom_errno = 0;
					break;
				} else
					goto out;
			}
			fwrite(line, nread, 1, stdout);
		}

		if (!fgets(line, sizeof(line) - 1, stdin))
			break;

		len = strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[--len] = '\0';
		line[len++] = '\r';

		if (ipsshc_write(ssh, line, len) != len)
			break;
	}

	if (0) {
	error:
		status = ERROR;
	}

out:
	if (ssh)
		ipsshc_close(ssh);

	return status;
}
