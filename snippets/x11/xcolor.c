// cc -o xcolor xcolor.c -I/usr/X11R6/include -I/usr/include/freetype2 -lX11 -lXft
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>
#include <err.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xft/Xft.h>
#include <X11/XKBlib.h>

int defaultbg = 0;
int borderpx = 2;
int cols = 80;
int rows = 24;

char *opt_title = "xcolor window";

const char *colorname[] = {
    // normal colors
    "black",
    "red3",
    "green3",
    "yellow3",
    "blue2",
    "magenta3",
    "cyan3",
    "gray90",

    // bright colors
    "gray50",
    "red",
    "green",
    "yellow",
    "#5c5cff",
    "magenta",
    "cyan",
    "white",

    [255] = 0,

    // more colors can be added after 255 to use with DefaultXX
    "#cccccc",
    "#555555",
};

void kpress(XEvent *ev);

void (*handler[LASTEvent])(XEvent *) = {
    [KeyPress] = kpress,
};

typedef struct {
	Display *dpy;
	Visual *vis;
	Colormap cmap;
	Window win;
	GC gc;
	Drawable buf;
	XftDraw *draw;
	XftColor *col;
	size_t ncol;
	Atom netwmname;
	XSetWindowAttributes attrs;
	int scr;
	int l, t;
	int w, h;
	int cw, ch;
} XW;

XW xw;

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define nelem(x) (sizeof(x) / sizeof(x[0]))

const char *
evstr(int type)
{
	switch (type) {
	case KeyPress:
		return "keypress";
	case ClientMessage:
		return "client message";
	case ConfigureNotify:
		return "configure notify";
	case UnmapNotify:
		return "unmap notify";
	case Expose:
		return "expose";
	case FocusIn:
		return "focus in";
	case FocusOut:
		return "focus out";
	case MotionNotify:
		return "motion notify";
	case ButtonPress:
		return "button press";
	case ButtonRelease:
		return "button release";
	}
	return "unknown";
}

void *
xcalloc(size_t nmemb, size_t size)
{
	void *p;
	p = calloc(nmemb, size);
	if (!p)
		abort();
	return p;
}

uint16_t
sixd_to_16bit(int x)
{
	return x == 0 ? 0 : 0x3737 + 0x2828 * x;
}

int
xloadcol(size_t i, const char *name, XftColor *ncolor)
{
	XRenderColor color = {.alpha = 0xffff};

	if (!name) {
		if (16 <= i && i <= 255) {
			if (i < 6 * 6 * 6 + 16) { /* same colors as xterm */
				color.red = sixd_to_16bit(((i - 16) / 36) % 6);
				color.green = sixd_to_16bit(((i - 16) / 6) % 6);
				color.blue = sixd_to_16bit(((i - 16) / 1) % 6);
			} else { /* greyscale */
				color.red = 0x0808 + 0x0a0a * (i - (6 * 6 * 6 + 16));
				color.green = color.blue = color.red;
			}
			return XftColorAllocValue(xw.dpy, xw.vis, xw.cmap, &color, ncolor);
		} else
			name = colorname[i];
	}
	return XftColorAllocName(xw.dpy, xw.vis, xw.cmap, name, ncolor);
}

void
xloadcols(void)
{
	size_t i;

	xw.ncol = max(nelem(colorname), 256);
	xw.col = xcalloc(xw.ncol, sizeof(*xw.col));
	for (i = 0; i < xw.ncol; i++) {
		if (!xloadcol(i, NULL, &xw.col[i])) {
			if (colorname[i])
				errx(1, "could not allocate color '%s'", colorname[i]);
			else
				errx(1, "could not allocate color %zu", i);
		}
		printf("{%d, %d, %d, 255},\n", xw.col[i].color.red >> 8, xw.col[i].color.green >> 8, xw.col[i].color.blue >> 8);
	}
	printf("\n");
}

void
xsetenv(void)
{
	char buf[sizeof(long) * 8 + 1];

	snprintf(buf, sizeof(buf), "%lu", xw.win);
	setenv("WINDOWID", buf, 1);
	printf("WINDOWID %s\n", buf);
	printf("LASTEvent %d\n", LASTEvent);
}

void
xsettitle(void)
{
	XTextProperty prop;

	Xutf8TextListToTextProperty(xw.dpy, &opt_title, 1, XUTF8StringStyle, &prop);
	XSetWMName(xw.dpy, xw.win, &prop);
	XSetTextProperty(xw.dpy, xw.win, &prop, xw.netwmname);
	XFree(prop.value);
}

void
xinit(void)
{
	XGCValues gcvalues;
	Window parent;

	xw.dpy = XOpenDisplay(NULL);
	if (!xw.dpy)
		errx(1, "can't open display");

	xw.scr = XDefaultScreen(xw.dpy);
	xw.vis = XDefaultVisual(xw.dpy, xw.scr);
	xw.cmap = XDefaultColormap(xw.dpy, xw.scr);
	xloadcols();

	xw.cw = 12;
	xw.ch = 12;
	xw.w = 2 * borderpx + cols * xw.cw;
	xw.h = 2 * borderpx + rows * xw.ch;

	xw.attrs.background_pixel = xw.col[defaultbg].pixel;
	xw.attrs.border_pixel = xw.col[defaultbg].pixel;
	xw.attrs.bit_gravity = NorthWestGravity;
	xw.attrs.event_mask = FocusChangeMask | KeyPressMask | KeyReleaseMask |
	                      ExposureMask | VisibilityChangeMask | StructureNotifyMask |
	                      ButtonMotionMask | ButtonPressMask | ButtonReleaseMask;

	parent = XRootWindow(xw.dpy, xw.scr);
	xw.win = XCreateWindow(xw.dpy, parent, xw.l, xw.t, xw.w, xw.h,
	                       0, XDefaultDepth(xw.dpy, xw.scr), InputOutput,
	                       xw.vis, CWBackPixel | CWBorderPixel | CWBitGravity | CWEventMask | CWColormap, &xw.attrs);

	memset(&gcvalues, 0, sizeof(gcvalues));
	xw.gc = XCreateGC(xw.dpy, parent, GCGraphicsExposures, &gcvalues);
	printf("graphics exposure: %d\n", gcvalues.graphics_exposures);

	xw.buf = XCreatePixmap(xw.dpy, xw.win, xw.w, xw.h, DefaultDepth(xw.dpy, xw.scr));
	xw.draw = XftDrawCreate(xw.dpy, xw.buf, xw.vis, xw.cmap);
	XMapWindow(xw.dpy, xw.win);
	XSync(xw.dpy, False);

	xw.netwmname = XInternAtom(xw.dpy, "_NET_WM_NAME", False);
	xsettitle();
}

void
kpress(XEvent *ev)
{
	XKeyEvent *kev = &ev->xkey;

	printf("kpress %d %d %d %d\n", kev->type, kev->x, kev->y, kev->keycode);
	switch (kev->keycode) {
	case 113: // left
		if (defaultbg > 0)
			defaultbg--;
		break;
	case 114: // right
		if (defaultbg < xw.ncol)
			defaultbg++;
		break;
	case 9: // escape
		exit(0);
		break;
	}
	printf("bg %d %#lx\n", defaultbg, xw.col[defaultbg].pixel);
}

void
run(void)
{
	XEvent ev;

	// wait for window mapping
	do {
		XNextEvent(xw.dpy, &ev);
		// This XFilterEvent call is required for XOpenIM.
		// It filters out the key event and some client
		// message for the input method too
		if (XFilterEvent(&ev, None))
			continue;
	} while (ev.type != MapNotify);

	for (;;) {
		while (XPending(xw.dpy)) {
			XNextEvent(xw.dpy, &ev);
			if (XFilterEvent(&ev, None))
				continue;
			if (handler[ev.type])
				handler[ev.type](&ev);
			else
				printf("%s event\n", evstr(ev.type));
		}
		XSetForeground(xw.dpy, xw.gc, xw.col[defaultbg].pixel);
		XFillRectangle(xw.dpy, xw.buf, xw.gc, 0, 0, 1024, 1024);
		XCopyArea(xw.dpy, xw.buf, xw.win, xw.gc, 0, 0, 1024, 1024, 0, 0);
		XFlush(xw.dpy);
	}
}

int
main(void)
{
	setlocale(LC_CTYPE, "");
	XSetLocaleModifiers("");
	xinit();
	xsetenv();
	run();
	return 0;
}
