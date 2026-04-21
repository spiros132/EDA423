#include "CANHelper.h"
#include "Database.h"
#include "MusicPlayer.h"

#include "sciTinyTimber.h"
#include "canTinyTimber.h"

#include "stdio.h"
#include "stdlib.h"

extern Serial sci0;
extern Can can0;
extern Database database;
extern MusicPlayer musicPlayer;

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

int get_buffer_value(unsigned char buffer[8]) {
  // Buffer starts at index 1, since index 0 is used for the type.
  char strBuffer[8];
  for (int i = 0; i < 7; i++) {
    strBuffer[i] = (char)buffer[i+1];

    if (buffer[i+1] == 0) {
      break;
    }
  }

  return atoi(strBuffer);
}

void receive_can_message(CANMsg* msg) {
int priority = get_priority(msg->msgId);

  if (priority == PRIORITY_HEARTBEAT) {
    // Heartbeat message.
    int type = get_type(msg->buff);
    int group = msg->nodeId;

    if (type == TYPE_HEARTBEAT) {
      SYNC(&database, update_heartbeat, group);
      SYNC(&database, update_last_heartbeat, group);
    } else if(type == TYPE_CONDUCTOR_SEND_STATE) {
      // First two bytes are the tempo, the third byte is the key.
      int uchar_size = sizeof(unsigned char);
      int tempo = (msg->buff[1] << uchar_size) | msg->buff[2];
      int key = msg->buff[3];

      SYNC(&musicPlayer, set_tempo, tempo);
      SYNC(&musicPlayer, set_key, key);
    } else {
      SCI_WRITE(&sci0, "ERROR | CONDUCTOR SEND STATE | Received type: ");

      // Char can only be maximum three characters long + null terminator.
      char strBuffer[10];
      sprintf(strBuffer, "%d", msg->buff[0]);
      SCI_WRITE(&sci0, strBuffer);
      SCI_WRITECHAR(&sci0, '\n');
    }
  } else if (priority == PRIORITY_CONDUCTOR_CLAIM) {
    // Conductor claim message.
    int type = get_type(msg->buff);

    if (type == TYPE_CONDUCTOR_CLAIM) {
      SYNC(&database, update_current_conductor, msg->nodeId);
    } else {
      SCI_WRITE(&sci0, "ERROR | CONDUCTOR CLAIM | Received type: ");

      // Char can only be maximum three characters long + null terminator.
      char strBuffer[10];
      sprintf(strBuffer, "%d", msg->buff[0]);
      SCI_WRITE(&sci0, strBuffer);
      SCI_WRITECHAR(&sci0, '\n');
    }
  } else if (priority == PRIORITY_CONDUCTOR) {
    // Conductor message.
    int type = get_type(msg->buff);
    int buff_value = get_buffer_value(msg->buff);

    switch (type) {
      case TYPE_NOTE_TO_PLAY:
        ASYNC(&musicPlayer, play_note, buff_value);
        break;
      case TYPE_RESET:
        ASYNC(&musicPlayer, stop_music, 0);
        break;
      case TYPE_PLAY:
        ASYNC(&musicPlayer, toggle_music, 0);
        break;
      case TYPE_STOP:
        ASYNC(&musicPlayer, stop_music, 0);
        break;
      case TYPE_SET_TEMPO:
        ASYNC(&musicPlayer, set_tempo, buff_value);
        break;
      case TYPE_SET_KEY:
        ASYNC(&musicPlayer, set_key, buff_value);
        break;
      default:
        SCI_WRITE(&sci0, "ERROR | CONDUCTOR | Received type: ");

        // Char can only be maximum three characters long + null terminator.
        char strBuffer[10];
        sprintf(strBuffer, "%d", msg->buff[0]);
        SCI_WRITE(&sci0, strBuffer);
        SCI_WRITECHAR(&sci0, '\n');
        break;
    }
  } else {
    SCI_WRITE(&sci0, "ERROR | Received CAN message with ID: ");

    // Char can only be maximum three characters long + null terminator.
    char strBuffer[4];
    sprintf(strBuffer, "%d", msg->msgId);
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
  }

}

void send_can_message(unsigned char priority, unsigned char type, char buff[7]) {
  CANMsg msg;

  msg.nodeId = 2;
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
