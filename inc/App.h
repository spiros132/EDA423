#ifndef _APP_H
#define _APP_H

#include "TinyTimber.h"
#include "Definitions.h"
#include <stdbool.h>
#include "canTinyTimber.h"

typedef struct {
  Object super;

  char buffer[10];
  int bufferLength;
  int history[3];
  int historyLength;

  Msg debug_msg;

  Timer timer;
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
  initTimer() \
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


/// @brief Prints out the debug information about the current tempo.
/// @param App The app object.
/// @param int Unused parameter.
void print_debug_info(App *, int);


/// @brief Resets the board. Should be used when we have failed somehow.
/// @param App The app object.
/// @param int Unused parameter.
void reset_board(App *, int);

/// @brief Checks active boards to see if we are the only one in the network.
/// @param App The app object.
/// @param int Unused parameter.
void check_active_boards(App *, int);


int get_time(App *, int);

#endif
