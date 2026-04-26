#ifndef _APP_H
#define _APP_H

#include "TinyTimber.h"
#include "Definitions.h"
#include <stdbool.h>

#define BUFFERLENGTH 10



typedef struct {
  Object super;

  char buffer[BUFFERLENGTH];
  int bufferLength;
  int history[3];
  int historyLength;

  /// @brief The mode of the board. Can be either conductor or musician mode.
  int mode;

  int silent_failure;

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
  /* Operation Mode */ \
  MUSICIAN_MODE, \
  /* Silent Failure */ \
  false, \
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


/// @brief Getter for the mode of the board.
/// @param App The app object.
/// @param int The new mode to set for the board.
void set_mode(App *, int);

/// @brief Getter for the mode of the board.
/// @param App The app object.
/// @param int Unused parameter.
/// @return The mode of the board.
int get_mode(App *, int);

/// @brief Starts the failure mode, the board will not be able to send or receive any CAN messages.
/// @param App The app object.
/// @param int Unused parameter.
void start_failure_mode(App *, int);

/// @brief Stops the failure mode.
/// @param App The app object.
/// @param int Unused parameter.
void stop_failure_mode(App *, int);

/// @brief Gets if we are in failure mode or not.
/// @param App The app object.
/// @param int Unused parameter.
/// @return 1 if we are in failure mode, 0 otherwise.
int get_failure_mode(App *, int);

/// @brief Resets the board. Should be used when we have failed somehow.
/// @param App The app object.
/// @param int Unused parameter.
void reset_board(App *, int);

#endif
