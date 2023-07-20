#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"

#include "images/metroidBackground.h"
#include "images/samusRight.h"
#include "images/samusLeft.h"
#include "images/samusUp.h"
#include "images/samusUpRight.h"
#include "images/samusUpLeft.h"
#include "images/metroidGameSplashScreen.h"
#include "images/enemy.h"
#include "images/WinScreen.h"
#include "images/GameOver.h"
#include "images/PauseScreen.h"

// initialize five enemies
struct enemy e1 = {
  .row = HEIGHT/8,
  .col = WIDTH/8,
  .size = 28,
  .isAlive = 1,
  .isDrawn = 1,
};
struct enemy e2 = {
  .row = HEIGHT/8,
  .col = (WIDTH/8)*6,
  .size = 28,
  .isAlive = 1,
  .isDrawn = 1,
};
struct enemy e3 = {
  .row = HEIGHT/2,
  .col = WIDTH/8,
  .size = 28,
  .isAlive = 1,
  .isDrawn = 1,
};
struct enemy e4 = {
  .row = HEIGHT/2,
  .col = (WIDTH/8)*6,
  .size = 28,
  .isAlive = 1,
  .isDrawn = 1,
};
struct enemy e5 = {
  .row = HEIGHT/8,
  .col = (WIDTH/2) - 11,
  .size = 28,
  .isAlive = 1,
  .isDrawn = 1,
};

// initialize the array for the enemies
struct enemy earr[5];
// initialize a game_state for the current and previous states
struct game_state cs, ps;

// a function that detects whether a bolt will collide with an enemy on the next frame
int detectCollision(struct bolt b, struct enemy e) {
  int bleft = b.col + b.cd;
  int bright = bleft + b.size;
  int btop = b.row + b.rd;
  int bbottom = btop + b.size;
  int eleft = e.col;
  int eright = eleft + e.size;
  int etop = e.row;
  int ebottom = etop + e.size;
  if (bleft < eright && bright > eleft && btop < ebottom && bbottom > etop) {
      return 1;
  }
  return 0;
}

// a function that resets all relevant states and enemies back to an intended starting value
void resetStates(void) {
  cs.samus_state = RIGHT;
  cs.gba_state = START;
  cs.jss = 1;
  cs.time = 15;
  cs.enemiesKilled = 0;
  cs.laser_color = YELLOW;
  cs.bolt0.row = 0;
  cs.bolt0.col = 0;
  cs.bolt0.size = 0;
  earr[0] = e1;
  earr[1] = e2;
  earr[2] = e3;
  earr[3] = e4;
  earr[4] = e5;
  return;
}


int main(void) {
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial application state
  resetStates();

  // game loop
  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons

    ps = cs; // store current state to old state

    // if backspace is pressed, reset back to start
    if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons) && cs.gba_state != START) {
      resetStates();
    }

    // LOGIC PRE WAITFORVBLANK
    switch (cs.gba_state) {
      case START: ;
        // if START is pressed, change states and go to PLAY
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          cs.gba_state = PLAY;
          cs.samus_state = RIGHT;
          cs.jss = 1;
        }
        // logic to randomly update/animare samus_state's aim on the Start Screen
        int rand_result = randint(1, 6);
        switch (rand_result) {
          case 1: ;
            cs.samus_state = RIGHT;
            break;
          case 2: ;
            cs.samus_state = LEFT;
            break;
          case 3: ;
            cs.samus_state = UP;
            break;
          case 4: ;
            cs.samus_state = UPRIGHT;
            break;
          case 5: ;
            cs.samus_state = UPLEFT;
            break;
        }
        break;
      case PLAY: ;
        // update samus_state depending on arrow keys pressed
        if (KEY_DOWN(BUTTON_RIGHT, currentButtons) && !KEY_DOWN(BUTTON_UP, currentButtons)) {
          cs.samus_state = RIGHT;
        }
        else if (KEY_DOWN(BUTTON_LEFT, currentButtons) && !KEY_DOWN(BUTTON_UP, currentButtons)) {
          cs.samus_state = LEFT;
        }
        else if (KEY_DOWN(BUTTON_UP, currentButtons) && !KEY_DOWN(BUTTON_LEFT, currentButtons) && !KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
          cs.samus_state = UP;
        }
        else if (KEY_DOWN(BUTTON_RIGHT, currentButtons) && KEY_DOWN(BUTTON_UP, currentButtons)) {
          cs.samus_state = UPRIGHT;
        }
        else if (KEY_DOWN(BUTTON_LEFT, currentButtons) && KEY_DOWN(BUTTON_UP, currentButtons)) {
          cs.samus_state = UPLEFT;
        }
        // iterate through the five enemies and check for collisions between bolts and enemies
        for (int i = 0; i < 5; i++) {
          if (detectCollision(cs.bolt0, earr[i]) && earr[i].isAlive) {
            // if collision occurs on alive enemy, mark them is not alive, increment kills, despawn bolt
            earr[i].isAlive = 0;
            cs.enemiesKilled++;
            cs.bolt0.row = 0;
            cs.bolt0.col = 0;
            cs.bolt0.size = 0;
          }
        }
        // if START is pressed, go to the Pause Screen
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          cs.gba_state = PAUSE;
          cs.jss = 1;
        }
        // if more than 4 enemies are killed, go to Win Screen
        if (cs.enemiesKilled > 4) {
          cs.gba_state = WIN;
          cs.jss = 1;
        }
        // if time hits 0, go to Game Over Screen
        if (cs.time < 1) {
          cs.gba_state = LOSE;
          cs.jss = 1;
        }
        break;
      case PAUSE: ;
        // if START is pressed, resume play on the PLAY state
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          cs.gba_state = PLAY;
          cs.jss = 1;
        // update color of blaster bolt depending on current color and arrow button pressed
        } else if (cs.laser_color == YELLOW && KEY_JUST_PRESSED(BUTTON_DOWN, currentButtons, previousButtons)) {
          cs.laser_color = CYAN;
        } else if (cs.laser_color == CYAN && KEY_JUST_PRESSED(BUTTON_DOWN, currentButtons, previousButtons)) {
          cs.laser_color = MAGENTA;
        } else if (cs.laser_color == MAGENTA && KEY_JUST_PRESSED(BUTTON_UP, currentButtons, previousButtons)) {
          cs.laser_color = CYAN;
        } else if (cs.laser_color == CYAN && KEY_JUST_PRESSED(BUTTON_UP, currentButtons, previousButtons)) {
          cs.laser_color = YELLOW;
        }
        break;
      case WIN: ;
        break;
      case LOSE: ;
        break;
    }

    waitForVBlank();

    // DRAWING POST WAITFORVBLANK

    switch (cs.gba_state) {
      case START: ;
        // if JustSwitchedStates, draw background, first animated Samus frame, and text 
        if (cs.jss) {
          drawFullScreenImageDMA(metroidGameSplashScreen);
          drawImageDMA(40, 12, 23, 46, samusRight);
          char startText[51];
          sprintf(startText, "Press START...");
          drawString(100, 80, startText, WHITE);
          cs.jss = 0;
        }
        // every second, animate Samus by drawing a new pose depending on samus_state
        if (vBlankCounter % 60 == 0) {
          switch (cs.samus_state) {
            case RIGHT: ;
              drawImageDMA(40, 12, 23, 46, samusRight);
              break;
            case LEFT: ;
              drawImageDMA(40, 12, 23, 46, samusLeft);
              break;
            case UP: ;
              drawImageDMA(40, 12, 23, 46, samusUp);
              break;
            case UPRIGHT: ;
              drawImageDMA(40, 12, 23, 46, samusUpRight);
              break;
            case UPLEFT: ;
              drawImageDMA(40, 12, 23, 46, samusUpLeft);
              break;
          }
        }
        break;

      case PLAY: ;
        // if JustSwitchedStates, draw background, alive enemies, and the starting timer
        if (cs.jss) {
          drawFullScreenImageDMA(metroidBackground);
          for (int i = 0; i < 5; i++) {
            if (earr[i].isAlive) {
              drawImageTransparentDMA(earr[i].row, earr[i].col, earr[i].size, earr[i].size, enemy, metroidBackground);
            }
          }
          char buffer[51];
          sprintf(buffer, "Time: %d", cs.time);
          undrawImageDMA(150, 5, 200, 10, metroidBackground);
          drawString(150, 5, buffer, WHITE);
          cs.jss = 0;
        }
        // depending on samus_state, draw Samus aiming the correct way
        switch (cs.samus_state) {
          case RIGHT: ;
            drawImageDMA(82, 108, 23, 46, samusRight);
            break;
          case LEFT: ;
            drawImageDMA(82, 108, 23, 46, samusLeft);
            break;
          case UP: ;
            drawImageDMA(82, 108, 23, 46, samusUp);
            break;
          case UPRIGHT: ;
            drawImageDMA(82, 108, 23, 46, samusUpRight);
            break;
          case UPLEFT: ;
            drawImageDMA(82, 108, 23, 46, samusUpLeft);
            break;
        }
        // every second, decrement the time and update the timer
        if (vBlankCounter % 60 == 0) {
          cs.time--;
          char buffer[51];
          sprintf(buffer, "Time: %d", cs.time);
          undrawImageDMA(150, 5, 200, 10, metroidBackground);
          drawString(150, 5, buffer, WHITE);
        }

        // if B (X) button pressed, initialize a bolt depending on samus_state
        if (KEY_JUST_PRESSED(BUTTON_B, currentButtons, previousButtons)) {
          struct bolt b = {
            .size = 4,
            .color = cs.laser_color,
          };
          switch (cs.samus_state) {
            case RIGHT: ;
              b.row = 99;
              b.col = 126;
              b.rd = 0;
              b.cd = 10;
              break;
            case LEFT: ;
              b.row = 99;
              b.col = 107;
              b.rd = 0;
              b.cd = -10;
              break;
            case UP: ;
              b.row = 85;
              b.col = 120;
              b.rd = -10;
              b.cd = 0;
              break;
            case UPRIGHT: ;
              b.row = 87;
              b.col = 126;
              b.rd = -7;
              b.cd = 7;
              break;
            case UPLEFT: ;
              b.row = 87;
              b.col = 107;
              b.rd = -7;
              b.cd = -7;
              break;
          }
          // store initialized bolt in current state
          cs.bolt0 = b;
        }
        // update the position of the bolt based on its delta row and delta col
        cs.bolt0.row += cs.bolt0.rd;
        cs.bolt0.col += cs.bolt0.cd;
        // despawn if bolt hits edge of screen
        if((cs.bolt0.row < 0) || (cs.bolt0.row > HEIGHT - cs.bolt0.size) || (cs.bolt0.col < 0) || (cs.bolt0.col > WIDTH - cs.bolt0.size)) {
          cs.bolt0.row = 0;
          cs.bolt0.col = 0;
          cs.bolt0.size = 0;
        }
        // Remove the old bolt
        undrawImageDMA(ps.bolt0.row, ps.bolt0.col, ps.bolt0.size, ps.bolt0.size, metroidBackground);

        // Draw the new bolt
        drawRectDMA(cs.bolt0.row, cs.bolt0.col, cs.bolt0.size, cs.bolt0.size, cs.bolt0.color);

        // if enemy is not alive and drawn, undraw enemy
        for (int i = 0; i < 5; i++) {
          if (!(earr[i].isAlive) && earr[i].isDrawn) {
            undrawImageDMA(earr[i].row, earr[i].col, earr[i].size, earr[i].size, metroidBackground);
            earr[i].isDrawn = 0;
          }
        }

        break;
      case PAUSE: ;
        // if JustSwitchedStates, draw pause screen, and time left
        if (cs.jss) {
          drawFullScreenImageDMA(PauseScreen);
          char buffer[51];
          sprintf(buffer, "%d sec left", cs.time);
          drawString(20, 157, buffer, WHITE);
          cs.jss = 0;
        }
        // draw an arrow indicating the current laser color depending on laser color
        switch (cs.laser_color) {
          case YELLOW: ;
            undrawImageDMA(107, 10, 15, 24, PauseScreen);
            drawString(95, 10, "->", YELLOW);
            break;
          case CYAN: ;
            undrawImageDMA(95, 10, 15, 12, PauseScreen);
            undrawImageDMA(119, 10, 15, 12, PauseScreen);
            drawString(107, 10, "->", CYAN);
            break;
          case MAGENTA: ;
            undrawImageDMA(95, 10, 15, 24, PauseScreen);
            drawString(119, 10, "->", MAGENTA);
            break;
        }
        break;
      case WIN: ;
        // if JustSwitchedStates, draw win screen
        if (cs.jss) {
          drawFullScreenImageDMA(WinScreen);
          cs.jss = 0;
        }
        break;
      case LOSE: ;
        // if JustSwitchedStates, draw game over screen
        if (cs.jss) {
          drawFullScreenImageDMA(GameOver);
          cs.jss = 0;
        }
        break;
    }
    previousButtons = currentButtons; // Store the current state of the buttons
  }
  return 0;
}