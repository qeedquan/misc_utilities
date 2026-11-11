// http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/init.html

#include <ncurses.h>

void
draw(void)
{
	int row, col, len;
	char msg[80];

	getmaxyx(stdscr, row, col);

	attron(A_BOLD);
	mvprintw(0, 0, "Is this thing on?");
	attroff(A_BOLD);

	attron(A_ITALIC);
	len = snprintf(msg, sizeof(msg), "Hello, World!\n");
	mvprintw(row / 2, (col - len) / 2, "%s", msg);
	mvprintw(row - 2, 0, "%dx%d\n", row, col);
	mvprintw(row - 1, 0, "Press any key to continue...");

	refresh();
	getch();
}

int
main(void)
{
	initscr();
	draw();
	endwin();
	return 0;
}
