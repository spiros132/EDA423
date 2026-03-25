#ifndef _APP_H
#define _APP_H

#include "TinyTimber.h"
#include <stdbool.h>

#define BUFFERLENGTH 10
#define DELIMITER 'e'
#define DAC_REG (*(volatile uint8_t *)0x4000741C)

#define CONDUCTOR_MODE 1
#define MUSICIAN_MODE  2

typedef struct {
  Object super;
  
  //Part 0

  // Step 0
  char buffer[BUFFERLENGTH];
  int bufferLength;
  int history[3];
  int historyLength;

  // Step 1
  int songIndex;
  int song[32];
  int period[25];

  /* Mute bool. Mutes all SoundPlayers */
  int mute;

  /* Enables the deadlines of the SoundPlayers */
  int enable_deadlines;


  // Local definitions of the variables
  int playing;
  int key;
  int tempo;
  int volume;

  int mode;
} App;


#define initApp() \
{ \
  /* Super */ \
  initObject(), \
  \
  /* Buffer */ \
  {}, \
  /* Buffer Length*/ \
  0, \
  /* History */ \
  {0, 0, 0}, \
  /* History Length */ \
  0, \
  \
  /* Song Index */ \
  0, \
  /* Song */ \
  { \
    0, 2, 4,  0, 0, 2,  4, 0, \
    4, 5, 7,  4, 5, 7,  7, 9, \
    7, 5, 4,  0, 7, 9,  7, 5, \
    4, 0, 0, -5, 0, 0, -5, 0  \
  }, \
  { \
    2025, 1911, 1804, 1703, 1607, \
    1517, 1432, 1351, 1276, 1204, \
    1136, 1073, 1012,  956,  902, \
      851,  804,  758,  716,  676, \
      638,  602,  568,  536,  506  \
  }, \
  /* Mute bool */ \
  0, \
  /*Allows deadlines*/ \
  0, \
  0, \
  0, \
  120, \
  5, \
  CONDUCTOR_MODE \
}


void reader(App *, int);
void receiver(App *, int);
void startApp(App *, int);
void finish_input(App *, int);
void clear_input(App *, int);

#endif
