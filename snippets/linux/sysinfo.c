#include <stdio.h>
#include <sys/sysinfo.h>

void
printsysinfo(void)
{
	struct sysinfo s;

	sysinfo(&s);
	printf("uptime     %lu\n", s.uptime);
	printf("loads      %lu %lu %lu\n", s.loads[0], s.loads[1], s.loads[2]);
	printf("total ram  %lu\n", s.totalram);
	printf("free ram   %lu\n", s.freeram);
	printf("shared ram %lu\n", s.sharedram);
	printf("buffer ram %lu\n", s.bufferram);
	printf("total swap %lu\n", s.totalswap);
	printf("free swap  %lu\n", s.freeswap);
	printf("procs      %u\n", s.procs);
	printf("total high %lu\n", s.totalhigh);
	printf("mem unit   %u\n", s.mem_unit);
}

int
main(void)
{
	printsysinfo();
	return 0;
}
