/*

MODULES specifies a list of files to compile as a component.

LINK_SYMS specifies a list of VXB_DRV symbols found in the source.
If the names in the source don't match the LINK_SYMS list, the VIP won't be able to include it

*/

Component DRV_CUSTOM {
	NAME            Custom Method driver
	SYNOPSIS        This component provides custom method driver.
	MODULES         driver.o
	LINK_SYMS       customdrv
}

