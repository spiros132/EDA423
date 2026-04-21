#ifndef _CONDUCTOR_H
#define _CONDUCTOR_H

#include "canTinyTimber.h"

/// @brief This function should only be called if we are in conductor mode.
/// @param msg The CAN message received.
void conductor_receive_can_message(CANMsg* msg);


/// @brief Send a CAN message to the musicians with the note for the next one to play.
/// @param note The index of the note to play.
void conductor_send_note_to_play(int note);

/// @brief Send a CAN message to the musicians to reset their music player.
void conductor_send_reset();

/// @brief Send a CAN message to the musicians to play their music player.
void conductor_send_play();

/// @brief Send a CAN message to the musicians to stop their music player.
void conductor_send_stop();

/// @brief Send a CAN message to the musicians to set the tempo.
/// @param tempo The new tempo to set.
void conductor_send_tempo(int tempo);

/// @brief Send a CAN message to the musicians to set the key.
/// @param key The new key to set.
void conductor_send_key(int key);

#endif
