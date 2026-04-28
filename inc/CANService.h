#ifndef _CAN_SERVICE_H
#define _CAN_SERVICE_H

#include "TinyTimber.h"
#include "canTinyTimber.h"


typedef struct {
  Object super;

  /// @brief A boolean to indicate whether we want the board to stop responding and getting messages.
  int force_silent;

  /// @brief A boolean to indicate whether the board has been failing to connect to the CAN bus.
  int bus_can_missing;

  /// @brief A boolean to see if we were the conductor before we failed.
  int conductor_last;

} CANService;


#define initCANService() \
{ \
  initObject(), \
  /* force_silent */ \
  false, \
  /* bus_can_missing */ \
  false, \
  /* conductor_last */ \
  false \
}

/// @brief Sets the force silent mode of the object. If the force silent mode is enabled, the board will ignore any send or receive CAN requests.
/// @param CANService The CANService object.
/// @param int The new value to set the force silent mode to.
void can_set_force_silent(CANService *, int);

/// @brief Gets the force silent mode of the object.
/// @param CANService The CANService object.
/// @param int Unused parameter.
/// @return The boolean value of the force silent mode.
int can_get_force_silent(CANService *, int);

/// @brief Gets if the CAN bus is missing.
/// @param CANService The CANService object.
/// @param int Unused parameter.
/// @return The boolean value of if the CAN bus is missing.
int can_get_bus_can_missing(CANService *, int);

/// @brief A function to send the heartbeat message to the other boards.
/// @param CANService The CANService object.
/// @param int Unused parameter.
void can_send_heartbeat(CANService *, int);


/// @brief Sends a conductor claim message to the other boards.
/// @param CANService The CANService object.
/// @param int Unused parameter.
void can_send_conductor_claim(CANService *, int);

/// @brief Sends the note to play, the index of the melody, to the other boards.
/// @param CANService The CANService object.
/// @param int The index of the note to play.
void can_send_note_to_play(CANService *, int);

/// @brief Sends a reset message to the other boards.
/// @param CANService The CANService object.
/// @param int Unused parameter.
void can_send_reset(CANService *, int);

/// @brief Sends a play message to the other boards.
/// @param CANService The CANService object.
/// @param int Unused parameter.
void can_send_play(CANService *, int);

/// @brief Sends a stop message to the other boards.
/// @param CANService The CANService object.
/// @param int Unused parameter.
void can_send_stop(CANService *, int);

/// @brief Sends a set tempo message to the other boards.
/// @param CANService The CANService object.
/// @param int The new tempo to set. This parameter should have the size of a short.
void can_send_set_tempo(CANService *, int);

/// @brief Sends a set key message to the other boards.
/// @param CANService The CANService object.
/// @param int The new key to set. This parameter should have the size of a char.
void can_send_set_key(CANService *, int);


/// @brief A function to receive a CAN message from the CAN bus and handle it accordingly.
/// @param CANService The CANService object.
/// @param int Unused parameter.
void can_receive_message(CANService *, int);


#endif
