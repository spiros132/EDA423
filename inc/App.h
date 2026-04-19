#ifndef _APP_H
#define _APP_H

#include "TinyTimber.h"
#include "Definitions.h"
#include <stdbool.h>

#define BUFFERLENGTH 10



typedef struct {
  Object super;

  //Part 0

  // Step 0
  char buffer[BUFFERLENGTH];
  int bufferLength;
  int history[3];
  int historyLength;


  /// @brief The index of the current note playing in the song.
  int songIndex;

  /// @brief The notes of the song.
  int song[32];

  /// @brief The period of the notes in the song.
  int period[25];


  /// @brief Whether the board is currently muted or not.
  int mute;

  /// @brief Whether the board is currently playing a song or not.
  int playing;

  /// @brief The key of the song. Changes the pitch of the notes.
  int key;

  /// @brief The tempo of the song. Changes the speed of the notes.
  int tempo;

  /// @brief The volume of the song. Changes the loudness of the notes.
  int volume;


  /// @brief The mode of the board. Can be either conductor or musician mode.
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
  /* Playing bool */ \
  0, \
  /*Allows deadlines*/ \
  0, \
  /*Allows deadlines*/ \
  120, \
  /*Allows deadlines*/ \
  5, \
  /*Allows deadlines*/ \
  CONDUCTOR_MODE \
}


void reader(App *, int);
void receiver(App *, int);
void startApp(App *, int);
void finish_input(App *, int);
void clear_input(App *, int);

#endif
