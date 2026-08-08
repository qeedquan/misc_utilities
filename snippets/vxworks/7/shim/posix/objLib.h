#ifndef _OBJLIB_H_
#define _OBJLIB_H_

#define OM_CREATE 0x10000000
#define OM_EXCL 0x20000000
#define OM_DELETE_ON_LAST_CLOSE 0x40000000

#define S_objLib_OBJ_ID_ERROR (M_objLib | 1)
#define S_objLib_OBJ_UNAVAILABLE (M_objLib | 2)
#define S_objLib_OBJ_DELETED (M_objLib | 3)
#define S_objLib_OBJ_TIMEOUT (M_objLib | 4)
#define S_objLib_OBJ_NO_METHOD (M_objLib | 5)
#define S_objLib_OBJ_NO_ACCESS_RIGHTS (M_objLib | 6)
#define S_objLib_OBJ_NAME_TRUNCATED (M_objLib | 7)
#define S_objLib_OBJ_NAME_CLASH (M_objLib | 8)
#define S_objLib_OBJ_NOT_NAMED (M_objLib | 9)
#define S_objLib_OBJ_ILLEGAL_CLASS_TYPE (M_objLib | 10)
#define S_objLib_OBJ_OPERATION_UNSUPPORTED (M_objLib | 11)
#define S_objLib_OBJ_INVALID_OWNER (M_objLib | 12)
#define S_objLib_OBJ_RENAME_NOT_ALLOWED (M_objLib | 13)
#define S_objLib_OBJ_DESTROY_ERROR (M_objLib | 14)
#define S_objLib_OBJ_HANDLE_TBL_FULL (M_objLib | 15)
#define S_objLib_OBJ_NOT_FOUND (M_objLib | 16)
#define S_objLib_OBJ_INVALID_ARGUMENT (M_objLib | 17)
#define S_objLib_OBJ_LIB_NOT_INITIALIZED (M_objLib | 18)
#define S_objLib_ACCESS_DENIED (M_objLib | 19)

#endif
