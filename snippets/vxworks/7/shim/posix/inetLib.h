#ifndef _INET_LIB_H_
#define _INET_LIB_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/select.h>

enum {
	INET_ADDR_LEN = 18,
	INET6_ADDR_LEN = 46,
};

void inet_ntoa_b(struct in_addr inetAddress, char *pString);

#endif
