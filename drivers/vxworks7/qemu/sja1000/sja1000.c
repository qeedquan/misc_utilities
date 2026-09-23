/*

SJA1000 CAN controller driver
Driver only supports PeliCAN mode

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <vxLib.h>
#include <sysLib.h>
#include <spinLockLib.h>
#include <hwif/vxBus.h>
#include <hwif/util/vxbIsrDeferLib.h>
#include <canDevLib.h>
#include "sja1000.h"

#define csr8r(ctlr, reg) ctlr->read(ctlr, reg)
#define csr8w(ctlr, reg, val) ctlr->write(ctlr, reg, val)

LOCAL int sja1000xmit(CAN_DEV *, CAN_MSG *);
LOCAL int sja1000ioctl(CAN_DEV *, int, caddr_t);
LOCAL void sja1000writecmdreg(SJA1000_CTRL *, unsigned char);
LOCAL void sja1000changestate(SJA1000_CTRL *, CAN_MSG *, enum can_state, enum can_state);
LOCAL void sja1000task(CAN_DEV *pCanDev);

LOCAL CAN_FUNCS sja1000netfuncs = {
	sja1000ioctl,
	sja1000xmit,
	NULL,
	NULL
};

LOCAL int
txstate2frame(enum can_state state)
{
	switch (state) {
	case CAN_STATE_ERROR_ACTIVE:
		return CAN_ERR_CRTL_ACTIVE;
	case CAN_STATE_ERROR_WARNING:
		return D_CAN_ERR_CRTL_TX_WARNING;
	case CAN_STATE_ERROR_PASSIVE:
		return D_CAN_ERR_CRTL_TX_PASSIVE;
	default:
		return 0;
	}
}

LOCAL int
rxstate2frame(enum can_state state)
{
	switch (state) {
	case CAN_STATE_ERROR_ACTIVE:
		return CAN_ERR_CRTL_ACTIVE;
	case CAN_STATE_ERROR_WARNING:
		return D_CAN_ERR_CRTL_RX_WARNING;
	case CAN_STATE_ERROR_PASSIVE:
		return D_CAN_ERR_CRTL_RX_PASSIVE;
	default:
		return 0;
	}
}

LOCAL CAN_DEV_STAT
sja1000statusget(SJA1000_CTRL *ctlr)
{
	UINT8 status;

	status = csr8r(ctlr, SJA1000_SR);
	if (status & SJA1000_SR_BS)
		return CAN_STATE_BUS_OFF;
	if (status & SJA1000_SR_ES)
		return CAN_STATE_ERROR_WARNING;
	return CAN_STATE_ERROR_ACTIVE;
}

UINT
sja1000getbaudrate(SJA1000_CTRL *ctlr, UINT *samplepoint)
{
	UINT quanta, brp, tseg1, tseg2;

	brp = (csr8r(ctlr, SJA1000_BTR0) & 0x3f) + 1;
	tseg1 = csr8r(ctlr, SJA1000_BTR1);
	tseg2 = ((tseg1 & 0x70) >> 4) + 1;
	tseg1 = (tseg1 & 0x0f) + 1;

	quanta = (1 + tseg1 + tseg2);
	*samplepoint = ((1 + tseg1) * 100) / quanta;
	return ((16000000 / 2) / ((quanta * brp) * 2));
}

LOCAL void
sja1000setbaudrate(SJA1000_CTRL *ctlr, UINT32 baudrate)
{
	static const struct btr {
		UINT32 baudrate;
		UINT8 btr1;
		UINT8 btr0;
	} btrs[] = {
		// Baudrate(K)   BTR1      BTR0
		{ 1000, 0x14, SJA1000_BTR_SJW_0 | 0 },
		{ 800, 0x16, SJA1000_BTR_SJW_0 | 0 },
		{ 666, 0xB6, SJA1000_BTR_SJW_2 | 0 },
		{ 500, 0x1C, SJA1000_BTR_SJW_0 | 0 },
		{ 400, 0xFA, SJA1000_BTR_SJW_2 | 0 },
		{ 250, 0x1C, SJA1000_BTR_SJW_0 | 1 },
		{ 200, 0xFA, SJA1000_BTR_SJW_2 | 1 },
		{ 125, 0x1C, SJA1000_BTR_SJW_0 | 3 },
		{ 100, 0x2F, SJA1000_BTR_SJW_1 | 3 },
		{ 50, 0x2F, SJA1000_BTR_SJW_1 | 7 },
		{ 80, 0xFF, SJA1000_BTR_SJW_2 | 3 },
		{ 40, 0xFF, SJA1000_BTR_SJW_2 | 7 },
		{ 20, 0x2F, SJA1000_BTR_SJW_1 | 0x13 },
		{ 10, 0x2F, SJA1000_BTR_SJW_1 | 0x27 },
		{ 5, 0xFF, SJA1000_BTR_SJW_2 | 0x3F },
		{ 0, 0, 0 },
	};

	const struct btr *pbtr;

	pbtr = btrs;
	for (;;) {
		if (pbtr->baudrate == baudrate)
			break;
		if (pbtr->baudrate == 0)
			return;
		pbtr++;
	}

	ctlr->tseg1 = pbtr->btr1 & 0x0F;
	ctlr->tseg2 = (pbtr->btr1 >> 4) & 0x07;
	ctlr->samples = (pbtr->btr1 >> 7) & 0x01;
	ctlr->brp = pbtr->btr0 & 0x3F;
	ctlr->sjw = (pbtr->btr0 >> 6) & 0x03;
}

LOCAL STATUS
sja1000bittimingset(SJA1000_CTRL *ctlr)
{
	UINT8 value;

	if (ctlr->sjw > 0x03 || ctlr->brp > 0x3f || ctlr->tseg1 > 15 || ctlr->tseg1 < 2 || ctlr->tseg2 > 7 || ctlr->tseg2 < 1)
		return ERROR;

	// if controller is not in reset mode, enable change configuration
	value = csr8r(ctlr, SJA1000_MOD);
	if ((value & SJA1000_MOD_RM) == 0) {
		csr8w(ctlr, SJA1000_MOD, value | SJA1000_MOD_RM);
	}

	csr8w(ctlr, SJA1000_BTR0, (ctlr->sjw << 6) | ctlr->brp);
	if (ctlr->samples) {
		// three sample mode
		csr8w(ctlr, SJA1000_BTR1, 0x80 | (ctlr->tseg2 << 4) | ctlr->tseg1);
	} else {
		// one sample mode
		csr8w(ctlr, SJA1000_BTR1, (ctlr->tseg2 << 4) | ctlr->tseg1);
	}

	// restore original state of controller
	if ((value & SJA1000_MOD_RM) == 0) {
		csr8w(ctlr, SJA1000_MOD, value);

		// wait until the controller comes out of reset
		while (csr8r(ctlr, SJA1000_MOD) != value)
			;

		// wait for bus OK
		while (sja1000statusget(ctlr) != CAN_STATE_ERROR_ACTIVE)
			;
	}

	return OK;
}

LOCAL void
sja1000init(SJA1000_CTRL *ctlr)
{
	UINT8 value;

	// put the controller into reset mode
	csr8w(ctlr, SJA1000_MOD, SJA1000_MOD_RM);

	// set controller to PeliCAN mode
	value = csr8r(ctlr, SJA1000_CDR);
	value |= SJA1000_CDR_PELIMODE;
	csr8w(ctlr, SJA1000_CDR, value);

	// set (reset mode = 1, acceptance filter = 1)
	csr8w(ctlr, SJA1000_MOD, 0x09);

	// set command register to zero
	sja1000writecmdreg(ctlr, 0);

	// if baud rate not set, set default baud rate
	if (!ctlr->baudrateset)
		sja1000setbaudrate(ctlr, ctlr->baudrate);

	sja1000bittimingset(ctlr);

	// set the error warning limit
	csr8w(ctlr, SJA1000_EWLR, 0x60);

	// disable all interrupts
	csr8w(ctlr, SJA1000_IER, 0x0);

	// set output control
	csr8w(ctlr, SJA1000_OCR, 0xfb);

	// reset receive error counter
	csr8w(ctlr, SJA1000_RXERR, 0x0);

	// reset transmit error counter
	csr8w(ctlr, SJA1000_TXERR, 0x0);

	// set all acceptance code registers to 0xff
	// received messages must have these bits set
	csr8w(ctlr, SJA1000_ACR0, 0xff);
	csr8w(ctlr, SJA1000_ACR1, 0xff);
	csr8w(ctlr, SJA1000_ACR2, 0xff);
	csr8w(ctlr, SJA1000_ACR3, 0xff);

	// set the acceptance mask registers to don't care
	csr8w(ctlr, SJA1000_AMR0, 0xff);
	csr8w(ctlr, SJA1000_AMR1, 0xff);
	csr8w(ctlr, SJA1000_AMR2, 0xff);
	csr8w(ctlr, SJA1000_AMR3, 0xff);

	// enable interrupts
	csr8w(ctlr, SJA1000_IER, 0xff);

	// clear the controller reset
	value = csr8r(ctlr, SJA1000_MOD) & ~0x1;
	csr8w(ctlr, SJA1000_MOD, value);

	// wait until the controller comes out of reset
	while (csr8r(ctlr, SJA1000_MOD) != value)
		;

	// wait for bus OK
	while (sja1000statusget(ctlr) != CAN_STATE_ERROR_ACTIVE)
		;

	// initialize tx frame info, can only be done when controller is not in reset
	csr8w(ctlr, SJA1000_SFF, 0);

	if (ctlr->cmdsem) {
		semDelete(ctlr->cmdsem);
		ctlr->cmdsem = NULL;
	}
	ctlr->cmdsem = semBCreate(SEM_Q_FIFO, SEM_FULL);
}

LOCAL void
sja1000reinit(SJA1000_CTRL *ctlr)
{
	sja1000init(ctlr);
}

STATUS
sja1000create(SJA1000_CTRL *ctlr)
{
	ctlr->cookie = canDevConnect(&ctlr->candev, &sja1000netfuncs, ctlr->canname);
	if (ctlr->cookie == NULL)
		return ERROR;

	sja1000init(ctlr);

	if (!ctlr->isrqid)
		ctlr->isrqid = isrDeferQueueGet(ctlr->dev, 0, 0, 0);
	ctlr->isrdefrx.func = (VOIDFUNCPTR)sja1000task;
	ctlr->isrdefrx.pData = &ctlr->candev;

	return OK;
}

LOCAL STATUS
sja1000xmit(CAN_DEV *candev, CAN_MSG *msg)
{
	SJA1000_CTRL *ctlr;
	UINT8 value;
	UINT8 ndx;
	UINT32 i;
	int tries;

	ctlr = (SJA1000_CTRL *)candev;
	if (!msg)
		return ERROR;

	if (semTake(ctlr->cmdsem, SJA1000_CMD_TIMEOUT) != OK) {
		sja1000reinit(ctlr);
		return ERROR;
	}

	// check whether transmit buffer is empty
	tries = 10000;
	do {
		value = csr8r(ctlr, SJA1000_SR);
	} while (!(value & SJA1000_SR_TBS) && (tries-- > 0));

	if (tries <= 0)
		return ERROR;

	// check whether is extended message
	value = ((msg->can_id & CAN_EFF_FLAG) ? SJA1000_SFF_FF : 0);

	// check whether is a remote transmission request
	if (msg->can_id & CAN_RTR_FLAG)
		value |= SJA1000_SFF_RTR;

	value |= msg->can_dlc;
	csr8w(ctlr, SJA1000_SFF, value);

	// write the message ID
	ndx = SJA1000_TXID;
	if ((msg->can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG) {
		value = (msg->can_id >> (24 - 3)) & 0xff;
		csr8w(ctlr, ndx++, value);

		value = (msg->can_id >> (16 - 3)) & 0xff;
		csr8w(ctlr, ndx++, value);

		value = (msg->can_id >> (8 - 3)) & 0xff;
		csr8w(ctlr, ndx++, value);

		value = (msg->can_id << 3) & 0xff;
		csr8w(ctlr, ndx++, value);
	} else {
		value = (msg->can_id >> (8 - 5)) & 0xff;
		csr8w(ctlr, ndx++, value);

		value = (msg->can_id << 5) & 0xff;
		csr8w(ctlr, ndx++, value);
	}

	// write message data
	if (!(msg->can_id & CAN_RTR_FLAG)) {
		for (i = 0; i < msg->can_dlc; i++)
			csr8w(ctlr, ndx++, msg->data[i]);
	}

	// request a transmission
	sja1000writecmdreg(ctlr, SJA1000_CMR_TR);

	ctlr->txmsgcnt++;
	ctlr->txmsgbytecnt += msg->can_dlc;
	return OK;
}

LOCAL STATUS
sja1000ioctl(CAN_DEV *candev, int cmd, caddr_t data)
{
	return ERROR;

	(void)candev;
	(void)cmd;
	(void)data;
}

STATUS
sja1000shutdown(CAN_DEV *candev)
{
	SJA1000_CTRL *ctlr;

	ctlr = (SJA1000_CTRL *)candev;

	// put the controller into reset mode
	csr8w(ctlr, SJA1000_MOD, SJA1000_MOD_RM);

	return OK;
}

LOCAL STATUS
sja1000msgread(SJA1000_CTRL *ctlr, CAN_MSG *msg)
{
	UINT8 i;
	UINT8 value;
	UINT8 offset;

	value = csr8r(ctlr, SJA1000_SR);
	if (!(value & SJA1000_SR_RBS))
		return ERROR;

	value = csr8r(ctlr, SJA1000_SFF);
	msg->can_id = (value & (SJA1000_SFF_FF | SJA1000_SFF_RTR)) << 24;
	msg->can_dlc = value & 0xF;

	// test if message ID is extended or standard
	if (value & SJA1000_SFF_FF) {
		offset = 2;

		value = csr8r(ctlr, SJA1000_RXID);
		msg->can_id |= (value << (24 - 3));

		value = csr8r(ctlr, SJA1000_RXID + 1);
		msg->can_id |= (value << (16 - 3));

		value = csr8r(ctlr, SJA1000_RXID + 2);
		msg->can_id |= (value << (8 - 3));

		value = csr8r(ctlr, SJA1000_RXID + 3);
		msg->can_id |= (value >> 3);
	} else {
		offset = 0;

		value = csr8r(ctlr, SJA1000_RXID);
		msg->can_id |= (value << (8 - 5));

		value = csr8r(ctlr, SJA1000_RXID + 1);
		msg->can_id |= (value >> 5);
	}

	for (i = 0; i < msg->can_dlc; i++) {
		msg->data[i] = csr8r(ctlr, SJA1000_SFDATA + i + offset);
	}

	// release the receive buffer
	sja1000writecmdreg(ctlr, SJA1000_CMR_RRB);

	return OK;
}

LOCAL void
sja1000recv(CAN_DEV *candev)
{
	SJA1000_CTRL *ctlr;
	CAN_MSG msg;
	UINT8 value;

	ctlr = (SJA1000_CTRL *)candev;
	value = csr8r(ctlr, SJA1000_SR);
	if (!(value & SJA1000_SR_RBS))
		return;

	while (sja1000msgread(ctlr, &msg) == OK) {
		ctlr->rxmsgcnt++;
		ctlr->rxmsgbytecnt += msg.can_dlc;

		if (candev->pFuncTable->receiveRtn)
			candev->pFuncTable->receiveRtn(&msg, 0, candev, NULL);
	}
}

LOCAL void
sja1000errhandle(CAN_DEV *candev)
{
	CAN_MSG msg;
	UINT8 rxerror;
	UINT8 txerror;
	UINT8 ecc;
	UINT8 alc;
	UINT8 status;
	enum can_state state;
	enum can_state rxstate;
	enum can_state txstate;
	SJA1000_STATS *stats;
	SJA1000_CTRL *ctlr;

	ctlr = (SJA1000_CTRL *)candev;
	stats = &ctlr->stats;
	state = ctlr->state;

	status = csr8r(ctlr, SJA1000_SR);

	memset(&msg, 0, sizeof(CAN_MSG));
	msg.can_id = CAN_ERR_FLAG | CAN_ERR_BUSERROR;

	txerror = csr8r(ctlr, SJA1000_TXERR);
	rxerror = csr8r(ctlr, SJA1000_RXERR);
	msg.data[6] = txerror;
	msg.data[7] = rxerror;

	if (ctlr->irqstatus & SJA1000_IR_BEI) {
		stats->bus_errors++;

		ecc = csr8r(ctlr, SJA1000_ECC);

		msg.can_id |= CAN_ERR_PROT | CAN_ERR_BUSERROR;

		// set error type
		switch (ecc & ECC_MASK) {
		case ECC_BIT:
			msg.data[2] |= CAN_ERR_PROT_BIT;
			break;

		case ECC_FORM:
			msg.data[2] |= CAN_ERR_PROT_FORM;
			break;

		case ECC_STUFF:
			msg.data[2] |= CAN_ERR_PROT_STUFF;
			break;

		default:
			break;
		}

		// set error location
		msg.data[3] = ecc & ECC_SEG;

		// error occurred during transmission
		if (!(ecc & ECC_DIR)) {
			msg.data[2] |= CAN_ERR_PROT_TX;
			stats->tx_errors++;
		} else {
			stats->rx_errors++;
		}
	}

	if (ctlr->irqstatus & SJA1000_IR_ALI) {
		alc = csr8r(ctlr, SJA1000_ALC);
		stats->arb_lost_errors++;
		stats->tx_errors++;
		msg.can_id |= CAN_ERR_LOSTARB;
		msg.data[0] = alc & 0x1f;
	}

	if (ctlr->irqstatus & SJA1000_IR_EPI) {
		if (state == CAN_STATE_ERROR_PASSIVE) {
			state = CAN_STATE_ERROR_WARNING;
		} else {
			state = CAN_STATE_ERROR_PASSIVE;
		}
	}

	if (ctlr->irqstatus & SJA1000_IR_DOI) {
		msg.can_id |= CAN_ERR_CRTL;
		msg.data[1] = D_CAN_ERR_CRTL_RX_OVERFLOW;
		stats->rx_over_errors++;
		stats->rx_errors++;

		// clear bit
		sja1000writecmdreg(ctlr, CMD_CDO);
	}

	if (ctlr->irqstatus & SJA1000_IR_EI) {
		if (status & SR_BS) {
			state = CAN_STATE_BUS_OFF;
		} else if (status & SR_ES) {
			state = CAN_STATE_ERROR_WARNING;
		} else {
			state = CAN_STATE_ERROR_ACTIVE;
		}
	}

	if (state != ctlr->state) {
		txstate = (txerror >= rxerror) ? state : 0;
		rxstate = (txerror <= rxerror) ? state : 0;
		sja1000changestate(ctlr, &msg, txstate, rxstate);
		if (state == CAN_STATE_BUS_OFF)
			sja1000reinit(ctlr);
	}

	if (candev->pFuncTable && candev->pFuncTable->receiveRtn) {
		candev->pFuncTable->receiveRtn(&msg, 0, candev, NULL);
	}

	sja1000reinit(ctlr);
}

LOCAL void
sja1000task(CAN_DEV *candev)
{
	SJA1000_CTRL *ctlr;

	ctlr = (SJA1000_CTRL *)candev;
	while (ctlr->irqstatus) {
		if (ctlr->irqstatus & SJA1000_IR_ERR)
			sja1000errhandle(candev);

		if (ctlr->irqstatus & SJA1000_IR_RI)
			sja1000recv(candev);

		if (ctlr->irqstatus & SJA1000_IR_TI)
			semGive(ctlr->cmdsem);

		ctlr->irqstatus = csr8r(ctlr, SJA1000_IR);
	}

	// enable default interrupts
	csr8w(ctlr, SJA1000_IER, 0xff);
}

void
sja1000int(VXB_DEV_ID dev)
{
	SJA1000_CTRL *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	while (ctlr != NULL) {
		ctlr->irqstatus = csr8r(ctlr, SJA1000_IR);
		if (ctlr->irqstatus) {
			// disable all interrupts
			csr8w(ctlr, SJA1000_IER, 0x0);
			while (csr8r(ctlr, SJA1000_IER) != 0x0)
				;

			// add the job to queue
			isrDeferJobAdd(ctlr->isrqid, &ctlr->isrdefrx);
		}

		ctlr = ctlr->next;
	}
}

LOCAL void
sja1000writecmdreg(SJA1000_CTRL *ctlr, unsigned char val)
{
	// the command register needs some locking and time to settle
	SPIN_LOCK_ISR_TAKE(&ctlr->spinlock);
	csr8w(ctlr, SJA1000_CMR, val);

	// between two commands at least one internal clock cycle is needed
	// add SR read to increase delay
	csr8r(ctlr, SJA1000_SR);
	SPIN_LOCK_ISR_GIVE(&ctlr->spinlock);
}

LOCAL void
sj1000updatestats(SJA1000_CTRL *ctlr, enum can_state newstate)
{
	if (newstate <= ctlr->state)
		return;

	switch (newstate) {
	case CAN_STATE_ERROR_WARNING:
		ctlr->stats.error_warning++;
		break;
	case CAN_STATE_ERROR_PASSIVE:
		ctlr->stats.error_passive++;
		break;
	case CAN_STATE_BUS_OFF:
		ctlr->stats.bus_off++;
		break;
	default:
		break;
	}
}

LOCAL void
sja1000changestate(SJA1000_CTRL *ctlr, CAN_MSG *msg, enum can_state txstate, enum can_state rxstate)
{
	enum can_state newstate;

	newstate = max(txstate, rxstate);
	if (newstate == ctlr->state)
		return;

	sj1000updatestats(ctlr, newstate);
	ctlr->state = newstate;

	if (!msg)
		return;

	if (newstate == CAN_STATE_BUS_OFF) {
		msg->can_id |= CAN_ERR_BUSOFF;
		return;
	}

	msg->can_id |= CAN_ERR_CRTL;
	if (txstate >= rxstate)
		msg->data[1] |= txstate2frame(txstate);
	if (txstate <= rxstate)
		msg->data[1] |= rxstate2frame(rxstate);
}
