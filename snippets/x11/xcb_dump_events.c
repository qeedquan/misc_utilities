// basic xcb usage
// make a window and handle events
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#include <xcb/xcb.h>

void
die(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(1);
}

int
main(void)
{
	static const char *title = "Screen Grabber";
	xcb_connection_t *conn;
	xcb_screen_t *screen;
	xcb_window_t window;
	xcb_generic_event_t *ev;
	uint32_t mask, values[2];

	conn = xcb_connect(NULL, NULL);
	if (!conn)
		die("failed to connect to x");

	// use first screen
	screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
	            XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
	            XCB_EVENT_MASK_ENTER_WINDOW |
	            XCB_EVENT_MASK_LEAVE_WINDOW |
	            XCB_EVENT_MASK_STRUCTURE_NOTIFY |
	            XCB_EVENT_MASK_PROPERTY_CHANGE |
	            XCB_EVENT_MASK_BUTTON_PRESS |
	            XCB_EVENT_MASK_BUTTON_RELEASE |
	            XCB_EVENT_MASK_POINTER_MOTION |
	            XCB_EVENT_MASK_FOCUS_CHANGE |
	            XCB_EVENT_MASK_KEY_PRESS;

	window = xcb_generate_id(conn);
	xcb_create_window(conn,
	                  0,                             // depth
	                  window,                        // window     
	                  screen->root,                  // parent window
	                  0, 0,                          // x, y
	                  150, 150,                      // width, height
	                  10,                            // border width
	                  XCB_WINDOW_CLASS_INPUT_OUTPUT, // class
	                  screen->root_visual,           // visual
	                  mask, values                   // masks
	);

	xcb_change_property(conn,
	                    XCB_PROP_MODE_REPLACE,
	                    window,
	                    XCB_ATOM_WM_NAME,
	                    XCB_ATOM_STRING,
	                    8,
	                    strlen(title),
	                    title);

	xcb_map_window(conn, window);
	xcb_flush(conn);

	for (;;) {
		ev = xcb_wait_for_event(conn);
		if (ev == NULL)
			break;

		switch (ev->response_type & ~0x80) {
		case XCB_BUTTON_PRESS:
			goto out;

		default:
			printf("%x\n", ev->response_type & ~0x80);
			break;
		}
	}

out:
	xcb_disconnect(conn);

	return 0;
}
