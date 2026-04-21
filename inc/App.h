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
  /* Mute bool */ \
  0, \
  /* Playing bool */ \
  0, \
  /* Current key */ \
  0, \
  /* Current tempo */ \
  120, \
  /* Current volume */ \
  5, \
  /* Operation Mode */ \
  CONDUCTOR_MODE \
}


void reader(App *, int);
void receiver(App *, int);
void startApp(App *, int);

/// @brief Finishes the input and updates the history list.
/// @param App The app object.
/// @param int Unused parameter.
void finish_input(App *, int);

/// @brief Clears the input buffer and resets the buffer length.
/// @param App The app object.
/// @param int Unused parameter.
void clear_input(App *, int);


/// @brief The loop that will send heartbeats to the other boards.
/// @param App The app object.
/// @param int Unused parameter.
void heartbeat_tick(App *, int);

#endif
