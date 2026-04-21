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

  /// @brief The mode of the board. Can be either conductor or musician mode.
  int mode;
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
