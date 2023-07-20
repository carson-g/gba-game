#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

enum gba_state {
  START,
  PLAY,
  PAUSE,
  WIN,
  LOSE,
};

enum samus_state {
  RIGHT,
  LEFT,
  UP,
  UPRIGHT,
  UPLEFT,
};

struct bolt {
  int row;
  int col;
  int rd;
  int cd;
  int size;
  unsigned short color;
};

struct enemy {
  int row;
  int col;
  int size;
  int isAlive;
  int isDrawn;
};

struct game_state {
  struct bolt bolt0;
  enum gba_state gba_state;
  enum samus_state samus_state;
  u16 laser_color;
  int jss;
  int time;
  int enemiesKilled;
};

int detectCollision(struct bolt b, struct enemy e);
void resetStates(void);
int main(void);

#endif
