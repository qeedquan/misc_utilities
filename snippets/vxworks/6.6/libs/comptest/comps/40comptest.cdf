/*

Copy this to $(TGT_DIR)/config/comps/vxworks

In order for the library file to be linked into kernel image, need it to call functions from that library

*/

Component INCLUDE_COMP_TEST {
        NAME            Component Test
        SYNOPSIS        Component Test
        _CHILDREN       FOLDER_SHOW_ROUTINES
		PROTOTYPE       extern void abc(void); extern void xyz(void);
		INIT_RTN        abc(); xyz();
        MODULES         abc.o xyz.o
		_INIT_ORDER     usrShowInit
		INIT_BEFORE     INCLUDE_RTP_SHOW
}

