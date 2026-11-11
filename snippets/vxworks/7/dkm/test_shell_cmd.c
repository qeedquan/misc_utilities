// call various shell commands from C

#include <vxWorks.h>
#include <ipnet.h>

typedef int (*Func)(int, char **);

typedef struct {
	Func func;
	int argc;
	char *argv[8];
} Cmd;

void
execcmd(Cmd *cmds, size_t ncmd)
{
	Cmd *c;
	size_t i;

	for (i = 0; i < ncmd; i++) {
		c = cmds + i;
		c->func(c->argc, c->argv);
	}
}

void
ipcmd(void)
{
	Cmd cmds[] = {
		{ ipnet_cmd_ifconfig, 2, { "ifconfig", "-a" } },
		{ ipnet_cmd_ping, 2, { "ping", "127.0.0.1" } },
	};

	execcmd(cmds, NELEMENTS(cmds));
}
