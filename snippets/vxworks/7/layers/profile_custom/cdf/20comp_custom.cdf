Component       INCLUDE_CUSTOM_LAYER_INIT {
	NAME        Custom Layer Init Routine
	SYNOPSIS    Custom Layer Initialization
	PROTOTYPE   IMPORT void xcinit ();
	INIT_RTN    xcinit ();
	MODULES     xcinit.o
	REQUIRES    INCLUDE_BOOT_APP
}

InitGroup       usrCustomLayerStart {
	INIT_RTN    usrCustomLayerStart ();
	SYNOPSIS    Starts the custom layer after initialization is complete.
	INIT_ORDER  INCLUDE_CUSTOM_LAYER_INIT
	INIT_BEFORE usrBootAppStart
	INIT_AFTER  usrToolsInit
	_INIT_ORDER usrRoot
}
