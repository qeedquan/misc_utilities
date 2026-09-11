// cc -o xresource xresource.c -lX11
#include <stdio.h>
#include <err.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

int
main(void)
{
	static const char *keys[] = {
	    "Xft.dpi",
	    "Xft.antialias",
	    "Xcursor.theme",
	    "*customization",
	    "xscreensaver.dateFormat",
	    "xscreensaver.*",
	};

	char *resmgr, *type;
	XrmDatabase db;
	XrmValue ret;
	Display *dpy;
	size_t i;

	if (XSupportsLocale())
		printf("locale: '%s'\n", XSetLocaleModifiers(""));

	dpy = XOpenDisplay(NULL);
	if (!dpy)
		errx(1, "failed to open display");

	// initialize xresources
	XrmInitialize();
	resmgr = XResourceManagerString(dpy);
	printf("%s\n\n\n", resmgr);

	// get the xresource database
	db = XrmGetStringDatabase(resmgr);
	if (db == NULL)
		errx(1, "failed to open database");

	// query xresource keys
	for (i = 0; i < nelem(keys); i++) {
		XrmGetResource(db, keys[i], "String", &type, &ret);
		printf("[%s] (%s) = %s\n", keys[i], type, ret.addr);
	}

	XrmDestroyDatabase(db);
	XCloseDisplay(dpy);

	return 0;
}
