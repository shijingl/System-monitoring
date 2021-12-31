#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  int n = 30;
  NCursesDisplay::Display(system, n);
}