/* VMXNET3 driver, ported from OpenBSD driver,
   needs *nomsi=1 because this driver does not handle
   msi interrupts and need traditional interrupts to 
   receive/transmit
 */

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"
#include "../port/error.h"
#include "../port/netif.h"

#include "etherif.h"

#define IMASK(i)	(0x000 + (i) * 8)
#define TXH(q)		(0x600 + (q) * 8)
#define RXH1(q)		(0x800 + (q) * 8)
#define RXH2(q)		(0xa00 + (q) * 8)

enum
{
	Vrrs	= 0x00,
	Uvrs	= 0x08,
	DsL		= 0x10,
	DsH		= 0x18,
	Cmd		= 0x20,
	MacL	= 0x28,
	MacH	= 0x30,
	Intr	= 0x38,
	Event	= 0x40,
};

enum
{
	CmdEnable		= 0xcafe0000,
	CmdDisable		= 0xcafe0001,
	CmdReset		= 0xcafe0002,
	CmdSetRxMode	= 0xcafe0003,
	CmdSetFilter	= 0xcafe0004,
	CmdGetStatus	= 0xf00d0000,
	CmdGetStats		= 0xf00d0001,
	CmdGetLink		= 0xf00d0002,
	CmdGetMacL		= 0xf00d0003,
	CmdGetMacH		= 0xf00d0004,
};

enum
{
	IctlrDisableAll	= 0x1,
};

enum
{
	Upt1Csum	= 0x1,
	Upt1Rss		= 0x2,
	Upt1Vlan	= 0x4,
	Upt1Lro		= 0x8,
};

enum
{
	Upt1ImodNone		= 0,
	Upt1ImodHighest		= 7,
	Upt1ImodAdaptive	= 8,
};

enum
{
	Rbsz	= 9*1024,
	MaxMtu	= 9000,
	MinMtu	= 60,
};

enum
{
	EventRqError	= 0x1,
	EventTqError	= 0x2,
	EventLink		= 0x4,
	EventDic		= 0x8,
	EventDebug		= 0x10,
};

enum
{
	TxLenM		= 0x3fff,
	TxLenS		= 0,
	TxGenM		= 0x1,
	TxGenS		= 14,
	TxRes0		= 0x8000,
	TxDtypeM	= 0x1,
	TxDtypeS	= 16,
	TxRes1		= 0x2,
	TxOpM		= 0x3fff,
	TxOpS		= 18,

	TxHlenM		= 0x3ff,
	TxHlenS		= 0,
	TxOmM		= 0x3,
	TxOmS		= 10,
	TxEop		= 0x1000,
	TxCompReq	= 0x2000,
	TxRes2		= 0x4000,
	TxVtagMode	= 0x8000,
	TxVlanTagM	= 0xffff,
	TxVlanTagS	= 16,
};

enum
{
	TxcEopIdxM	= 0xfff,
	TxcEopIdxS	= 0,
	TxcRes0M	= 0xfffff,
	TxcRes0S	= 12,

	TxcRes2M	= 0xffffff,
	TxcTypeM	= 0x7f,
	TxcTypeS	= 24,
	TxcGenM		= 0x1,
	TxcGenS		= 31,
};

enum
{
	RxLenM		= 0x3fff,
	RxLenS		= 0,
	RxBtypeM	= 0x1,
	RxBtypeS	= 14,
	RxDtypeM	= 0x1,
	RxDtypeS	= 15,
	RxRes0M		= 0x7fff,
	RxRes0S		= 16,
	RxGenM		= 0x1,
	RxGenS		= 31,
};

enum
{
	RxcIdxM		= 0xfff,
	RxcIdxS		= 0,
	RxcRes0M	= 0x3,
	RxcResS		= 12,
	RxcEop		= 0x4000,
	RxcSop		= 0x8000,
	RxcQidM		= 0x3ff,
	RxcQidS		= 16,
	RxcRssTypeM	= 0xf,
	RxcRssTypeS	= 26,
	RxcNoCsum	= 0x40000000,
	RxcRes1		= 0x80000000,

	RxcRssHashM	= 0xffffffff,
	RxcRssHashS	= 0,
	RxcLenM		= 0x3fff,
	RxcLenS		= 0,
	RxcError	= 0x4000,
	RxcVlan		= 0x8000,
	RxcVlanTagM	= 0xffff,
	RxcVlanTagS	= 16,

	RxcCsumM	= 0xffff,
	RxcCsumS	= 16,
	RxcCsumOk	= 0x10000,
	RxcUdp		= 0x20000,
	RxcTcp		= 0x40000,
	RxcIpSumOk	= 0x80000,
	RxcIpv6		= 0x100000,
	RxcIpv4		= 0x200000,
	RxcFragment	= 0x400000,
	RxcFcs		= 0x800000,
	RxcTypeM	= 0x7f000000,
	RxcGenM		= 0x1,
	RxcGenS		= 31,
};

enum
{
	RxModeUcast		= 0x1,
	RxModeMcast		= 0x2,
	RxModeBcast		= 0x4,
	RxModeAllMulti	= 0x8,
	RxModePromisc	= 0x10,
};

enum
{
	BtypeHead	= 0,
	BtypeBody	= 1,
};

enum
{
	Rev	= 0xbabefee1,
	Ver	= 0x10000,
};

enum
{
	GosUnknown	= 0x00,
	GosLinux	= 0x04,
	GosWindows	= 0x08,
	GosSolaris	= 0x0c,
	GosFreebsd	= 0x10,
	GosPxe		= 0x14,

	Gos32	= 0x01,
	Gos64	= 0x02,
};

enum
{
	Nintr	= 1,

	Nrxq	= 1,
	Ntxq	= 1,
};

enum
{
	Ntxd	= 128,
	Ntxs	= 8,
	Nrxd	= 128,
	Ntxcd	= Ntxd,
	Nrxcd	= Nrxd*2,
};

enum
{
	Txsz	= 256,
	Txdsz	= 16,
	Txcdsz	= 16,
	Rxsz	= 256,
	Rxdsz	= 16,
	Rxcdsz	= 16,
	Dssz	= 720,
	Qssz	= Txsz*Ntxq + Rxsz*Nrxq,
};

typedef struct Ctlr Ctlr;
typedef struct TXQ TXQ;
typedef struct RXQ RXQ;
typedef struct Td Td;
typedef struct Rd Rd;
typedef struct Cd Cd;

struct Td
{
	Block **bp;
	uchar *desc;
	uint head;
	uint next;
	uint gen;
};

struct Rd
{
	Block **bp;
	uchar *desc;
	uint fill;
	uint rid;
	uint next;
	uint gen;
};

struct Cd
{
	uchar *desc;
	uint next;
	uint gen;
};

struct TXQ
{
	uchar *ts;
	Td cmd;
	Cd comp;
};

struct RXQ
{
	uchar *rs;
	Rd cmd[2];
	Cd comp;
};

struct Ctlr
{
	Lock tlock, ilock;
	QLock alock, slock;

	Ctlr *link;
	Pcidev *pdev;
	u32int *mmio[2];

	int attached;
	int active;
	int port;

	uchar *ds;
	uchar *qs;
	uchar *mcast;

	TXQ tx[Ntxq];
	RXQ rx[Nrxq];
};

static Ctlr *ctlrhead, *ctlrtail;

#define csr32r(c, m, r) 	(*((c)->mmio[(m)]+((r)/4)))
#define csr32w(c, m, r, v)	(*((c)->mmio[(m)]+((r)/4)) = (v))
#define vcmd(c, v)			(csr32w(c, 1, Cmd, v))

static void
put4(uchar *p, u32int v)
{
	p[0] = v & 0xff;
	p[1] = (v >> 8) & 0xff;
	p[2] = (v >> 16) & 0xff;
	p[3] = (v >> 24) & 0xff;
}

static void
put8(uchar *p, u64int v)
{
	p[0] = v & 0xff;
	p[1] = (v >> 8) & 0xff;
	p[2] = (v >> 16) & 0xff;
	p[3] = (v >> 24) & 0xff;
	p[4] = (v >> 32) & 0xff;
	p[5] = (v >> 40) & 0xff;
	p[6] = (v >> 48) & 0xff;
	p[7] = (v >> 56) & 0xff;
}

static u32int
get4(uchar *p)
{
	return p[0] | p[1]<<8 | p[2]<<16 | p[3]<<24;
}

static u64int
get8(uchar *p)
{
	return (u64int)p[0] | (u64int)p[1]<<8 | (u64int)p[2]<<16 | (u64int)p[3]<<24 |
			(u64int)p[4]<<32 | (u64int)p[5]<<40 | (u64int)p[6]<<48 | (u64int)p[7]<<56;
}

static void
vsple(Ctlr *ctlr, int i)
{
	csr32w(ctlr, 0, IMASK(i), 0);
}

static void
vspld(Ctlr *ctlr, int i)
{
	csr32w(ctlr, 0, IMASK(i), 1);
}

static void
vsplhi(Ctlr *ctlr)
{
	uchar *p;
	uint i;

	p = ctlr->ds+108;
	put4(p, get4(p) | IctlrDisableAll);
	for (i = 0; i < Nintr; i++)
		vspld(ctlr, i);
}

static void
vspllo(Ctlr *ctlr)
{
	uchar *p;
	uint i;

	p = ctlr->ds+108;
	put4(p, get4(p) & ~IctlrDisableAll);
	for (i = 0; i < Nintr; i++)
		vsple(ctlr, i);
}

static void
stop(Ctlr *ctlr)
{
	vsplhi(ctlr);
	vcmd(ctlr, CmdDisable);
}

static void
shutdown(Ether *edev)
{
	stop(edev->ctlr);
}

static char*
inittx(TXQ *tx)
{
	uchar *p;

	if (tx->cmd.desc == nil)
		tx->cmd.desc = mallocalign(Ntxd*Txdsz, 512, 0, 0);

	if (tx->comp.desc == nil)
		tx->comp.desc = mallocalign(Ntxcd*Txcdsz, 512, 0, 0);

	if (tx->cmd.bp == nil)
		tx->cmd.bp = malloc(Ntxcd*sizeof(Block*));

	if (tx->cmd.desc == nil || tx->comp.desc == nil || tx->cmd.bp == nil)
		return "failed to allocate tx ring";

	tx->cmd.head = 0;
	tx->cmd.next = 0;
	tx->cmd.gen = 1;

	tx->comp.next = 0;
	tx->comp.gen = 1;

	p = tx->ts;
	memset(p, 0, Txsz);
	p += 4; /* num pending */
	put4(p, 1); /* interrupt threshold */
	p += 4;
	p += 8; /* reserved1 */
	put8(p, PCIWADDR(tx->cmd.desc)); /* cmd ring */
	p += 8;
	p += 8; /* data ring */
	put8(p, PCIWADDR(tx->comp.desc)); /* comp ring */
	p += 8;
	put8(p, PCIWADDR(tx)); /* driver data */
	p += 8;
	p += 8; /* reserved2 */
	put4(p, Ntxd); /* cmd ring len */
	p += 4;
	put4(p, sizeof(*tx)); /* driver data length */
	p += 4;
	put4(p, Ntxcd); /* comp ring length */
	p += 4;
	p++; /* interrupt index */
	p += 7; /* pad1 */
	*p = 1; /* stopped */

	return nil;
}

static int
mkrxbuf(Rd *r)
{
	Block *bp;
	uchar *p;
	uint n;
	u32int v, len;
	u64int addr;

	if (r->rid != 0)
		return -1;

	n = r->fill;
	r->bp[n] = bp = iallocb(Rbsz);

	addr = 0;
	len = 0;
	if (bp != nil) {
		addr = PCIWADDR(bp->rp);
		len = Rbsz;
	}

	p = r->desc + n*Rxdsz;
	put8(p, addr); /* addr */
	p += 8;

	v = (len & RxLenM) << RxLenS;
	v |= ((BtypeHead & RxBtypeM) << RxBtypeS);
	v |= ((r->gen & RxGenM) << RxGenS);
	put4(p, v);

	if (++n == Nrxd) {
		n = 0;
		r->gen ^= 1;
	}
	r->fill = n;
	return 0;
}

static char*
initrx(RXQ *rx)
{
	uchar *p;
	uint i, j;

	for (i = 0; i < nelem(rx->cmd); i++) {
		if (rx->cmd[i].desc == nil)
			rx->cmd[i].desc = mallocalign(Nrxd*Rxdsz, 512, 0, 0);

		if (rx->cmd[i].bp == nil)
			rx->cmd[i].bp = malloc(Nrxd*sizeof(Block*));
	}

	if (rx->comp.desc == nil)
		rx->comp.desc = mallocalign(Nrxcd*Rxcdsz, 512, 0, 0);

	for (i = 0; i < nelem(rx->cmd); i++) {
		if (rx->cmd[i].desc == nil || rx->cmd[i].bp == nil)
			return "failed to allocate rx ring";
	}
	if (rx->comp.desc == nil)
		return "failed to allocate rx ring";

	for (i = 0; i < nelem(rx->cmd); i++) {
		rx->cmd[i].fill = 0;
		rx->cmd[i].gen = 1;
		rx->cmd[i].rid = i;
		for (j = Nrxd; j > 0; j--)
			mkrxbuf(&rx->cmd[i]);
	}
	rx->comp.next = 0;
	rx->comp.gen = 1;

	p = rx->rs;
	memset(p, 0, Rxsz);
	p++; /* update rx head */
	p += 7; /* pad1 */
	p += 8; /* reserved 1 */
	put8(p, PCIWADDR(rx->cmd[0].desc)); /* cmd ring 0 */
	p += 8;
	put8(p, PCIWADDR(rx->cmd[1].desc)); /* cmd ring 1 */
	p += 8;
	put8(p, PCIWADDR(rx->comp.desc)); /* comp ring */
	p += 8;
	put8(p, PCIWADDR(rx)); /* driver data */
	p += 8;
	p += 8; /* reserved2 */
	put4(p, Nrxd); /* cmd ring len 0 */
	p += 4;
	put4(p, Nrxd); /* cmd ring len 1 */
	p += 4;
	put4(p, Nrxcd); /* comp ring length */
	p += 4;
	put4(p, sizeof(*rx)); /* driver data len */
	p++; /* interrupt index */
	p += 7; /* pad2 */
	*p = 1; /* stopped */

	return nil;
}

static char*
initring(Ctlr *ctlr)
{
	char *err;
	uchar *p;
	uint i;

	if (ctlr->ds == nil)
		ctlr->ds = mallocalign(Dssz, 8, 0, 0);

	if (ctlr->qs == nil)
		ctlr->qs = mallocalign(Qssz, 128, 0, 0);

	if (ctlr->mcast == nil)
		ctlr->mcast = mallocalign(682*Eaddrlen, 32, 0, 0);

	if (ctlr->ds == nil || ctlr->qs == nil || ctlr->mcast == nil)
		return "failed to allocate dma memory";

	p = ctlr->qs;
	for (i = 0; i < Ntxq; i++) {
		ctlr->tx[i].ts = p;
		if ((err = inittx(&ctlr->tx[i])) != nil)
			return err;
		p += Txsz;
	}

	for (i = 0; i < Nrxq; i++) {
		ctlr->rx[i].rs = p;
		if ((err = initrx(&ctlr->rx[i])) != nil)
			return err;
		p += Rxsz;
	}

	p = ctlr->ds;
	put4(p, Rev); /* magic */
	p += 4;
	p += 4; /* pad1 */
	put4(p, Ver); /* version */
	p += 4;
	put4(p, GosUnknown | Gos64); /* guest */
	p += 4;
	put4(p, 1); /* revision */
	p += 4;
	put4(p, 1); /* upt version */
	p += 4;
	put8(p, 0); /* upt features */
	p += 8;
	put8(p, PCIWADDR(ctlr)); /* driver data */
	p += 8;
	put8(p, PCIWADDR(ctlr->qs)); /* queue shared data */
	p += 8;
	put4(p, sizeof(*ctlr)); /* driver data len */
	p += 4;
	put4(p, Ntxq*Txsz + Nrxq*Rxsz); /* queue shared len */
	p += 4;
	put4(p, MaxMtu); /* max mtu */
	p += 4;
	p += 2; /* nrxsg max */
	*p++ = Ntxq; /* ntxqueue */
	*p++ = Nrxq; /* rtxqueue */
	p += 4*4; /* reserved1 */
	*p++ = 1; /* automask */
	*p++ = 1; /* nintr */
	*p++ = 0; /* evintr */
	for (i = 0; i < Nintr; i++)
		*p++ = Upt1ImodAdaptive; /* modlevel */
	put4(p, IctlrDisableAll); /* ictlr */
	p += 4;
	p += 4*2; /* reserved2 */
	p += 4; /* rxmode */
	p += 2; /* mcast table len */
	p += 2; /* pad2 */
	put8(p, PCIWADDR(ctlr->mcast)); /* mcast table */

	csr32w(ctlr, 1, DsL, PCIWADDR(ctlr->ds));
	csr32w(ctlr, 1, DsH, PCIWADDR(ctlr->ds)>>32);

	return nil;
}

static void
viff(Ctlr *ctlr)
{
	vcmd(ctlr, CmdSetFilter);
	put4(ctlr->ds+120, RxModeBcast | RxModeUcast);
	vcmd(ctlr, CmdSetRxMode);
}

static char*
reset(Ctlr *ctlr)
{
	char *err;
	uint i;

	if ((err = initring(ctlr)) != nil)
		return err;

	stop(ctlr);
	vcmd(ctlr, CmdEnable);
	if (csr32r(ctlr, 1, Cmd)) {
		stop(ctlr);
		return "failed to initialize link";
	}

	for (i = 0; i < Nrxq; i++) {
		csr32w(ctlr, 0, RXH1(i), 0);
		csr32w(ctlr, 0, RXH2(i), 0);
	}

	viff(ctlr);
	vspllo(ctlr);

	return nil;
}

static void
transmit(Ether *edev)
{
	Ctlr *ctlr;
	Td *t;
	Block *bp;
	uchar w, *h, *p;
	uint gen;

	ctlr = edev->ctlr;
	ilock(&ctlr->tlock);

	t = &ctlr->tx[0].cmd;
	gen = t->gen ^ 1;
	h = p = t->desc + t->head*Txdsz;
	for (w = 0;; w = 1) {
		if (((t->next - t->head - 1) % Ntxd) < Ntxs)
			break;
		bp = qget(edev->oq);
		if (bp == nil)
			break;

		t->bp[t->head] = bp;
		p = t->desc + t->head*Txdsz;
		put8(p, PCIWADDR(bp->rp)); /* tx addr */
		put4(p+8, ((BLEN(bp) & TxLenM) << TxLenS) | ((gen & TxGenM) << TxGenS)); /* txword2 */
		put4(p+12, 0); /* txword3 */

		if (++t->head == Ntxd) {
			t->head = 0;
			t->gen ^= 1;
		}
		gen = t->gen;		
	}
	put4(p+12, TxEop | TxCompReq);
	put4(h+8, get4(h+8) ^ (TxGenM << TxGenS));

	if (w)
		csr32w(ctlr, 0, TXH(0), t->head);

	iunlock(&ctlr->tlock);
}

static void
event(Ctlr *ctlr)
{
	u32int v;

	v = get4(ctlr->ds+696);
	csr32w(ctlr, 1, Event, v);
	if (v & (EventTqError | EventRqError)) {
		vcmd(ctlr, CmdGetStatus);
		reset(ctlr);
	}
}

static void
receive(Ether *edev, RXQ *rx)
{
	Ctlr *ctlr;
	Cd *c;
	Rd *r;
	Block *bp;
	uchar *p, *q;
	u32int w0, w2, w3, qid;
	uint idx, i, j, n;

	ctlr = edev->ctlr;
	c = &rx->comp;
	for (j = 0;; j++) {
		p = c->desc + c->next*Rxcdsz;
		w3 = get4(p+12);
		if (((w3 >> RxcGenS) & RxcGenM) != c->gen)
			break;

		if (++c->next == Nrxcd) {
			c->next = 0;
			c->gen ^= 1;
		}

		w0 = get4(p);
		idx = (w0 >> RxcIdxS) & RxcIdxM;
		if (((w0 >> RxcQidS) & RxcQidM) < Nrxq)
			r = &rx->cmd[0];
		else
			r = &rx->cmd[1];

		w2 = get4(p+8);
		n = (w2 >> RxcLenS) & RxcLenM;

		q = r->desc + idx*Rxdsz;
		bp = r->bp[idx];

		if (((get4(q+8) >> RxBtypeS) & RxBtypeM) != BtypeHead)
			goto skip;

		if (w2 & RxcError)
			goto skip;

		if (n < MinMtu)
			goto skip;

		bp->wp += n;
		etheriq(edev, bp, 1);
		r->bp[idx] = nil;

	skip:
		if (r->bp[idx] != nil) {
			freeb(r->bp[idx]);
			r->bp[idx] = nil;
		}

		if (get4(rx->rs)) {
			qid = (w0 >> RxcQidS) & RxcQidM;
			idx = (idx + 1) % Nrxd;
			if (qid < Nrxq)
				csr32w(ctlr, 0, RXH1(qid), idx);
			else {
				qid -= Nrxq;
				csr32w(ctlr, 0, RXH2(qid), idx);
			}
		}
	}

	for (i = 0; i < j; i++)
		mkrxbuf(&rx->cmd[0]);
}

static void
transmitted(Ether *edev, TXQ *tx)
{
	Cd *c;
	Td *t;
	uchar *p;
	u32int w0, w3;
	uint sop;

	c = &tx->comp;
	t = &tx->cmd;
	for (;;) {
		p = c->desc + c->next*Txcdsz;

		w3 = get4(p+12);
		if (((w3 >> TxcGenS) & TxcGenM) != c->gen)
			break;

		if (++c->next == Ntxcd) {
			c->next = 0;
			c->gen ^= 1;
		}

		sop = t->next;
		if (t->bp[sop] == nil)
			panic("vmxnet3: null tx ring: %ud", sop);
		freeb(t->bp[sop]);
		t->bp[sop] = nil;

		w0 = get4(p);
		t->next = (((w0 >> TxcEopIdxS) & TxcEopIdxM) + 1) % Ntxd;
	}
	transmit(edev);
}

static void
interrupt(Ureg*, void *a)
{
	Ether *edev;
	Ctlr *ctlr;

	edev = a;
	ctlr = edev->ctlr;

	ilock(&ctlr->ilock);
	if (csr32r(ctlr, 1, Intr) == 0) {
		iunlock(&ctlr->ilock);
		return;
	}

	if (get4(ctlr->ds+696))
		event(ctlr);

	receive(edev, &ctlr->rx[0]);
	transmitted(edev, &ctlr->tx[0]);
	vsple(ctlr, 0);
	iunlock(&ctlr->ilock);
}

static long
ifstat(Ether *edev, void *buf, long n, ulong offset)
{
	static char *txs[] = {
		"TSO Packets Transmitted",
		"TSO Bytes Transmitted",
		"Unicast Packets Transmitted",
		"Unicast Bytes Transmitted",
		"Multicast Packets Transmitted",
		"Multicast Bytes Transmitted",
		"Broadcast Packets Transmitted",
		"Broadcast Bytes Transmitted",
		"Packets Transmitted Error",
		"Packets Transmitted Discarded",
	};

	static char *rxs[] = {
		"LRO Packets Received",
		"LRO Bytes Received",
		"Unicast Packets Received",
		"Unicast Bytes Received",
		"Multicast Packets Received",
		"Multicast Bytes Received",
		"Broadcast Packets Received",
		"Packets Received Out of Band",
		"Packets Received Error",
	};

	Ctlr *ctlr;
	char *s;
	uchar *qs, *p;
	long l;
	uint i, j;

	s = smalloc(READSTR);
	ctlr = edev->ctlr;
	qlock(&ctlr->slock);

	vcmd(ctlr, CmdGetStats);
	qs = ctlr->qs;
	l = 0;
	for (i = 0; i < Ntxq; i++) {
		l += snprint(s+l, READSTR-l, "Tx Queue #%ud\n", i+1);
		p = qs + 88;
		for (j = 0; j < nelem(txs); j++) {
			l += snprint(s+l, READSTR-l, "%s: %llud\n", txs[j], get8(p));
			p += 8;
		}
		qs += Txsz;
		l += snprint(s+l, READSTR-l, "\n");
	}

	for (i = 0; i < Nrxq; i++) {
		l += snprint(s+l, READSTR-l, "Rx Queue #%ud\n", i+1);
		p = qs + 88;
		for (j = 0; j < nelem(rxs); j++) {
			l += snprint(s+l, READSTR-l, "%s: %llud\n", rxs[j], get8(p));
			p += 8;
		}
		qs += Rxsz;
		if (i != Nrxq-1)
			l += snprint(s+l, READSTR-l, "\n");
	}

	n = readstr(offset, buf, n, s);
	free(s);
	qunlock(&ctlr->slock);

	return n;
}

static void
attach(Ether *edev)
{
	Ctlr *ctlr;
	char *err;

	ctlr = edev->ctlr;
	qlock(&ctlr->alock);
	if (waserror()) {
		print("#l%d: %s\n", edev->ctlrno, up->errstr);
		qunlock(&ctlr->alock);
		nexterror();
	}

	if (ctlr->attached == 0) {
		if (err = reset(ctlr))
			error(err);

		ctlr->attached = 1;
	}

	qunlock(&ctlr->alock);
	poperror();
}

static void
pci(void)
{
	Ctlr *ctlr;
	Pcidev *pdev;
	void *mem[2];
	uint i;

	pdev = nil;
	while (pdev = pcimatch(pdev, 0, 0)) {
		if (pdev->ccrb != 0x02 || pdev->ccru != 0)
			continue;

		if (pdev->vid != 0x15ad || pdev->did != 0x7b0)
			continue;

		memset(mem, 0, sizeof(*mem));
		ctlr = nil;

		for (i = 0; i < nelem(mem); i++) {
			mem[i] = vmap(pdev->mem[i].bar & ~0xf, pdev->mem[i].size);
			if (mem[i] == nil) {
				print("vmxnet3: can't map BAR%d at address %8.8luX\n", i, pdev->mem[i].bar);
				goto error;
			}
		}

		ctlr = malloc(sizeof(*ctlr));
		if (ctlr == nil) {
			print("vmxnet3: can't allocate Ctlr\n");
			goto error;
		}

		ctlr->pdev = pdev;
		for (i = 0; i < nelem(mem); i++)
			ctlr->mmio[i] = mem[i];
		ctlr->port = pdev->mem[0].bar & ~0xf;

		if (ctlrhead != nil)
			ctlrtail->link = ctlr;
		else
			ctlrhead = ctlr;
		ctlrtail = ctlr;
		continue;

	error:
		for (i = 0; i < nelem(mem); i++) {
			if (mem[i] != nil)
				vunmap(mem[i], pdev->mem[i].size);
		}
		free(ctlr);
	}
}

static int
init(Ether *edev)
{
	Ctlr *ctlr;
	u32int ver, macl, mach;
	uint i;

	ctlr = edev->ctlr;

	ver = csr32r(ctlr, 1, Vrrs);
	if ((ver & 0x1) == 0) {
		print("vmxnet3: unsupported hardware version %x\n", ver);
		return -1;
	}
	csr32w(ctlr, 1, Vrrs, 1);

	ver = csr32r(ctlr, 1, Uvrs);
	if ((ver & 0x1) == 0) {
		print("vmxnet3: incompatible UPT version %x\n", ver);
		return -1;
	}
	csr32w(ctlr, 1, Uvrs, 1);

	vcmd(ctlr, CmdGetMacL);
	macl = csr32r(ctlr, 1, Cmd);
	vcmd(ctlr, CmdGetMacH);
	mach = csr32r(ctlr, 1, Cmd);

	for (i = 0; i < 4; i++)
		edev->ea[i] = (macl>>(8*i)) & 0xff;
	for (i = 0; i < 2; i++)
		edev->ea[4+i] = (mach>>(8*i)) & 0xff;

	csr32w(ctlr, 1, MacL, macl);
	csr32w(ctlr, 1, MacH, mach);

	vcmd(ctlr, CmdGetLink);
	edev->mbps = csr32r(ctlr, 1, Cmd) >> 16;

	return 0;
}

static int
pnp(Ether *edev)
{
	Ctlr *ctlr;

	if (ctlrhead == nil)
		pci();

again:
	for (ctlr = ctlrhead; ctlr != nil; ctlr = ctlr->link) {
		if (ctlr->active)
			continue;
		if (edev->port == 0 || edev->port == ctlr->port) {
			ctlr->active = 1;
			break;
		}
	}

	if (ctlr == nil)
		return -1;

	edev->arg = edev;
	edev->ctlr = ctlr;
	edev->port = ctlr->port;
	edev->irq = ctlr->pdev->intl;
	edev->tbdf = ctlr->pdev->tbdf;

	edev->attach = attach;
	edev->transmit = transmit;
	edev->ifstat = ifstat;
	edev->shutdown = shutdown;
	edev->interrupt = interrupt;

	if (init(edev) < 0) {
		edev->ctlr = nil;
		goto again;
	}

	return 0;
}

void
ethervmxnet3link(void)
{
	addethercard("vmxnet3", pnp);
}

