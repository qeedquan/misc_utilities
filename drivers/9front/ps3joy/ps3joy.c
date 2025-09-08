/* userspace usb driver, binds over /dev/kbdin and translate the
 * controller data into configurable keypress events
 * information for init/led was gotten from LilyPAD in PCSX2 and the xboxdrv program */
#include <u.h>
#include <libc.h>
#include <bio.h>
#include <thread.h>
#include <ndb.h>
#include <keyboard.h>
#include "usb.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))

enum
{
	JoyCSP = 3,

	HidGetReport = 0x01,
	HidGetIdle   = 0x02,
	HidGetProto  = 0x03,
	HidSetReport = 0x09,
	HidSetIdle   = 0x0a,
	HidSetProto  = 0x0b,

	HidReportTypeInput   = 0x01,
	HidReportTypeOutput  = 0x02,
	HidReportTypeFeature = 0x03,
};

enum
{
	Js, Jt, Jx, Jo, Jsl, Jst,
	Jr1, Jl1, Jr2, Jl2, Jl3, Jr3,
	Ju, Jl, Jr, Jd, Jul, Jll, Jrl, Jdl,
	Jur, Jlr, Jrr, Jdr, Jmax,

	Jmaxstr = 8,
};

typedef struct Joy Joy;

struct Joy
{
	Dev *dev;
	Dev *ep;

	int  kbdinfd;
	Rune keys[Jmax][Jmaxstr];

	int delay;
};

static int interrupted;

static void
usage(void)
{
	fprint(2, "%s: [-c config] [-d delay] devid\n", argv0);
	threadexits("usage");
}

static void
joyfree(Joy *j)
{
	if (j == nil)
		return;
	if (j->kbdinfd >= 0)
		close(j->kbdinfd);
	if (j->dev)
		closedev(j->dev);
	if (j->ep)
		closedev(j->ep);
	free(j);
}

static void
sethipri(void)
{
	char fn[64];
	int fd;

	snprint(fn, sizeof(fn), "/proc/%d/ctl", getpid());
	fd = open(fn, OWRITE);
	if(fd < 0)
		return;
	fprint(fd, "pri 13");
	close(fd);
}

static void
joyfatal(Joy *j, char *fmt, ...)
{
	char err[ERRMAX], *sts;
	va_list ap;

	sts = nil;
	if (fmt != nil) {
		va_start(ap, fmt);
		vsnprint(err, sizeof(err), fmt, ap);
		va_end(ap);
		sts = err;
		fprint(2, "%s: fatal: %s\n", argv0, sts);
	} else
		fprint(2, "%s: exiting\n", argv0);

	joyfree(j);
	threadexits(sts);
}

static void
catch(void*, char *c)
{
	if (strstr(c, "alarm")) {
		interrupted = 1;
		noted(NCONT);
	}
	noted(NDFLT);
}

static void
initseq(Joy *j)
{
	uchar p[20];
	int i;

	for (i = 0; i < nelem(p); i++)
		p[i] = rand() & 0xff;

	usbcmd(j->dev, 0xa1, 1, 0x03f2, 0, p, 17);
}

static void
setled(Joy *j, int status)
{
	u8int cmd[] = {
		0x00, 0x00, 0x00, 0x00, 0x00,   /* rumble values */
		0x00, 0x00, 0x00, 0x00, status, /* 0x10=LED1 .. 0x02=LED4 */
		0xff, 0x27, 0x10, 0x00, 0x32,   /* LED 4 */
		0xff, 0x27, 0x10, 0x00, 0x32,   /* LED 3 */
		0xff, 0x27, 0x10, 0x00, 0x32,   /* LED 2 */
		0xff, 0x27, 0x10, 0x00, 0x32,   /* LED 1 */
		0x00, 0x00, 0x00, 0x00, 0x00,
	};

	usbcmd(j->dev, Rh2d|Rclass|Riface, HidSetReport, 
			(HidReportTypeOutput<<8) | 1, 0, cmd, sizeof(cmd));
}

static void
parse(Joy *j, uchar *p)
{
	static int button[] = {
		Jl2, Jr2, Jl1, Jr1, Jt, Jo, Jx, Js,
		Jsl, Jl3, Jr3, Jst, Ju, Jr, Jd, Jl,
	};
	static struct {
		int a, b;
	} axis[] = {
		{Jll, Jrl},
		{Jul, Jdl},
		{Jlr, Jrr},
		{Jur, Jdr},
	};
	static int threshold = 50;

	ulong x;
	int i;

	x = ((ulong)p[2])<<8 | (ulong)p[3];
	for (i = 0; i < nelem(button); i++) {
		if ((x & 1) && j->keys[button[i]][0])
			fprint(j->kbdinfd, "%S", j->keys[button[i]]);
		x >>= 1;
	}

	for (i = 0; i < nelem(axis); i++) {
		if (p[6+i] <= 0x80-threshold && j->keys[axis[i].a][0])
			fprint(j->kbdinfd, "%S", j->keys[axis[i].a]);
		else if (p[6+i] >= 0x80+threshold && j->keys[axis[i].b][0])
			fprint(j->kbdinfd, "%S", j->keys[axis[i].b]);
	}
}

static void
joywork(void *a)
{
	static int maxtries = 10;

	Joy *j;
	uchar p[64];
	int tries, n;

	j = a;
	threadsetname("ps3joy %s", j->ep->dir);
	sethipri();

	tries = 0;
	initseq(j);
	setled(j, 0x10);
	for (;;) {
		if (j->ep->maxpkt < 1 || j->ep->maxpkt > nelem(p))
			joyfatal(j, "joy: weird mouse maxpkt: %d bytes", j->ep->maxpkt);

		sleep(j->delay);

		alarm(2000);
		n = read(j->ep->dfd, p, j->ep->maxpkt);
		alarm(0);
		if (n < 0 || interrupted) {
			if (++tries >= maxtries)
				joyfatal(j, "joy: max read retries of %d times exceeded, aborting", maxtries);

			initseq(j);
			setled(j, 0x10);
			interrupted = 0;
			continue;
		}
		if (n >= 10)
			parse(j, p);
	}
}

/* we need mount the existing kbdfs to be able to write to
 * /dev/kbdin and broadcast, on startup rio binds over this device
 * and reject access to it unless we are the keyboard, so we need to
 * override rio */
static void
mountkbdin(void *a)
{
	char *m[] = {"/bin/mount", "-b", "/srv/cons", "/dev", nil };

	procexec(a, m[0], m);
	threadexits("can't exec");
}

static int
mapkey(Rune *r, char *s)
{
	static struct {
		char *key;
		Rune  val[Jmaxstr];
	} map[] = {
		{"Kdown", {Kdown}},
		{"Kup", {Kup}},
		{"Kleft", {Kleft}},
		{"Kright", {Kright}},
		{"Kshift", {Kshift}},
		{"Kbreak", {Kbreak}},
		{"Kdel", {Kdel}},
		{"Kreturn", {10}},
		{"disable", {0}},
	}, *m;

	Rune t[Jmaxstr];
	char *p;
	int i, l;

	for (i = 0; i < nelem(map); i++) {
		m = &map[i];
		if (cistrcmp(m->key, s) == 0) {
			runesnprint(r, Jmaxstr, "%S", m->val);
			return 0;
		}
	}

	p = s;
	for (i = 0; i < Jmaxstr; i++) {
		l = chartorune(&t[i], p);
		p += l;
		if (t[i] == '\0')
			break;
		if (t[i] == Runeerror) {
			werrstr("key value %q contains invalid UTF-8", s);
			return -1;
		}
	}
	if (i == Jmaxstr) {
		werrstr("key value %q is too long", s);
		return -1;
	}

	runesnprint(r, Jmaxstr, "%S", t);
	return 0;
}

static void
loadconfig(Joy *j, char *config)
{
	static struct {
		int   button;
		char  *name;
		Rune  key[Jmaxstr];
	} map[] = {
		{Ju, "up", {Kup}},
		{Jd, "down", {Kdown}},
		{Jl, "left", {Kleft}},
		{Jr, "right", {Kright}},
		{Jul, "lup", {Kup}},
		{Jdl, "ldown", {Kdown}},
		{Jll, "lleft", {Kleft}},
		{Jrl, "lright", {Kright}},
		{Jur, "rup", {Kup}},
		{Jdr, "rdown", {Kdown}},
		{Jlr, "rleft", {Kleft}},
		{Jrr, "rright", {Kright}},
		{Js, "square", {'x'}},
		{Jt, "triangle", {'s'}},
		{Jo, "circle", {'z'}},
		{Jx, "cross", {'a'}},
		{Jl1, "l1", {'q'}},
		{Jr1, "r1", {'w'}},
		{Jl2, "l2", {'e'}},
		{Jr2, "r2", {'r'}},
		{Jl3, "l3", {'d'}},
		{Jr3, "r3", {'c'}},
		{Jsl, "select", {Kshift}},
		{Jst, "start", {10}},
	}, *m;

	Ndb *ndb;
	Ndbtuple *t;
	int i;

	for (i = 0; i < nelem(j->keys); i++) {
		m = &map[i];
		runesnprint(j->keys[m->button], Jmaxstr, "%S", m->key);
	}

	if (config == nil)
		return;

	ndb = ndbopen(config);
	if (ndb == nil)
		return;

	for (; t = ndbparse(ndb); ndbfree(t)) {
		for (i = 0; i < nelem(map); i++) {
			m = &map[i];
			if (cistrcmp(t->attr, m->name) == 0)
				break;
		}
		if (i == nelem(map)) {
			fprint(2, "config: ignoring unknown key name %q", t->attr);
			continue;
		}

		if (mapkey(j->keys[m->button], t->val) < 0) {
			fprint(2, "config: invalid key mapping for %q: %r", t->attr);
			continue;
		}
	}

	ndbclose(ndb);
}

static void
joystart(Dev *d, Ep *ep, char *config, int delay, void (*f)(void*))
{
	Joy *j;
	Channel *c;

	c = chancreate(sizeof(int), 1);
	proccreate(mountkbdin, c, 4096);
	recv(c, nil);
	chanfree(c);

	j = emallocz(sizeof(Joy), 1);
	j->kbdinfd = -1;
	j->delay = max(delay, 0);

	loadconfig(j, config);

	incref(d);
	j->dev = d;
	j->ep = openep(j->dev, ep->id);
	if (j->ep == nil) {
		fprint(2, "%s: %s: openep %d: %r\n", argv0, d->dir, ep->id);
		goto Err;
	}

	if (opendevdata(j->ep, OREAD) < 0) {
		fprint(2, "%s: %s: opendevdata: %r\n", argv0, j->ep->dir);
		goto Err;
	}

	j->kbdinfd = open("/dev/kbdin", OWRITE);
	if (j->kbdinfd < 0) {
		fprint(2, "open: %r\n");
		goto Err;
	}

	notify(catch);

	f(j);
	return;

Err:
	joyfree(j);
}

void
threadmain(int argc, char *argv[])
{
	Dev *d;
	Ep *ep;
	Usbdev *ud;
	char *home, *keymap;
	long len;
	int delay, i;

	keymap = nil;
	delay = 50;
	ARGBEGIN {
	case 'd':
		delay = atoi(ARGF());
		break;
	case 'c':
		keymap = ARGF();
		break;
	default:
		usage();
	} ARGEND;

	if (argc != 1)
		usage();

	if (keymap == nil) {
		home = getenv("home");
		if (home != nil) {
			len = strlen(home) + 32;
			keymap = emallocz(len, 1);
			snprint(keymap, len, "%s/lib/ps3joy.cfg", home);
			free(home);
		}
	}

	d = getdev(*argv);
	if (d == nil)
		sysfatal("getdev: %r");
	ud = d->usb;
	ep = nil;

	if (ud->vid != 0x054c || ud->did != 0x0268)
		sysfatal("usb device %x:%x do not match a ps3 controller", ud->vid, ud->did);

	for (i = 0; i < nelem(ud->ep); i++) {
		if ((ep = ud->ep[i]) == nil)
			continue;

		if (ep->type == Eintr && ep->dir == Ein && ep->iface->csp == JoyCSP)
			break;
	}
	if (ep == nil)
		sysfatal("no suitable endpoint found");

	joystart(d, ep, keymap, delay, joywork);
	threadexits(nil);
}

