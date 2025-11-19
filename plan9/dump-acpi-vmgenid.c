/* Dumps QEMU VMGENID ACPI table entries */
#include <u.h>
#include <libc.h>
#include <aml.h>

typedef struct Tbl Tbl;

struct Tbl {
	uchar sig[4];
	uchar len[4];
	uchar rev;
	uchar csum;
	uchar oemid[6];
	uchar oemtid[8];
	uchar oemrev[4];
	uchar cid[4];
	uchar crev[4];
	uchar data[];
};

enum {
	Tblsz = 4 + 4 + 1 + 1 + 6 + 8 + 4 + 4 + 4,
};

ulong
get32(uchar *p)
{
	return p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0];
}

int
getdata(void *dot, void *type)
{
	void *r;

	if (amleval(dot, "", &r) != 0)
		return 1;

	switch ((int)(uintptr)type) {
	case 0:
		print("hid %s\n", amlval(r));
		break;

	case 1:
		print("cid %s\n", amlval(r));
		break;

	case 2:
		print("vgia %x\n", amlint(r));
		break;
	}

	return 1;
}

void
dump(void)
{
	Tbl *t;
	int fd, n, l;

	amlinit();
	t = nil;
	fd = open("/dev/acpitbls", OREAD);
	if (fd < 0)
		goto fail;

	for (;;) {
		t = malloc(sizeof(*t));
		if (t == nil) {
			werrstr("failed to allocate acpi table structure");
			goto fail;
		}

		if ((n = readn(fd, t, Tblsz)) <= 0)
			break;
		if (n != Tblsz) {
			werrstr("failed to read acpi table");
			goto fail;
		}

		l = get32(t->len);
		if (l < Tblsz) {
			werrstr("invalid acpi table length: %d bytes", l);
			goto fail;
		}
		l -= Tblsz;

		t = realloc(t, sizeof(*t) + l);
		if (t == nil) {
			werrstr("acpi table too big: %d bytes", l);
			goto fail;
		}

		if (readn(fd, t->data, l) != l) {
			werrstr("failed to read acpi table");
			goto fail;
		}

		if (memcmp("SSDT", t->sig, 4) == 0) {
			amlload(t->data, l);
			break;
		} else
			free(t);
	}

	amlenum(amlroot, "_HID", getdata, (void *)0);
	amlenum(amlroot, "_CID", getdata, (void *)1);
	amlenum(amlroot, "VGIA", getdata, (void *)2);
out:
	if (fd >= 0)
		close(fd);
	free(t);
	amlexit();
	return;

fail:
	fprint(2, "%r\n");
	goto out;
}

void
main(void)
{
	dump();
	exits(nil);
}
