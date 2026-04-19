#ifndef _MUSICIAN_H
#define _MUSICIAN_H

#include "canTinyTimber.h"

/// @brief This function should only be called if we are in musician mode.
/// @param msg The CAN message received.
void musician_receive_can_message(CANMsg* msg);

#endif
