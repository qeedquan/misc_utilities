#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if_alg.h>

void
hexdump(void *buf, size_t len)
{
	uint8_t *p;
	size_t i;

	p = buf;
	for (i = 0; i < len; i++)
		printf("%02x", p[i]);
	printf("\n");
}

int
mkcryptfd(const char *type, const char *name)
{
	struct sockaddr_alg sa;
	int fd;

	fd = socket(PF_ALG, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
	if (fd < 0)
		return -errno;

	memset(&sa, 0, sizeof(sa));
	sa.salg_family = AF_ALG;
	strcpy((char *)sa.salg_type, type);
	strcpy((char *)sa.salg_name, name);

	if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
		return -errno;

	return fd;
}

ssize_t
docrypt(__u32 op, int fd, void *dst, size_t dstlen, void *src, size_t srclen)
{
	char cbuf[CMSG_SPACE(sizeof(op))];
	struct cmsghdr *cmsg;
	struct msghdr msg;
	struct iovec iov;
	ssize_t len;

	memset(cbuf, 0, sizeof(cbuf));
	memset(&msg, 0, sizeof(msg));

	msg.msg_control = cbuf;
	msg.msg_controllen = sizeof(cbuf);

	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(sizeof(op));
	memcpy(CMSG_DATA(cmsg), &op, sizeof(op));

	iov.iov_base = src;
	iov.iov_len = srclen;

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	len = sendmsg(fd, &msg, 0);
	if (len < 0)
		return -errno;

	len = read(fd, dst, dstlen);
	if (len < 0)
		return -errno;

	return len;
}

void
aesecb(void)
{
	const uint8_t key[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	uint8_t plain[] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31 };
	uint8_t cipher[sizeof(plain)];
	uint8_t buf[sizeof(plain)];

	int fd, cd, r;

	cd = -1;
	fd = mkcryptfd("skcipher", "ecb(aes)");
	if (fd < 0)
		goto error;

	if (setsockopt(fd, SOL_ALG, ALG_SET_KEY, key, sizeof(key)) < 0)
		goto error;

	cd = accept(fd, NULL, 0);
	if (cd < 0)
		goto error;

	r = docrypt(ALG_OP_ENCRYPT, cd, cipher, sizeof(cipher), plain, sizeof(plain));
	if (r < 0)
		goto error;

	r = docrypt(ALG_OP_DECRYPT, cd, buf, sizeof(buf), cipher, sizeof(cipher));
	if (r < 0)
		goto error;

	if (memcmp(plain, buf, sizeof(buf)))
		printf("%s: mismatch encrypt/decrypt\n", __func__);

	if (0) {
	error:
		printf("%s: %s\n", __func__, strerror(errno));
	}

	if (fd >= 0)
		close(fd);
	if (cd >= 0)
		close(cd);
}

void
aescmac(void)
{
	uint8_t key[16], msg[16], hash[16];
	int fd, cd;

	memset(key, 0x55, sizeof(key));
	memset(msg, 0x7a, sizeof(msg));

	cd = -1;
	fd = mkcryptfd("hash", "cmac(aes)");
	if (fd < 0)
		goto error;

	if (setsockopt(fd, SOL_ALG, ALG_SET_KEY, key, sizeof(key)) < 0)
		goto error;

	cd = accept(fd, NULL, 0);
	if (cd < 0)
		goto error;

	if (write(cd, msg, sizeof(msg)) != sizeof(msg))
		goto error;

	if (read(cd, hash, sizeof(hash)) != sizeof(hash))
		goto error;

	hexdump(hash, sizeof(hash));

	if (0) {
	error:
		printf("%s: %s\n", __func__, strerror(errno));
	}

	if (fd >= 0)
		close(fd);
	if (cd >= 0)
		close(cd);
}

int
main(void)
{
	aesecb();
	aescmac();
	return 0;
}
