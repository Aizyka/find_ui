#pragma once
#include <ncurses.h>
#include <stdarg.h>

#define PAIR_CYAN 1
#define PAIR_GREEN 2
#define PAIR_RED 3

WINDOW *scr;
WINDOW *list_scr;
int max_per_page;

WINDOW *getscr() { return scr; }
WINDOW *getlistscr() { return list_scr; }
int getmax() { return max_per_page; }

void init_win() {
  initscr();
  noecho();
  keypad(stdscr, 1);
  curs_set(0);
  start_color();
  init_pair(1, COLOR_CYAN, 0);
  init_pair(2, COLOR_GREEN, 0);
  init_pair(3, COLOR_RED, 0);
  init_pair(4, COLOR_BLACK, COLOR_WHITE);

  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  const char *info_text =
      " Exit: ESC | Reload: R | New Search: BACKSPACE | Print Help: "
      "H | Arguments: A | Directory: D | Copy: X | Copy Relative: C";
  WINDOW *info_window = subwin(stdscr, 1, cols, 0, 0);
  wbkgd(info_window, COLOR_PAIR(4));
  mvwprintw(info_window, 0, cols / 2 - strlen(info_text) / 2 - 1, info_text);
  wrefresh(info_window);

  WINDOW *border_window = subwin(stdscr, rows - 1, (cols - 1) / 2 - 1, 1, 0);

  wbkgd(border_window, 1);
  wborder(border_window, 0, 0, 0, 0, 0, 0, 0, 0);
  mvwprintw(border_window, 0, 2, "Commands");
  wrefresh(border_window);

  scr = subwin(border_window, rows - 3, (cols - 1) / 2 - 4, 2, 2);

  border_window = subwin(stdscr, rows - 1, cols / 2 + 1, 1, cols / 2 - 1);

  wbkgd(border_window, 1);
  wborder(border_window, 0, 0, 0, 0, 0, 0, 0, 0);
  mvwprintw(border_window, 0, 2, "List");
  wrefresh(border_window);

  list_scr =
      subwin(border_window, rows - 3, (cols - 1) / 2 - 3, 2, cols / 2 - 1 + 2);
  max_per_page = rows - 3;
}

void stop() { endwin(); }

void print_text_r(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vwprintw(getscr(), fmt, args);
  wrefresh(getscr());
  va_end(args);
}

void print_text_nr(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vwprintw(getscr(), fmt, args);
  va_end(args);
}

void refresh_win() { wrefresh(getscr()); }

void print_text_colored(int color, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  wattron(getscr(), COLOR_PAIR(color));
  vwprintw(getscr(), fmt, args);
  wattroff(getscr(), COLOR_PAIR(color));
  wrefresh(getscr());
  va_end(args);
}

void get_text(char *buffer, int max) {
  int current_pos = 0;
  while (true) {
    if (current_pos == max)
      break;

    char current_char = getch();
    if (current_char == '\n')
      break;
    else if (current_char == '\a') {
      if (current_pos > 0) {
        buffer[current_pos] = '\0';
        current_pos--;
        wprintw(getscr(), "\b \b");
        wrefresh(getscr());
      }
      continue;
    }
    wprintw(getscr(), "%c", current_char);
    buffer[current_pos] = current_char;
    current_pos++;
    wrefresh(getscr());
  }
  buffer[current_pos] = '\0';
  wprintw(getscr(), "\n");
}