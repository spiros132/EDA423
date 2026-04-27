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

  /// @brief The heartbeat message that is sent to the other boards.
  Msg heartbeat_message;

} CANService;


#define initCANService() \
{ \
  initObject(), \
  /* force_silent */ \
  false, \
  /* bus_can_missing */ \
  false, \
  /* heartbeat_message */ \
  NULL \
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


/// @brief A function to send the heartbeat message to the other boards.
/// @param CANService The CANService object.
/// @param int Unused parameter.
void can_send_heartbeat(CANService *, int);

/// @brief Stops the heartbeat message loop.
/// @param CANService The CANService object.
/// @param int Unused parameter.
void stop_heartbeat_message(CANService *, int);


/// @brief A function to send the conductor state (tempo and key) to the other boards.
/// @param tempo The tempo to pack.
/// @param key The key to pack.
/// @return The packed arguments as an int.
static inline int can_send_conductor_state_pack(short tempo, signed char key) {
  return (int)((tempo << 8) | key);
}
/// @brief A function to unpack the tempo from the arguments of the `can_send_conductor_state` function.
/// @param args The arguments packed as an int.
/// @return The unpacked tempo as a short.
static inline short can_send_conductor_state_unpack_tempo(int args) {
  return (short)((args >> 8) & 0xFFFF);
}
/// @brief A function to unpack the key from the arguments of the `can_send_conductor_state` function.
/// @param args The arguments packed as an int.
/// @return The unpacked key as a char.
static inline signed char can_send_conductor_state_unpack_key(int args) {
  return (signed char)(args & 0xFF);
}

/// @brief Sends the state of the conductor to the other boards.
/// @param CANService The CANService object.
/// @param int The arguments packed as an int. Use the `can_send_conductor_state_pack` function to pack the arguments into an integer.
void can_send_conductor_state(CANService *, int);


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
