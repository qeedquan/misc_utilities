/*

Old VxWorks implement MTD drivers using this method, where you have to add this file into the source tree and add it to the mtdTable[]

VxWorks 7 way to do this is to implement a flash driver using VXBUS and let the vxbFlashStub MTD driver add your flash driver for use
VxWorks 7 also added a way to register a MTD function using flIdentifyRegister(), this allows us to register function without modifying the core source code

The structure hierarchy is as follows:
Flash {
        Socket
}

Sockets are stored in a global array and can be referred to without a flash device context

*/

#include <vxWorks.h>
#include <stdio.h>
#include <tffs/flflash.h>
#include <tffs/backgrnd.h>

LOCAL FLStatus
dtffsread(FLFlash vol, CardAddress address, void FAR1 *buffer, unsigned int length, int modes)
{
	kprintf("%s(vol=%p, address=%x, buffer=%p, length=%u, modes=%d)\n", __func__, vol, address, buffer, length, modes);
	return flOK;
}

LOCAL FLStatus
dtffswrite(FLFlash vol, CardAddress address, const void FAR1 *buffer, unsigned int length, int modes)
{
	kprintf("%s(vol=%p, address=%x, buffer=%p, length=%u, modes=%d)\n", __func__, vol, address, buffer, length, modes);
	return flOK;
}

LOCAL FLStatus
dtffserase(FLFlash vol, unsigned int first_erasable_block, unsigned int num_erasable_blocks)
{
	kprintf("%s(vol=%p, first_erasable_block=%u, num_erasable_blocks=%u)\n", __func__, vol, first_erasable_block, num_erasable_blocks);
	return flOK;
}

LOCAL void FAR0 *
dtffsmap(FLFlash vol, CardAddress address, unsigned int length)
{
	kprintf("%s(vol=%p, address=%x, length=%u)\n", __func__, vol, address, length);
	return NULL;
}

LOCAL void
dtffspoweron(FLFlash vol)
{
	kprintf("%s(vol=%p)\n", __func__, vol);
}

FLStatus
dtffsidentify(FLFlash vol)
{
	vol.type = 0x1337;
	vol.erasableBlockSize = 512;
	vol.chipSize = 0x400000;
	vol.noOfChips = 1;
	vol.interleaving = 0;
	vol.flags = 0;
	vol.mtdVars = NULL;

	// if these are not overwritten, they get set to a default IO function
	vol.read = dtffsread;
	vol.write = dtffswrite;
	vol.erase = dtffserase;
	vol.map = dtffsmap;
	vol.setPowerOnCallback = dtffspoweron;

	return flOK;
}

LOCAL FLBoolean
carddetected(FLSocket vol)
{
	kprintf("%s(vol=%p)\n", __func__, vol);
	return TRUE;
}

LOCAL void
vccon(FLSocket vol)
{
	kprintf("%s(vol=%p)\n", __func__, vol);
}

LOCAL void
vccoff(FLSocket vol)
{
	kprintf("%s(vol=%p)\n", __func__, vol);
}

LOCAL FLStatus
vppon(FLSocket vol)
{
	kprintf("%s(vol=%p)\n", __func__, vol);
	return flOK;
}

LOCAL void
vppoff(FLSocket vol)
{
	kprintf("%s(vol=%p)\n", __func__, vol);
}

LOCAL FLStatus
initsocket(FLSocket vol)
{
	kprintf("%s(vol=%p)\n", __func__, vol);
	return flOK;
}

LOCAL void
setwindow(FLSocket vol)
{
	kprintf("%s(vol=%p)\n", __func__, vol);
}

LOCAL void
setmappingcontext(FLSocket vol, unsigned page)
{
	kprintf("%s(vol=%p, page=%u)\n", __func__, vol, page);
}

LOCAL FLBoolean
cardchangeindicator(FLSocket vol)
{
	kprintf("%s(vol=%p)\n", __func__, vol);
	return FALSE;
}

LOCAL FLBoolean
writeprotected(FLSocket vol)
{
	kprintf("%s(vol=%p)\n", __func__, vol);
	return FALSE;
}

void
dtffssocketregister()
{
	FLSocket *sock;

	sock = flSocketOf(noOfDrives);
	sock->cardDetected = carddetected;
	sock->VccOn = vccon;
	sock->VccOff = vccoff;
	sock->VppOn = vppon;
	sock->VppOff = vppoff;
	sock->initSocket = initsocket;
	sock->setWindow = setwindow;
	sock->setMappingContext = setmappingcontext;
	sock->getAndClearCardChangeIndicator = cardchangeindicator;
	sock->writeProtected = writeprotected;
}