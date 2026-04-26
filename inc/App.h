#ifndef _APP_H
#define _APP_H

#include "TinyTimber.h"
#include "Definitions.h"
#include <stdbool.h>


typedef struct {
  Object super;

  char buffer[10];
  int bufferLength;
  int history[3];
  int historyLength;

  Msg debug_msg;

  Msg heartbeat_msg;
} App;


#define initApp() \
{ \
  initObject(), \
  /* Buffer */ \
  {}, \
  /* Buffer Length*/ \
  0, \
  /* History */ \
  {0, 0, 0}, \
  /* History Length */ \
  0, \
  /* Debug Msg */ \
  NULL, \
  /* Heartbeat Msg */ \
  NULL \
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

/// @brief Prints out the debug information about the current tempo.
/// @param App The app object.
/// @param int Unused parameter.
void print_debug_info(App *, int);


/// @brief Resets the board. Should be used when we have failed somehow.
/// @param App The app object.
/// @param int Unused parameter.
void reset_board(App *, int);

#endif
