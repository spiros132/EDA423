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

/// @brief A boolean value to indicate if we have lost connection when trying to send a message with the CAN bus.
static int silent_failure = 0;

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

short get_buffer_short_at(unsigned char buffer[8], int index) {
  if (index < 0 || index > 6) {
    return -1;
  }

  char char_size = sizeof(unsigned char);

  short value = (
    (buffer[index + 0] << (8 * char_size)) |
    (buffer[index + 1] << (0 * char_size))
  );

  return value;
}

int get_buffer_int_at(unsigned char buffer[8], int index) {
  if (index < 0 || index > 4) {
    return -1;
  }

  char char_size = sizeof(unsigned char);

  int value = (
    (buffer[index + 0] << (24 * char_size)) |
    (buffer[index + 1] << (16 * char_size)) |
    (buffer[index + 2] << (8  * char_size)) |
    (buffer[index + 3] << (0  * char_size))
  );

  return value;
}

void receive_can_message(CANMsg* msg) {
  // Since we got a can message, reset the silent failure to false.
  silent_failure = false;

  int group = msg->nodeId;
  int priority = get_priority(msg->msgId);
  int type = get_type(msg->buff);

  if(group != GROUP_7 && group != GROUP_8 && group != GROUP_13) {
    SCI_WRITE(&sci0, "ERROR | Received CAN message with invalid group: ");

    char strBuffer[4];
    sprintf(strBuffer, "%d", group);
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');

    return;
  }


  if (priority == PRIORITY_HEARTBEAT) {
    if (type == TYPE_HEARTBEAT) {
      BEFORE(MAX_LAG_DEADLINE, &database, update_last_heartbeat, group);
    } else if(type == TYPE_CONDUCTOR_SEND_STATE) {
      int current_conductor = SYNC(&database, get_current_conductor, 0);

      // If there is no conductor yet, set the conductor to the group of the received message.
      if (current_conductor == -1) {
        SYNC(&database, set_current_conductor, group);
        current_conductor = group;
      }

      // Check if the conductor is the correct one.
      if(current_conductor != group) {
        #if LOG_LEVEL >= 1
        SCI_WRITECHAR(&sci0, '\n');
        SCI_WRITE(&sci0, "ERROR | Received conductor send state message from wrong conductor, node: ");
        char strBuffer[4];
        sprintf(strBuffer, "%d", group);
        SCI_WRITE(&sci0, strBuffer);
        SCI_WRITECHAR(&sci0, '\n');
        #endif

        return;
      }

      // First two bytes are the tempo, the third byte is the key.
      short tempo = get_buffer_short_at(msg->buff, 1);
      char key = (char)msg->buff[3];

      if(tempo == -1) {
        SCI_WRITE(&sci0, "ERROR | Fault when trying to parse the tempo from conductor send state message.\n");
        return;
      }

      ASYNC(&musicPlayer, set_tempo, tempo);
      ASYNC(&musicPlayer, set_key, key);

      // Update the heartbeat for the conductor.
      BEFORE(MAX_LAG_DEADLINE, &database, update_last_heartbeat, group);
    } else {
      SCI_WRITE(&sci0, "ERROR | CONDUCTOR SEND STATE | Received type: ");

      // Char can only be maximum three characters long + null terminator.
      char strBuffer[4];
      sprintf(strBuffer, "%d", type);
      SCI_WRITE(&sci0, strBuffer);
      SCI_WRITECHAR(&sci0, '\n');
    }
  } else if (priority == PRIORITY_CONDUCTOR_CLAIM) {
    if (type == TYPE_CONDUCTOR_CLAIM) {
      SYNC(&database, update_current_conductor, group);
    } else {
      SCI_WRITE(&sci0, "ERROR | CONDUCTOR CLAIM | Received type: ");

      // Char can only be maximum three characters long + null terminator.
      char strBuffer[10];
      sprintf(strBuffer, "%d", type);
      SCI_WRITE(&sci0, strBuffer);
      SCI_WRITECHAR(&sci0, '\n');
    }
  } else if (priority == PRIORITY_CONDUCTOR) {
    // Conductor message.
    int current_conductor = SYNC(&database, get_current_conductor, 0);

    if(current_conductor == -1) {
      #if LOG_LEVEL >= 1
      SCI_WRITE(&sci0, "ERROR | No conductor has been chosen yet but we got a conductor message.\n");
      #endif

      return;
    }

    if(current_conductor != group) {
      #if LOG_LEVEL >= 1
      SCI_WRITECHAR(&sci0, '\n');
      SCI_WRITE(&sci0, "ERROR | Received conductor message from wrong conductor, node: ");
      char strBuffer[4];
      sprintf(strBuffer, "%d", group);
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
    SCI_WRITECHAR(&sci0, '\n');
    #endif

    switch (type) {
      case TYPE_NOTE_TO_PLAY:
        char note = (char)msg->buff[1];
        ASYNC(&musicPlayer, set_index, note);
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
        short tempo = get_buffer_short_at(msg->buff, 1);
        ASYNC(&musicPlayer, set_tempo, tempo);
        break;
      case TYPE_SET_KEY:
        char key = (char)msg->buff[1];
        ASYNC(&musicPlayer, set_key, key);
        break;
      default:
        SCI_WRITE(&sci0, "ERROR | CONDUCTOR | Received type: ");

        // Char can only be maximum three characters long + null terminator.
        char strBuffer[10];
        sprintf(strBuffer, "%d", type);
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
  if (silent_failure) {
    SCI_WRITE(&sci0, "ERROR | We have lost connection to the CAN bus, cannot send message. Now waiting for reconnection...\n");
    return;
  }

  CANMsg msg;

  msg.nodeId = GROUP_13;
  msg.msgId = msgId;

  for (int i = 0; i < 8; i++) {
    msg.buff[i] = buffer[i];
  }

  msg.length = 8;

  silent_failure = CAN_SEND(&can0, &msg);

  //receive_can_message(&msg);

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
