#pragma once

#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>

#include "textwin.h"

#define nullptr ((void *)0)

char *command = nullptr;
char *arguments = nullptr;
char *directory = nullptr;
char **output_lines = nullptr;
int output_count = 0;
int selected_line = 0;

/**
 * Initialize buffers for future use
 */
void init_api() {
  command = (char *)calloc(512, sizeof(char));
  arguments = (char *)calloc(512, sizeof(char));
  directory = (char *)calloc(512, sizeof(char));
  output_lines = (char **)calloc(0, sizeof(char *));
  sprintf(directory, "./");
}

/**
 * Runs command: find --help
 * Output printed in commands window
 */
void print_help() {
  FILE *fp = popen("find --help", "r");
  char line[1024];
  while (fgets(line, sizeof(line), fp) != NULL) {
    print_text_r("%s", line);
  }
  pclose(fp);
}

/**
 * Update buffer data with pre defined prompts
 * 
 * @param name name that will show in pre defined prompts
 * @param buffer pointer to buffer that will be modificated
 */
void buffer_input(const char *name, char **buffer) {
  print_text_r("Current %s: %s\n", name, *buffer);
  print_text_colored(PAIR_RED, "Keep existing(y,n)?");
  const int key = getch();
  if (key == 'n' || key == 'N') {
    free(*buffer);
    *buffer = (char *)calloc(512, sizeof(char));
    print_text_r("\nNew %s: ", name);
    get_text(*buffer, 512);
    print_text_colored(PAIR_GREEN, "Your %s: %s\n", name, *buffer);
  }
}

/**
 * Print output after running find with user defined data in list window
 */
void draw_output() {
  wclear(getlistscr());
  int start_pos = (selected_line / getmax()) * getmax();
  for (int i = start_pos; i < output_count; i++) {
    if (i == selected_line) {
      wprintw(getlistscr(), "> %s", output_lines[i]);
    } else
      wprintw(getlistscr(),"%s", output_lines[i]);
  }
  wrefresh(getlistscr());
}

/**
 * Running find with user defined data and show output using draw_output method
 */
void refresh_output() {
  selected_line = 0;
  for (int i = 0; i < output_count; i++) {
    free(output_lines[i]);
  }
  output_count = 0;
  free(output_lines);
  output_lines = (char **)calloc(0, sizeof(char *));
  FILE *fp = popen(command, "r");
  char line[256];
  print_text_colored(PAIR_GREEN, "Refreshing list");
  while (fgets(line, sizeof(line), fp) != NULL) {
    output_count++;
    output_lines =
        (char **)realloc(output_lines, output_count * sizeof(char *));
    output_lines[output_count - 1] = (char *)malloc(sizeof(line));
    strcpy(output_lines[output_count - 1], line);
  }
  pclose(fp);
  draw_output();
}

/**
 * Prepare find command based on pattern
 * 
 * @param pattern search pattern
 */
void search(char *pattern) {
  free(command);
  command = (char *)calloc(512, sizeof(char));
  sprintf(command, "find %s %s -name '*%s*'", directory, arguments, pattern);
  refresh_output();
}

/**
 * Ask user to write search pattern
 */
void input_bar() {
  print_text_r("Pattern: ");
  char input[80];
  get_text(input, 80);

  search(input);
}

/**
 * Run wl-copy to copy data to clipboardd
 * 
 * @param text text that will be copied to clipboard
 */
void copy_to_clipboard(const char *text) {
  char *wlcopy = (char *)calloc(512, sizeof(char));
  sprintf(wlcopy, "echo '%s' | wl-copy", text);
  system(wlcopy);
  free(wlcopy);
}

/**
 * Copy path if exist and after that call function copy_to_clipboard
 * 
 * @param relative copy relative path instead of absolute
 */
void copy_path(bool relative) {
  if(selected_line >= output_count || selected_line < 0)
    return;
  if (relative) {
    copy_to_clipboard(output_lines[selected_line]);
  } else {
    char full_path[PATH_MAX];
    realpath(output_lines[selected_line] + 2, full_path);
    copy_to_clipboard(full_path);
  }
  print_text_colored(PAIR_RED, "Copied to clipboard\n");
}

/**
 * Main method that used for keyboard interaction
 * 
 * @return is user pressed ESC to close program
 */
bool main_loop() {
  const int key = getch();
  wclear(getscr());
  if (key == 27) {
    return true;
  } else if (key == KEY_BACKSPACE) {
    input_bar();
  } else if (key == 'h') {
    print_help();
  } else if (key == 'r') {
    refresh_output();
  } else if (key == 'a') {
    buffer_input("arguments", &arguments);
  } else if (key == 'd') {
    buffer_input("directory", &directory);
  } else if (key == KEY_DOWN) {
    if (selected_line < output_count - 1)
      selected_line++;
    draw_output();
  } else if (key == KEY_UP) {
    if (selected_line > 0)
      selected_line--;
    draw_output();
  } else if (key == 'x') {
    copy_path(false);
    draw_output();
  } else if (key == 'c') {
    copy_path(true);
    draw_output();
  }
  return false;
}