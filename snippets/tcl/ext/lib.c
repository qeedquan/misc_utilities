#include <tcl.h>
#include <stdio.h>

static int
foo(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	printf("%s %d\n", __func__, objc);
	return TCL_OK;
}

static int
bar(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	printf("%s %d\n", __func__, objc);
	return TCL_OK;
}

static int
baz(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	printf("%s %d\n", __func__, objc);
	return TCL_OK;
}

int
Test_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL)
		return TCL_ERROR;

	printf("%s\n", __func__);
	Tcl_CreateObjCommand(interp, "foo", foo, NULL, NULL);
	Tcl_CreateObjCommand(interp, "bar", bar, NULL, NULL);
	Tcl_CreateObjCommand(interp, "baz", baz, NULL, NULL);

	return TCL_OK;
}
