// http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/windows.html
#include <ncurses.h>

typedef struct {
	chtype ls, rs, ts, bs, tl, tr, bl, br;
} Box;

typedef struct {
	int x, y, w, h;
	Box box;
} Win;

void mkwin(Win *);
void mkbox(Win *, bool);

int
main(void)
{
	Win win;
	int ch;

	initscr();
	start_color();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);

	mkwin(&win);

	attron(COLOR_PAIR(1));
	printw("Press ESC to exit");
	refresh();
	attroff(COLOR_PAIR(1));

	mkbox(&win, TRUE);
	while ((ch = getch()) != 27) {
		switch (ch) {
		case KEY_LEFT:
			mkbox(&win, FALSE);
			--win.x;
			mkbox(&win, TRUE);
			break;
		case KEY_RIGHT:
			mkbox(&win, FALSE);
			++win.x;
			mkbox(&win, TRUE);
			break;
		case KEY_UP:
			mkbox(&win, FALSE);
			--win.y;
			mkbox(&win, TRUE);
			break;
		case KEY_DOWN:
			mkbox(&win, FALSE);
			++win.y;
			mkbox(&win, TRUE);
			break;
		}
	}
	endwin();
	return 0;
}

void
mkwin(Win *wd)
{
	wd->h = 3;
	wd->w = 10;
	wd->y = (LINES - wd->h) / 2;
	wd->x = (COLS - wd->w) / 2;

	wd->box.ls = '|';
	wd->box.rs = '|';
	wd->box.ts = '-';
	wd->box.bs = '-';
	wd->box.tl = '+';
	wd->box.tr = '+';
	wd->box.bl = '+';
	wd->box.br = '+';
}

void
mkbox(Win *wd, bool borders)
{
	int x, y, w, h;
	int i, j;

	x = wd->x;
	y = wd->y;
	w = wd->w;
	h = wd->h;

	if (borders) {
		mvaddch(y, x, wd->box.tl);
		mvaddch(y, x + w, wd->box.tr);
		mvaddch(y + h, x, wd->box.bl);
		mvaddch(y + h, x + w, wd->box.br);
		mvhline(y, x + 1, wd->box.ts, w - 1);
		mvhline(y + h, x + 1, wd->box.bs, w - 1);
		mvvline(y + 1, x, wd->box.ls, h - 1);
		mvvline(y + 1, x + w, wd->box.rs, h - 1);
	} else {
		for (j = y; j <= y + h; ++j) {
			for (i = x; i <= x + w; ++i)
				mvaddch(j, i, ' ');
		}
	}

	refresh();
}
