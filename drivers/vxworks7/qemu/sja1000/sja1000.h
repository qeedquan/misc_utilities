#ifndef _SJA_1000_H_
#define _SJA_1000_H_

#ifdef __cplusplus
extern "C" {
#endif

enum {
	SJA1000_BASIC_MODE = 1,
	SJA1000_PELI_MODE = 2,
};

// SJA1000 basic mode register offsets
enum {
	SJA1000_CR = 0,
};

// SJA1000 pelican mode register offsets
enum {
	SJA1000_MOD = 0,
	SJA1000_CMR = 1,
	SJA1000_SR = 2,
	SJA1000_IR = 3,
	SJA1000_IER = 4,
	SJA1000_BTR0 = 6,
	SJA1000_BTR1 = 7,
	SJA1000_OCR = 8,
	SJA1000_ALC = 11,
	SJA1000_ECC = 12,
	SJA1000_EWLR = 13,
	SJA1000_RXERR = 14,
	SJA1000_TXERR = 15,
	SJA1000_SFF = 16,
	SJA1000_EFF = 16,
	SJA1000_ACR0 = 16,
	SJA1000_RXID = 17,
	SJA1000_TXID = 17,
	SJA1000_ACR1 = 17,
	SJA1000_ACR2 = 18,
	SJA1000_ACR3 = 19,
	SJA1000_SFDATA = 19,
	SJA1000_AMR0 = 20,
	SJA1000_AMR1 = 21,
	SJA1000_AMR2 = 22,
	SJA1000_AMR3 = 23,
	SJA1000_RMC = 29,
	SJA1000_RXBSA = 30,
	SJA1000_CDR = 31,
};

// Mode register bits
enum {
	SJA1000_MOD_RM = 0x01,
	SJA1000_MOD_LOM = 0x02,
	SJA1000_MOD_STM = 0x04,
	SJA1000_MOD_AFM = 0x08,
	SJA1000_MOD_SM = 0x10,
};

// Command register bits
enum {
	SJA1000_CMR_TR = 0x01,
	SJA1000_CMR_AT = 0x02,
	SJA1000_CMR_RRB = 0x04,
	SJA1000_CMR_CDO = 0x08,
	SJA1000_CMR_SRR = 0x10,
};

// Status register bits
enum {
	SJA1000_SR_RBS = 0x01,
	SJA1000_SR_DOS = 0x02,
	SJA1000_SR_TBS = 0x04,
	SJA1000_SR_TCS = 0x08,
	SJA1000_SR_RS = 0x10,
	SJA1000_SR_TS = 0x20,
	SJA1000_SR_ES = 0x40,
	SJA1000_SR_BS = 0x80,
};

// Interrupt register bits
enum {
	SJA1000_IR_RI = 0x01,
	SJA1000_IR_TI = 0x02,
	SJA1000_IR_EI = 0x04,
	SJA1000_IR_DOI = 0x08,
	SJA1000_IR_WUI = 0x10,
	SJA1000_IR_EPI = 0x20,
	SJA1000_IR_ALI = 0x40,
	SJA1000_IR_BEI = 0x80,

	SJA1000_IR_ERR = SJA1000_IR_BEI |
	                 SJA1000_IR_ALI |
	                 SJA1000_IR_EPI |
	                 SJA1000_IR_DOI |
	                 SJA1000_IR_EI,
};

// Interrupt enable register bits
enum {
	SJA1000_IER_RIE = 0x01,
	SJA1000_IER_TIE = 0x02,
	SJA1000_IER_EIE = 0x04,
	SJA1000_IER_DOIE = 0x08,
	SJA1000_IER_WUIE = 0x10,
	SJA1000_IER_EPIE = 0x20,
	SJA1000_IER_ALIE = 0x40,
	SJA1000_IER_BEIE = 0x80,
};

// SFF or EFF register bits
enum {
	SJA1000_SFF_FF = 0x80,
	SJA1000_SFF_RTR = 0x40,
};

// Error code capture register
enum {
	SJA1000_ECC_SEG0 = 0x01,
	SJA1000_ECC_SEG1 = 0x02,
	SJA1000_ECC_SEG2 = 0x04,
	SJA1000_ECC_SEG3 = 0x08,
	SJA1000_ECC_SEG4 = 0x10,
	SJA1000_ECC_DIR = 0x20,
	SJA1000_ECC_ERRC0 = 0x40,
	SJA1000_ECC_ERCC1 = 0x80,
};

// Clock Divider Register (CDR)
enum {
	SJA1000_CDR_PELIMODE = 0x80,
	SJA1000_CDR_BASICMODE = 0x00,
};

// SJA1000 BASIC mode register offsets
enum {
	SJA1000_BASIC_MODE_TXIDENTIFER1 = 0x0A,
	SJA1000_BASIC_MODE_TXIDENTIFER2 = 0x0B,
	SJA1000_BASIC_MODE_TXDATA1 = 0x0C,
	SJA1000_BASIC_MODE_TXDATA2 = 0x0D,
	SJA1000_BASIC_MODE_TXDATA3 = 0x0E,
	SJA1000_BASIC_MODE_TXDATA4 = 0x0F,
	SJA1000_BASIC_MODE_TXDATA5 = 0x10,
	SJA1000_BASIC_MODE_TXDATA6 = 0x11,
	SJA1000_BASIC_MODE_TXDATA7 = 0x12,
	SJA1000_BASIC_MODE_TXDATA8 = 0x13,
	SJA1000_BASIC_MODE_RXIDENTIFER1 = 0x14,
	SJA1000_BASIC_MODE_RXIDENTIFER2 = 0x15,
	SJA1000_BASIC_MODE_RXDATA1 = 0x16,
	SJA1000_BASIC_MODE_RXDATA2 = 0x17,
	SJA1000_BASIC_MODE_RXDATA3 = 0x18,
	SJA1000_BASIC_MODE_RXDATA4 = 0x19,
	SJA1000_BASIC_MODE_RXDATA5 = 0x1A,
	SJA1000_BASIC_MODE_RXDATA6 = 0x1B,
	SJA1000_BASIC_MODE_RXDATA7 = 0x1C,
	SJA1000_BASIC_MODE_RXDATA8 = 0x1D,
};

// SJA1000 BASIC mode
enum {
	SJA1000_BASIC_MODE_RTR = 0x10,
};

// error status of CAN-controller / data[1]
enum {
	D_CAN_ERR_CRTL_UNSPEC = 0x00,      // unspecified
	D_CAN_ERR_CRTL_RX_OVERFLOW = 0x01, // RX buffer overflow
	D_CAN_ERR_CRTL_TX_OVERFLOW = 0x02, // TX buffer overflow
	D_CAN_ERR_CRTL_RX_WARNING = 0x04,  // reached warning level for RX errors
	D_CAN_ERR_CRTL_TX_WARNING = 0x08,  // reached warning level for TX errors
	D_CAN_ERR_CRTL_RX_PASSIVE = 0x10,  // reached error passive status RX
	D_CAN_ERR_CRTL_TX_PASSIVE = 0x20,  // reached error passive status TX
};

// ECC register
enum {
	ECC_SEG = 0x1F,
	ECC_DIR = 0x20,
	ECC_ERR = 0x06,
	ECC_BIT = 0x00,
	ECC_FORM = 0x40,
	ECC_STUFF = 0x80,
	ECC_MASK = 0xc0,

	// status register content
	SR_BS = 0x80,
	SR_ES = 0x40,
	SR_TS = 0x20,
	SR_RS = 0x10,
	SR_TCS = 0x08,
	SR_TBS = 0x04,
	SR_DOS = 0x02,
	SR_RBS = 0x01,

	// commands
	CMD_SRR = 0x10,
	CMD_CDO = 0x08,
	CMD_RRB = 0x04,
	CMD_AT = 0x02,
	CMD_TR = 0x01,
};

enum {
	SJA1000_BTR_SJW_0 = 0,
	SJA1000_BTR_SJW_1 = 1 << 6,
	SJA1000_BTR_SJW_2 = 2 << 6,
	SJA1000_BTR_SJW_3 = 3 << 6,

	SJA1000_BTR1_SAM_0 = 0 << 7,
	SJA1000_BTR1_SAM_1 = 1 << 7,
};

#define SJA1000_CMD_TIMEOUT (sysClkRateGet() * 2)

typedef struct SJA1000_CTRL SJA1000_CTRL;
typedef UINT8 (*SJA1000_READ_FUNC)(SJA1000_CTRL *, UINT8);
typedef void (*SJA1000_WRITE_FUNC)(SJA1000_CTRL *, UINT8, UINT32);

typedef struct {
	unsigned int rx_errors;
	unsigned int tx_errors;
	unsigned int rx_over_errors;
	unsigned int bus_errors;
	unsigned int error_warning;
	unsigned int error_passive;
	unsigned int bus_off;
	unsigned int arb_lost_errors;
	unsigned int restarts;
} SJA1000_STATS;

struct SJA1000_CTRL {
	// must be first field
	CAN_DEV candev;
	char canname[32];

	// device id
	VXB_DEV_ID dev;

	// register read/write
	SJA1000_READ_FUNC read;
	SJA1000_WRITE_FUNC write;

	// controller set baudrate
	BOOL baudrateset;

	// device registers
	void *reghandle;
	VIRT_ADDR regbase;
	UINT32 regdelta;

	// pci specific device registers
	void *pcireghandle;
	VIRT_ADDR pciregbase;

	// can cookie
	void *cookie;
	// job queue for process packets on rx
	ISR_DEFER_QUEUE_ID isrqid;
	ISR_DEFER_JOB isrdefrx;

	// reference clock frequency
	UINT32 xtalfreq;
	// baud rate
	UINT32 baudrate;
	// sample number
	BOOL samples;
	// baud rate prescaler
	UINT8 brp;
	// sync jump width
	UINT8 sjw;
	// time segment
	UINT8 tseg1;
	UINT8 tseg2;

	volatile UINT32 irqstatus;

	VXB_RESOURCE *adrres;
	VXB_RESOURCE *pcires;
	VXB_RESOURCE *intres;

	// pci device ID
	UINT16 pcidid;
	UINT16 pcivid;

	SJA1000_CTRL *next;

	// basic or peli mode
	UINT32 mode;

	// for writes only, read is serialized inside the read task function
	SEM_ID cmdsem;

	// tx/rx counters
	UINT32 txmsgcnt;
	UINT32 txmsgbytecnt;
	UINT32 rxmsgcnt;
	UINT32 rxmsgbytecnt;

	// can device state and stats
	CAN_DEV_STAT state;
	SJA1000_STATS stats;
	spinlockIsr_t spinlock;
};

STATUS sja1000create(SJA1000_CTRL *);
STATUS sja1000shutdown(CAN_DEV *);
void sja1000int(VXB_DEV_ID);

#ifdef __cplusplus
}
#endif

#endif
