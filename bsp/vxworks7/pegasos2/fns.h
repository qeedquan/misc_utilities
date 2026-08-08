#ifndef _FNS_H_
#define _FNS_H_

STATUS i8254create(I8254_CTLR *, int, int, UINT32, UINT32, UINT32, UINT32);
void i8254int(VXB_DEV_ID);

STATUS ds1385create(DS1385_CTLR *, int, UINT32, UINT32, UINT32);
void ds1385int(VXB_DEV_ID);

STATUS lptcreate(LPT_CTLR *);
void lptint(VXB_DEV_ID);

STATUS m48t59create(M48T59_CTLR *);

UINT32 mvread32(UINT32);
void mvwrite32(UINT32, UINT32);

void utilinit(void);

STATUS pgs2pciaddio(VXB_DEV_ID, VXB_RESOURCE *, VXB_RESOURCE_ADR *);

UINT32 sysTimeBaseUGet(void);
UINT32 sysTimeBaseLGet(void);

void sysTimeBaseUPut(ULONG);
void sysTimeBaseLPut(ULONG);

void pgs2pcilink(void);
void vt8231link(void);
void viaidelink(void);
void i8254link(void);
void ds1385link(void);
void i6300wdlink(void);
void viai2clink(void);
void nvramlink(void);
void lptlink(void);
void m48t59link(void);
void utillink(void);

#endif
