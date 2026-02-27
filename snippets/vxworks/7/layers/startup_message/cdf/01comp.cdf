Component INCLUDE_STARTUP_MESSAGE {
        NAME            Startup Message
        SYNOPSIS        Prints a start up message
        CFG_PARAMS      STARTUP_VERSION \
                        STARTUP_VALUE1 \
                        STARTUP_VALUE2
        PROTOTYPE       IMPORT void usrStartupMessage (void);
        INIT_RTN        usrStartupMessage ();
        INIT_BEFORE     INCLUDE_RTL_APPL
        _INIT_ORDER     usrRoot
        CONFIGLETTES    usrStartupMessage.c
        HELP            Startup Message
}

/*

These parameters become macro defines that are visible in configlettes.
They also appear inside the project parameter header in the VIP.

*/

Parameter STARTUP_VERSION {
    NAME            Startup version.
    SYNOPSIS        Startup version.
    TYPE            char *
    DEFAULT         "ALEPH NULL"
}

Parameter STARTUP_VALUE1 {
    NAME            Startup value 1.
    SYNOPSIS        A value.
    TYPE            int
    DEFAULT         15
}

Parameter STARTUP_VALUE2 {
    NAME            Startup value 2.
    SYNOPSIS        A value.
    TYPE            int
    DEFAULT         353
}
