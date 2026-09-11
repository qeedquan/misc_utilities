#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <sys/sysinfo.h>

#define wrapsub(a, b) (((a) > (b)) ? ((a) - (b)) : 0)

typedef struct {
	unsigned long long usertime;
	unsigned long long nicetime;
	unsigned long long systemtime;
	unsigned long long idletime;
	unsigned long long iowait;
	unsigned long long irq;
	unsigned long long softirq;
	unsigned long long steal;
	unsigned long long guest;
	unsigned long long guestnice;

	unsigned long long idlealltime;
	unsigned long long systemalltime;
	unsigned long long virtalltime;
	unsigned long long totaltime;

	unsigned long long userperiod;
	unsigned long long niceperiod;
	unsigned long long systemperiod;
	unsigned long long systemallperiod;
	unsigned long long idleallperiod;
	unsigned long long idleperiod;
	unsigned long long iowaitperiod;
	unsigned long long irqperiod;
	unsigned long long softirqperiod;
	unsigned long long stealperiod;
	unsigned long long guestperiod;
	unsigned long long totalperiod;

	double nicemeter;
	double normalmeter;
	double kernelmeter;
	double irqmeter;
	double softirqmeter;
	double stealmeter;
	double guestmeter;
	double iowaitmeter;

	double usagepercent;
} cpu_stat_t;

long delayms = 100;

cpu_stat_t *
getcpusinfo(int *l)
{
	FILE *fp;
	cpu_stat_t *c, *p;
	char buf[4096];
	int nr, i, cpuid;

	fp = NULL;
	*l = get_nprocs() + 1;
	c = calloc(*l, sizeof(*c));
	if (!c)
		goto error;

	fp = fopen("/proc/stat", "r");
	if (!fp)
		goto error;

	for (i = 0; i < *l; i++) {
		if (!fgets(buf, sizeof(buf), fp))
			goto error;

		p = &c[i];
		if (i == 0) {
			nr = sscanf(buf, "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
			            &p->usertime, &p->nicetime, &p->systemtime, &p->idletime, &p->iowait, &p->irq,
			            &p->softirq, &p->steal, &p->guest, &p->guestnice);
			if (nr != 10)
				goto error;
		} else {
			nr = sscanf(buf, "cpu%4d %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
			            &cpuid, &p->usertime, &p->nicetime, &p->systemtime, &p->idletime, &p->iowait, &p->irq,
			            &p->softirq, &p->steal, &p->guest, &p->guestnice);
			if (nr != 11)
				goto error;
		}

		p->idlealltime = p->idletime + p->iowait;
		p->systemalltime = p->systemtime + p->irq + p->softirq;
		p->virtalltime = p->guest + p->guestnice;
		p->totaltime = p->usertime + p->nicetime + p->systemalltime + p->idlealltime + p->steal + p->virtalltime;
	}

	if (0) {
	error:
		free(c);
		c = NULL;
		*l = 0;
	}

	if (fp)
		fclose(fp);
	return c;
}

void
calcperiod(cpu_stat_t *r, cpu_stat_t *a, cpu_stat_t *b)
{
	double t;

	r->userperiod = wrapsub(a->usertime, b->usertime);
	r->niceperiod = wrapsub(a->nicetime, b->nicetime);
	r->systemperiod = wrapsub(a->systemtime, b->systemtime);
	r->systemallperiod = wrapsub(a->systemalltime, b->systemalltime);
	r->idleallperiod = wrapsub(a->idlealltime, b->idlealltime);
	r->idleperiod = wrapsub(a->idletime, b->idletime);
	r->iowaitperiod = wrapsub(a->iowait, b->iowait);
	r->irqperiod = wrapsub(a->irq, b->irq);
	r->softirqperiod = wrapsub(a->softirq, b->softirq);
	r->stealperiod = wrapsub(a->steal, b->steal);
	r->guestperiod = wrapsub(a->virtalltime, b->virtalltime);
	r->totalperiod = wrapsub(a->totaltime, b->totaltime);

	t = (r->totalperiod == 0) ? 1 : r->totalperiod;
	r->nicemeter = r->niceperiod / t * 100.0;
	r->normalmeter = r->userperiod / t * 100.0;
	r->kernelmeter = r->systemperiod / t * 100.0;
	r->irqmeter = r->irqperiod / t * 100.0;
	r->softirqmeter = r->softirqperiod / t * 100.0;
	r->stealmeter = r->stealperiod / t * 100.0;
	r->guestmeter = r->guestperiod / t * 100.0;
	r->iowaitmeter = r->iowaitperiod / t * 100.0;

	r->usagepercent = r->nicemeter + r->normalmeter + r->kernelmeter + r->irqmeter + r->softirqmeter;
}

void
dumpcpustats(void)
{
	cpu_stat_t *a, *b, *p;
	struct timespec tp;
	int i, n, m;
	double lv[3];

	getloadavg(lv, 3);
	printf("load average: %lf %lf %lf\n", lv[0], lv[1], lv[2]);

	tp.tv_sec = delayms / 1000;
	tp.tv_nsec = (delayms % 1000) * 1000000;

	a = getcpusinfo(&n);
	nanosleep(&tp, NULL);
	b = getcpusinfo(&m);
	for (i = 0; i < n; i++)
		calcperiod(&b[i], &b[i], &a[i]);

	if (!a || !b || (n != m))
		errx(1, "Failed to get CPU info\n");

	for (i = 0; i < n; i++) {
		p = &b[i];

		if (i == 0)
			printf("Total CPU Stats\n");
		else
			printf("CPU%d Stats\n", i - 1);

		printf("User Time         : %16llu\n", p->usertime);
		printf("Nice Time         : %16llu\n", p->nicetime);
		printf("System Time       : %16llu\n", p->systemtime);
		printf("Idle Time         : %16llu\n", p->idletime);
		printf("IO Wait           : %16llu\n", p->iowait);
		printf("IRQ               : %16llu\n", p->irq);
		printf("SoftIRQ           : %16llu\n", p->softirq);
		printf("Steal             : %16llu\n", p->steal);
		printf("Guest             : %16llu\n", p->guest);
		printf("Guest Nice        : %16llu\n", p->guestnice);
		printf("Idle Total Time   : %16llu\n", p->idlealltime);
		printf("System Total Time : %16llu\n", p->systemalltime);
		printf("Virt Total Time   : %16llu\n", p->virtalltime);
		printf("Total Time        : %16llu\n", p->totaltime);
		printf("Usage Percent     : %16lf\n", p->usagepercent);
		printf("\n");
	}

	free(a);
	free(b);
}

void
usage(void)
{
	fprintf(stderr, "usage: [-dh]\n");
	fprintf(stderr, "  -d <ms> delay for a milliseconds amount (default %ld)\n", delayms);
	fprintf(stderr, "  -h      print usage\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	int c;
	while ((c = getopt(argc, argv, "d:h")) != -1) {
		switch (c) {
		case 'h':
			usage();
			break;

		case 'd':
			delayms = atol(optarg);
			break;
		}
	}

	dumpcpustats();
	return 0;
}
