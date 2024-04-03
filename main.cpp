#include "console/console.h"
#include "game.h"
#include <cstdlib>
#include <ctime>

int main(void) {
  srand(time(NULL));
  console::init();
  game();
}
