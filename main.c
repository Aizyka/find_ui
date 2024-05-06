#include <locale.h>

#include "api.h"
#include "textwin.h"

int main() {
  setlocale(LC_ALL, "");

  init_api();
  init_win();

  while (true) {
    if (main_loop())
      break;
  }

  stop();
}