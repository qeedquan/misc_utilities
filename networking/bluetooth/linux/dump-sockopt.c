#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>

void
dump_l2cap_options(struct l2cap_options *o)
{
	printf("l2cap_options {\n");
	printf("\tomtu:        %d\n", o->omtu);
	printf("\timtu:        %d\n", o->imtu);
	printf("\tflush_to:    %d\n", o->flush_to);
	printf("\tmode:        %#x\n", o->mode);
	printf("\tfcs:         %d\n", o->fcs);
	printf("\tmax_tx:      %d\n", o->max_tx);
	printf("\ttxtwin_size: %d\n", o->txwin_size);
	printf("}\n");
}

void
dump_l2cap(void)
{
	struct l2cap_options l2o;
	int s;
	socklen_t len;

	s = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (s < 0)
		err(1, "socket");

	memset(&l2o, 0, sizeof(l2o));
	len = sizeof(l2o);
	if (getsockopt(s, SOL_L2CAP, L2CAP_OPTIONS, &l2o, &len) < 0)
		err(1, "getsockopt(L2CAP_OPTIONS)");

	dump_l2cap_options(&l2o);

	close(s);
}

void
dump_rc_conninfo(struct rfcomm_conninfo *c)
{
	printf("rfcomm_conninfo {\n");
	printf("\thci_handle : %d\n", c->hci_handle);
	printf("\tbluetooth  : (%d-%d-%d)\n", c->dev_class[0], c->dev_class[1], c->dev_class[2]);
	printf("}\n");
}

void
dump_rfcomm(const char *addr)
{
	struct rfcomm_conninfo conn;
	struct sockaddr_rc saddr = { 0 };
	socklen_t optlen;
	int s;

	s = socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if (s < 0)
		err(1, "socket");

	saddr.rc_family = PF_BLUETOOTH;
	saddr.rc_channel = 1;
	str2ba(addr, &saddr.rc_bdaddr);

	if (connect(s, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
		err(1, "connect");

	memset(&conn, 0, sizeof(conn));
	optlen = sizeof(conn);
	if (getsockopt(s, SOL_RFCOMM, RFCOMM_CONNINFO, &conn, &optlen) < 0)
		err(1, "getsockopt(RFCOMM_CONNINFO)");

	dump_rc_conninfo(&conn);

	close(s);
}

int
main(int argc, char *argv[])
{
	dump_l2cap();
	if (argc >= 2)
		dump_rfcomm(argv[1]);
	return 0;
}
