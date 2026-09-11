# VxWorks resources are encapsulated into a class group.
# This will dump all the information for each class.
# The classId types are:
#  1  windSemClass,       /* Wind native semaphore */
#  2  windSemPxClass,     /* POSIX semaphore */
#  3  windMsgQClass,      /* Wind native message queue */
#  4  windMqPxClass,      /* POSIX message queue */
#  5  windRtpClass,       /* real time process */
#  6  windTaskClass,      /* task */
#  7  windWdClass,        /* watchdog */
#  8  windFdClass,        /* file descriptor */
#  9  windPgPoolClass,    /* page pool */
#  10 windPgMgrClass,     /* page manager */
#  11 windGrpClass,       /* group */
#  12 windVmContextClass, /* virtual memory context */
#  13 windTrgClass,       /* trigger */
#  14 windMemPartClass,   /* memory partition */
#  15 windI2oClass,       /* I2O */
#  16 windDmsClass,       /* device management system */
#  17 windSetClass,       /* Set */
#  18 windIsrClass,       /* ISR object */
#  19 windTimerClass,     /* Timer services */
#  20 windSdClass,        /* Shared data region */
#  21 windPxTraceClass,   /* POSIX trace */

classShow(objClassIdGet(5), 1)
classShow(objClassIdGet(6), 1)
