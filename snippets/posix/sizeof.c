#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdalign.h>
#include <stddef.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <glob.h>
#include <dirent.h>
#include <termios.h>
#include <poll.h>
#include <iconv.h>
#include <mqueue.h>
#include <signal.h>
#include <regex.h>
#include <wordexp.h>
#include <utmpx.h>

#define P(x) printf("%-32s %zu\n", #x, sizeof(x))

int
main(void)
{
	P(uint8_t);
	P(uint16_t);
	P(uint32_t);
	P(uint64_t);
	P(uint_least8_t);
	P(uint_least16_t);
	P(uint_least32_t);
	P(uint_least64_t);
	P(uint_fast8_t);
	P(uint_fast16_t);
	P(uint_fast32_t);
	P(uint_fast64_t);
	P(uintmax_t);
	P(intmax_t);
	P(ptrdiff_t);
	P(uintptr_t);
	P(max_align_t);
	P(pthread_t);
	P(pthread_mutex_t);
	P(pthread_cond_t);
	P(pthread_attr_t);
	P(pthread_condattr_t);
	P(pthread_key_t);
	P(pthread_once_t);
	P(pthread_rwlock_t);
	P(pthread_rwlockattr_t);
	P(struct stat);
	P(time_t);
	P(struct tm);
	P(wchar_t);
	P(cc_t);
	P(clock_t);
	P(clockid_t);
	P(dev_t);
	P(div_t);
	P(FILE);
	P(glob_t);
	P(fpos_t);
	P(gid_t);
	P(iconv_t);
	P(id_t);
	P(ino_t);
	P(key_t);
	P(mode_t);
	P(mqd_t);
	P(nfds_t);
	P(nlink_t);
	P(off_t);
	P(pid_t);
	P(uid_t);
	P(regex_t);
	P(sig_atomic_t);
	P(sigset_t);
	P(wordexp_t);
	P(va_list);
	P(suseconds_t);
	P(struct timeval);
	P(struct timespec);
	P(struct utmpx);
	P(regex_t);
	P(regmatch_t);
	P(fd_set);
	return 0;
}
