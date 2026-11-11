#include <signal.h>
#include <ncurses.h>

volatile sig_atomic_t doresize;

void
resize(int sig)
{
	doresize = 1;
	(void)sig;
}

int
main(void)
{
	int ch;

	initscr();
	raw();

	// when a resize event happens, the terminal will send a signal that we can handle
	signal(SIGWINCH, resize);
	for (;;) {
		ch = getch();
		if (ch == 27)
			break;

		// on resize, need to reinit everything
		if (doresize) {
			endwin();
			initscr();
			clear();
			refresh();
		}
		clear();
		mvprintw(LINES / 2, COLS / 2, "key %x %dx%d\n", ch, LINES, COLS);
		refresh();
	}
	endwin();
	return 0;
}
