#ifndef _CAN_HELPER_H
#define _CAN_HELPER_H

#include "Definitions.h"


/// @brief Get the priority of the CAN message.
/// @param msgID The message ID of the CAN message.
/// @return The priority of the CAN message, or -1 if the priority is unknown.
int get_priority(unsigned char msgID);

/// @brief Get the type of the CAN message.
/// @param buffer The buffer of the CAN message.
/// @return The type of the CAN message, or -1 if the type is unknown.
int get_type(unsigned char buffer[8]);


/// @brief Send a CAN message with the given message ID and buffer.
/// @param msgId The message ID of the CAN message.
/// @param buff The buffer of the CAN message. Can be max 7 bytes long.
void send_can_message(unsigned char priority, unsigned char type, char buff[7]);

/// @brief Send a CAN message with the given message ID and an integer value as the buffer.
/// @param msgId The message ID of the CAN message.
/// @param value The integer value to send in the CAN message buffer.
void send_can_message_int(unsigned char priority, unsigned char type, int value);

/// @brief Send a CAN message with the given message ID and an empty buffer.
/// @param msgId The message ID of the CAN message.
void send_can_message_null(unsigned char priority, unsigned char type);


#endif
