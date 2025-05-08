#include <u.h>
#include <libc.h>
#include "i2c.h"

int
detect(int start, int end, int cmd)
{
	I2C ic[1];
	int i, j, r, v;

	if (i2copen(ic, 1) < 0)
		return -1;

	print("     ");
	for (i = 0; i <= 0xf; i++)
		print("%x   ", i);
	print("\n");

	for (i = 0; i < 8; i++) {
		print("%02x: ", i);
		for (j = 0; j < 16; j++) {
			r = 16 * i + j;
			if (!(start <= r && r <= end)) {
				print("    ");
				continue;
			}

			if (i2csetslave(ic, r) >= 0 && (v = i2csmbrb(ic, cmd)) >= 0)
				print("%2x  ", v);
			else
				print("--  ");
		}
		print("\n");
	}

	i2cclose(ic);
	return 0;
}

void
main(int argc, char *argv[])
{
	int start, end, cmd;

	start = 0x03;
	end = 0x7f;
	cmd = 0x0f;

	ARGBEGIN
	{
	case 'c':
		cmd = strtol(ARGF(), nil, 0);
		break;
	case 'h':
	default:
		fprint(2, "usage: %s [-c cmd] <start> <end>\n", argv0);
		exits("usage");
		break;
	}
	ARGEND;

	if (argc >= 1)
		start = strtol(argv[0], nil, 0);
	if (argc >= 2)
		end = strtol(argv[1], nil, 0);

	if (detect(start, end, cmd) < 0)
		sysfatal("%r");
	exits(0);
}
