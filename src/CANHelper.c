#include "CANHelper.h"
#include "canTinyTimber.h"
#include "stdlib.h"

extern Can can0;

int get_priority(unsigned char msgID) {
  if (msgID == 0 || msgID == 1 || msgID == 7) {
    return msgID;
  }

  return -1;
}


int get_type(unsigned char buffer[8]) {
  if (0 <= buffer[0] && buffer[0] <= 8) {
    return buffer[0];
  }

  return -1;
}

void send_can_message(unsigned char priority, unsigned char type, char buff[7]) {
  CANMsg msg;

  msg.nodeId = 1;
  msg.msgId = priority;
  msg.buff[0] = type;

  // Copy over the buffer
  for(int i = 0; i < 7; i++) {
    // Check for null terminator;
    msg.buff[i+1] = (unsigned char)buff[i];

    if (buff[i] == 0) {
      msg.length = i+1;
      break;
    }
  }

  CAN_SEND(&can0, &msg);
}

void send_can_message_int(unsigned char priority, unsigned char type, int value) {
  char buffer[7];
  itoa(value, buffer, 10);
  send_can_message(priority, type, buffer);
}

void send_can_message_null(unsigned char priority, unsigned char type) {
  char buffer[8] = {0};
  send_can_message(priority, type, buffer);
}
