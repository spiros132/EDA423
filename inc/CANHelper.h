#ifndef _CAN_HELPER_H
#define _CAN_HELPER_H

#include "canTinyTimber.h"
#include "Definitions.h"


/// @brief Get the priority of the CAN message.
/// @param msgID The message ID of the CAN message.
/// @return The priority of the CAN message, or -1 if the priority is unknown.
int get_priority(unsigned char msgID);

/// @brief Get the type of the CAN message.
/// @param buffer The buffer of the CAN message.
/// @return The type of the CAN message, or -1 if the type is unknown.
int get_type(unsigned char buffer[8]);

/// @brief Get a short value from the buffer at the given index.
/// @param buffer The buffer of the CAN message.
/// @param index The index to get the short value from. Must be between 0 and 6 inclusive.
/// @return The short value at the given index, or -1 if the index is out of bounds.
short get_buffer_short_at(unsigned char buffer[8], int index);

/// @brief Get an int value from the buffer at the given index.
/// @param buffer The buffer of the CAN message.
/// @param index The index to get the int value from. Must be between 0 and 4 inclusive.
/// @return The int value at the given index, or -1 if the index is out of bounds.
int get_buffer_int_at(unsigned char buffer[8], int index);


/// @brief Get a slice of the buffer.
/// @param buffer The buffer of the CAN message.
/// @param start The start index of the slice. Must be between 1 and 7 inclusive.
/// @param offset The offset from the start index. Must be non-negative and combined with the start index must be between 1 and 7 inclusive.
/// @return The value of the slice, or -1 if the slice is invalid.
int get_buffer_value_slice(unsigned char buffer[8], int start, int offset);


/// @brief Receive a CAN message, this function is for the general functionality that both conductor and musician mode will use.
/// @param msg The CAN message to receive.
void receive_can_message(CANMsg* msg);


/// @brief Send a CAN message with the given message ID and buffer.
/// @param msgId The message ID of the CAN message.
/// @param buffer The buffer of the CAN message. Only index 1-7 will be used.
void send_can_message(unsigned char msgId, unsigned char buffer[8]);

/// @brief Send a CAN message with the given message ID and an empty buffer.
/// @param msgId The message ID of the CAN message.
/// @param type The type of the CAN message, which will be put in the first index of the buffer.
void send_can_message_null(unsigned char priority, unsigned char type);


#endif
