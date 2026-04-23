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

int get_buffer_value_int(unsigned char buffer[8]) {
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

int get_buffer_value_slice(unsigned char buffer[8], int start, int offset) {
  // Check that we have gotten correct values.
  if (start < 0 || start >= 8 || offset < 0 || start + offset >= 8) {
    return -1;
  }

  int value = buffer[start];
  for (int i = 1; i <= offset; i++) {
    value = (value << sizeof(unsigned char)) | buffer[start + i];
  }

  return value;
}

void receive_can_message(CANMsg* msg) {
  int priority = get_priority(msg->msgId);

  if (priority == PRIORITY_HEARTBEAT) {
    // Heartbeat message.
    int type = get_type(msg->buff);
    int group = msg->nodeId;

    if (type == TYPE_HEARTBEAT) {
      ASYNC(&database, update_heartbeat_on, group);
      ASYNC(&database, update_last_heartbeat, group);
    } else if(type == TYPE_CONDUCTOR_SEND_STATE) {
      int current_conductor = SYNC(&database, get_current_conductor, 0);

      if(current_conductor != msg->nodeId) {
        #if LOG_LEVEL >= 1
        SCI_WRITECHAR(&sci0, '\n');
        SCI_WRITE(&sci0, "ERROR | Received conductor send state message from wrong conductor, node: ");
        char strBuffer[4];
        sprintf(strBuffer, "%d", msg->nodeId);
        SCI_WRITE(&sci0, strBuffer);
        SCI_WRITECHAR(&sci0, '\n');
        #endif

        return;
      }

      // First two bytes are the tempo, the third byte is the key.
      int tempo = get_buffer_value_slice(msg->buff, 1, 2);
      int key = msg->buff[3];

      if(tempo == -1) {
        SCI_WRITE(&sci0, "ERROR | Fault when trying to parse the tempo from conductor send state message.\n");
        return;
      }

      if (key == -1) {
        SCI_WRITE(&sci0, "ERROR | Fault when trying to parse the key from conductor send state message.\n");
        return;
      }

      ASYNC(&musicPlayer, set_tempo, tempo);
      ASYNC(&musicPlayer, set_key, key);
      ASYNC(&database, update_heartbeat_on, group);
    } else {
      SCI_WRITE(&sci0, "ERROR | CONDUCTOR SEND STATE | Received type: ");

      // Char can only be maximum three characters long + null terminator.
      char strBuffer[4];
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
    int buff_value = get_buffer_value_int(msg->buff);

    int current_conductor = SYNC(&database, get_current_conductor, 0);

    if(current_conductor != msg->nodeId) {
      #if LOG_LEVEL >= 1
      SCI_WRITECHAR(&sci0, '\n');
      SCI_WRITE(&sci0, "ERROR | Received conductor message from wrong conductor, node: ");
      char strBuffer[4];
      sprintf(strBuffer, "%d", msg->nodeId);
      SCI_WRITE(&sci0, strBuffer);
      SCI_WRITECHAR(&sci0, '\n');
      #endif

      return;
    }

    #if LOG_LEVEL >= 2
    SCI_WRITECHAR(&sci0, '\n');
    SCI_WRITE(&sci0, "INFO | Received conductor message with type: ");
    char strBuffer[10];
    sprintf(strBuffer, "%d", type);
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITE(&sci0, " and value: ");
    sprintf(strBuffer, "%d", buff_value);
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    #endif

    switch (type) {
      case TYPE_NOTE_TO_PLAY:
        // Check are we supposed to be playing this note?
        int buff_note = get_buffer_value_slice(msg->buff, 0, 3);

        int should_we_not = (buff_note + 2) % 3;

        if (should_we_not == 0) {
          SCI_WRITE(&sci0, "Not playing note: ");
          char strBuffer[10];
          sprintf(strBuffer, "%d", buff_note);
          SCI_WRITE(&sci0, strBuffer);
          SCI_WRITECHAR(&sci0, '\n');
          break;
        }

        ASYNC(&musicPlayer, play_note, buff_note);

        SCI_WRITE(&sci0, "Playing note\n");
        SCI_WRITECHAR(&sci0, buff_note);
        break;
      case TYPE_RESET:
        ASYNC(&musicPlayer, set_tempo, 120);
        ASYNC(&musicPlayer, set_key, 0);

        break;
      case TYPE_PLAY:
        ASYNC(&musicPlayer, play_music, 0);
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

void send_can_message(unsigned char msgId, unsigned char buffer[8]) {
  CANMsg msg;

  msg.nodeId = 2;
  msg.msgId = msgId;

  for (int i = 0; i < 8; i++) {
    msg.buff[i] = buffer[i];
  }

  msg.length = 8;

  CAN_SEND(&can0, &msg);

  receive_can_message(&msg);

  #if LOG_LEVEL >= 3
  SCI_WRITECHAR(&sci0, '\n');
  SCI_WRITE(&sci0, "SPAM | Sent CAN message with priority: ");
  char strBuffer[4];
  sprintf(strBuffer, "%d", msgId);
  SCI_WRITE(&sci0, strBuffer);
  SCI_WRITE(&sci0, " and type: ");
  sprintf(strBuffer, "%d", buffer[0]);
  SCI_WRITE(&sci0, strBuffer);
  SCI_WRITECHAR(&sci0, '\n');
  #endif
}

void send_can_message_null(unsigned char priority, unsigned char type) {
  unsigned char buffer[8] = {0};
  buffer[0] = type;
  send_can_message(priority, buffer);
}
