#include "CANService.h"
#include "canTinyTimber.h"
#include "Definitions.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"
#include "Database.h"
#include "App.h"

#if LOG_LEVEL >= 1
// DEBUG MODE
#include "sciTinyTimber.h"
#include "stdlib.h"

extern Serial sci0;
#endif

extern Can can0;
extern App app;
extern MusicPlayer musicPlayer;
extern SoundPlayer soundPlayer;
extern Database database;


/* HELPER FUNCTIONS */

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

signed char get_buffer_char_at(unsigned char buffer[8], int index) {
  return (signed char)buffer[index];
}

short get_buffer_short_at(unsigned char buffer[8], int index) {
  short value = (
    (buffer[index + 0] << 8) |
    (buffer[index + 1] << 0)
  );

  return value;
}

int get_buffer_int_at(unsigned char buffer[8], int index) {
  int value = (
    (buffer[index + 0] << 24) |
    (buffer[index + 1] << 16) |
    (buffer[index + 2] <<  8) |
    (buffer[index + 3] <<  0)
  );

  return value;
}

int find_buffered_message_index(CANService *self, int msgId) {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (!self->received_msg_used[i]) {
      continue;
    }

    if (self->received_msg[i].msgId == msgId) {
      return i;
    }
  }

  return -1;
}

void internal_receive_message(CANMsg *msg) {
  // We don't need to do anything here for problem 5

  SCI_WRITE(&sci0, "Delivered CAN message\n");
  return;

  // Since we got a can message, reset the silent failure to false.
  int group = msg->nodeId;
  int priority = get_priority(msg->msgId);
  int type = get_type(msg->buff);

  if(group != GROUP_7 && group != GROUP_8 && group != GROUP_13) {
    SCI_WRITE(&sci0, "ERROR | Received CAN message with invalid group: ");

    char strBuffer[4];
    itoa(group, strBuffer, 10);
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');

    return;
  }

  #if LOG_LEVEL >= 3
  SCI_WRITE(&sci0, "SPAM | Received CAN message with priority: ");
  char strBuffer[4];
  itoa(priority, strBuffer, 10);
  SCI_WRITE(&sci0, strBuffer);
  SCI_WRITE(&sci0, " and type: ");
  itoa(type, strBuffer, 10);
  SCI_WRITE(&sci0, strBuffer);
  SCI_WRITECHAR(&sci0, '\n');
  #endif

  if (priority == PRIORITY_HEARTBEAT) {
    if (type == TYPE_HEARTBEAT) {
      BEFORE(MAX_LAG_DEADLINE, &database, update_last_heartbeat, group);
    } else if(type == TYPE_CONDUCTOR_SEND_STATE) {
      int current_conductor = SYNC(&database, get_current_conductor, 0);

      // If there is no conductor yet, set the conductor to the group of the received message.
      if (current_conductor == -1) {
        SYNC(&database, set_current_conductor, group);
        current_conductor = group;

        #if LOG_LEVEL >= 2
        SCI_WRITE(&sci0, "INFO | No conductor has been chosen yet, setting conductor to group: ");
        char strBuffer[4];
        itoa(group, strBuffer, 10);
        SCI_WRITE(&sci0, strBuffer);
        SCI_WRITECHAR(&sci0, '\n');
        #endif
      }

      // Check if the conductor is the correct one.
      if(current_conductor != group) {
        #if LOG_LEVEL >= 1
        SCI_WRITECHAR(&sci0, '\n');
        SCI_WRITE(&sci0, "ERROR | Received conductor send state message from wrong conductor, node: ");
        char strBuffer[4];
        itoa(group, strBuffer, 10);
        SCI_WRITE(&sci0, strBuffer);
        SCI_WRITECHAR(&sci0, '\n');
        #endif

        return;
      }

      // First two bytes are the tempo, the third byte is the key.
      short tempo = get_buffer_short_at(msg->buff, 1);
      signed char key = get_buffer_char_at(msg->buff, 3);

      ASYNC(&musicPlayer, set_tempo, tempo);
      ASYNC(&musicPlayer, set_key, key);

      // Update the heartbeat for the conductor.
      BEFORE(MAX_LAG_DEADLINE, &database, update_last_heartbeat, group);
    } else {
      SCI_WRITE(&sci0, "ERROR | CONDUCTOR SEND STATE | Received type: ");

      // Char can only be maximum three characters long + null terminator.
      char strBuffer[4];
      itoa(type, strBuffer, 10);
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
      itoa(type, strBuffer, 10);
      SCI_WRITE(&sci0, strBuffer);
      SCI_WRITECHAR(&sci0, '\n');
    }
  } else if (priority == PRIORITY_CONDUCTOR) {
    // Conductor message.
    int current_conductor = SYNC(&database, get_current_conductor, 0);

    if(current_conductor == -1) {
      SYNC(&database, set_current_conductor, group);
      current_conductor = group;

      #if LOG_LEVEL >= 2
      SCI_WRITE(&sci0, "INFO | No conductor has been chosen yet, setting conductor to group: ");
      char strBuffer[4];
      itoa(group, strBuffer, 10);
      SCI_WRITE(&sci0, strBuffer);
      SCI_WRITECHAR(&sci0, '\n');
      #endif

      return;
    }

    // Check if the conductor is the correct one.
    if(current_conductor != group) {
      #if LOG_LEVEL >= 1
      SCI_WRITECHAR(&sci0, '\n');
      SCI_WRITE(&sci0, "ERROR | Received conductor send state message from wrong conductor, node: ");
      char strBuffer[4];
      itoa(group, strBuffer, 10);
      SCI_WRITE(&sci0, strBuffer);
      SCI_WRITECHAR(&sci0, '\n');
      #endif

      return;
    }

    #if LOG_LEVEL >= 2
    SCI_WRITE(&sci0, "INFO | Received conductor message with type: ");
    char strBuffer[10];
    itoa(type, strBuffer, 10);
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    #endif

    switch (type) {
      case TYPE_NOTE_TO_PLAY:
        char note = (char)msg->buff[1];
        SYNC(&musicPlayer, set_index, note);
        if (SYNC(&database, get_mode, 0) == MUSICIAN_MODE) {
          SYNC(&musicPlayer, abort_msg, 0);
          SYNC(&musicPlayer, start_playing, 0);
        }
        break;
      case TYPE_RESET:
        ASYNC(&musicPlayer, set_tempo, 120);
        ASYNC(&musicPlayer, set_key, 0);
        break;
      case TYPE_PLAY:
        ASYNC(&musicPlayer, start_playing, 0);
        break;
      case TYPE_STOP:
        ASYNC(&musicPlayer, stop_playing, 0);
        break;
      case TYPE_SET_TEMPO:
        short tempo = get_buffer_short_at(msg->buff, 1);
        ASYNC(&musicPlayer, set_tempo, tempo);
        break;
      case TYPE_SET_KEY:
        signed char key = get_buffer_char_at(msg->buff, 1);
        ASYNC(&musicPlayer, set_key, key);
        break;
      default:
        SCI_WRITE(&sci0, "ERROR | CONDUCTOR | Received type: ");

        // Char can only be maximum three characters long + null terminator.
        char strBuffer[10];
        itoa(type, strBuffer, 10);
        SCI_WRITE(&sci0, strBuffer);
        SCI_WRITECHAR(&sci0, '\n');
        break;
    }
  } else {
    SCI_WRITE(&sci0, "ERROR | Received CAN message with ID: ");

    // Char can only be maximum three characters long + null terminator.
    char strBuffer[4];
    itoa(msg->msgId, strBuffer, 10);
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
  }
}

void send_can_message(CANService *self, unsigned char msgId, unsigned char buffer[8]) {
  // Check if we are in failure mode, either with force silent or if the bus is missing.
  if(self->force_silent || self->bus_can_missing) {
    #if LOG_LEVEL >= 1
    SCI_WRITE(&sci0, "In failure mode, skipping send of the CAN message.\n");
    #endif

    return;
  }

  CANMsg msg;

  msg.nodeId = GROUP_13;
  msg.msgId = msgId;

  for (int i = 0; i < 8; i++) {
    msg.buff[i] = buffer[i];
  }

  msg.length = 8;

  self->bus_can_missing = CAN_SEND(&can0, &msg);

  if(self->bus_can_missing) {
    SCI_WRITE(&sci0, "Silent Failure\n");
    ASYNC(&app, reset_board, 0);
    stop_heartbeat_message(self, 0);

    if(SYNC(&database, get_mode, 0) == CONDUCTOR_MODE) {
      self->conductor_last = true;
    }
  }

  // Whenever we send a message receive it immediately.
  //internal_receive_message(&msg);

  #if LOG_LEVEL >= 3
  SCI_WRITECHAR(&sci0, '\n');
  SCI_WRITE(&sci0, "SPAM | Sent CAN message with priority: ");
  char strBuffer[4];
  itoa(msgId, strBuffer, 10);
  SCI_WRITE(&sci0, strBuffer);
  SCI_WRITE(&sci0, " and type: ");
  itoa(buffer[0], strBuffer, 10);
  SCI_WRITE(&sci0, strBuffer);
  SCI_WRITECHAR(&sci0, '\n');
  #endif
}

void send_can_message_null(CANService *self, unsigned char priority, unsigned char type) {
  unsigned char buffer[8] = {0};
  buffer[0] = type;
  send_can_message(self, priority, buffer);
}


/* FORCE SILENT FUNCTIONALITY */

void can_set_force_silent(CANService *self, int new_value) {
  self->force_silent = new_value;

  if (new_value) {
    stop_heartbeat_message(self, 0);

    if(SYNC(&database, get_mode, 0) == CONDUCTOR_MODE) {
      self->conductor_last = true;
    }
  } else {
    if(self->conductor_last) {
      self->conductor_last = false;
      SCI_WRITE(&sci0, "I Now Join As A Musician\n");
    }
  }
}

int can_get_force_silent(CANService *self, int unused) {
  return self->force_silent;
}


/* SEND FUNCTIONS */

void can_send_heartbeat(CANService *self, int unused) {
  if(self->force_silent || self->bus_can_missing) {
    return;
  }

  if (SYNC(&database, get_mode, 0) == MUSICIAN_MODE) {
    send_can_message_null(self, PRIORITY_HEARTBEAT, TYPE_HEARTBEAT);
  } else {
    signed char key = SYNC(&musicPlayer, get_key, 0);
    short tempo = SYNC(&musicPlayer, get_tempo, 0);
    int args = can_send_conductor_state_pack(tempo, key);

    can_send_conductor_state(self, args);
  }
  #if LOG_LEVEL >= 3
  SCI_WRITE(&sci0, "Sent heartbeat message.\n");
  #endif

  if (self->heartbeat_message != NULL) {
    ABORT(self->heartbeat_message);
    self->heartbeat_message = NULL;
  }

  self->heartbeat_message = SEND(MSEC(150), MAX_LAG_DEADLINE, self, can_send_heartbeat, 0);
}

void stop_heartbeat_message(CANService *self, int unused) {
  if (self->heartbeat_message != NULL) {
    ABORT(self->heartbeat_message);
    self->heartbeat_message = NULL;

    #if LOG_LEVEL >= 2
    SCI_WRITE(&sci0, "Stopped heartbeat message loop.\n");
    #endif
  }
}

void can_send_conductor_state(CANService *self, int args) {
  short tempo = can_send_conductor_state_unpack_tempo(args);
  char key = can_send_conductor_state_unpack_key(args);

  unsigned char buff[8] = {0};

  // Two bytes for the tempo.
  buff[0] = TYPE_CONDUCTOR_SEND_STATE;
  buff[1] = (tempo >> 8) & 0xFF;
  buff[2] = (tempo >> 0) & 0xFF;

  // One byte for the key.
  buff[3] = key & 0xFF;

  send_can_message(self, PRIORITY_HEARTBEAT, buff);
}

void can_send_conductor_claim(CANService *self, int unused) {
  send_can_message_null(self, PRIORITY_CONDUCTOR_CLAIM, TYPE_CONDUCTOR_CLAIM);

  #if LOG_LEVEL >= 1
  SCI_WRITE(&sci0, "Sent conductor claim message.\n");
  #endif
}

void can_send_note_to_play(CANService *self, int note) {
  unsigned char buff[8] = {0};

  buff[0] = TYPE_NOTE_TO_PLAY;
  buff[1] = note & 0xFF;

  send_can_message(self, PRIORITY_CONDUCTOR, buff);

  #if LOG_LEVEL >= 3
  char strBuff[10];
  itoa(note, strBuff, 10);
  SCI_WRITE(&sci0, "Sent note: ");
  SCI_WRITE(&sci0, strBuff);
  SCI_WRITECHAR(&sci0, '\n');
  #endif
}

void can_send_reset(CANService *self, int unused) {
  send_can_message_null(self, PRIORITY_CONDUCTOR, TYPE_RESET);

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "Sent reset message.\n");
  #endif
}

void can_send_play(CANService *self, int unused) {
  send_can_message_null(self, PRIORITY_CONDUCTOR, TYPE_PLAY);

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "Sent play message.\n");
  #endif
}

void can_send_stop(CANService *self, int unused) {
  send_can_message_null(self, PRIORITY_CONDUCTOR, TYPE_STOP);

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "Sent stop message.\n");
  #endif
}

void can_send_set_tempo(CANService *self, int tempo) {
  unsigned char buff[8] = {0};
  buff[0] = TYPE_SET_TEMPO;

  // Two bytes for the tempo.
  buff[1] = (tempo >> 8) & 0xFF;
  buff[2] = (tempo >> 0) & 0xFF;

  send_can_message(self, PRIORITY_CONDUCTOR, buff);

  #if LOG_LEVEL >= 2
  char strBuff[10];
  itoa(tempo, strBuff, 10);
  SCI_WRITE(&sci0, "Sent tempo: ");
  SCI_WRITE(&sci0, strBuff);
  SCI_WRITECHAR(&sci0, '\n');
  #endif
}

void can_send_set_key(CANService *self, int key) {
  unsigned char buff[8] = {0};
  buff[0] = TYPE_SET_KEY;
  buff[1] = key & 0xFF;

  send_can_message(self, PRIORITY_CONDUCTOR, buff);

  #if LOG_LEVEL >= 2
  char strBuff[10];
  itoa(key, strBuff, 10);
  SCI_WRITE(&sci0, "\nSent key: ");
  SCI_WRITE(&sci0, strBuff);
  SCI_WRITECHAR(&sci0, '\n');
  #endif
}

void can_send_one_message(CANService *self, int unused) {
  unsigned char buff[8] = {0};
  send_can_message(self, self->current_send_index, buff);

  self->current_send_index = (self->current_send_index + 1) % 128;

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "Sent a one message\n");
  #endif
}

void can_start_burst(CANService *self, int unused) {
  if(self->burst_message != NULL) {
    ABORT(self->burst_message);
    self->burst_message = NULL;
  }

  self->burst_message = BEFORE(MAX_LAG_DEADLINE, self, can_send_burst, 0);

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "Started burst\n");
  #endif
}

void can_send_burst(CANService *self, int unused) {
  can_send_one_message(self, 0);

  self->burst_message = SEND(MSEC(500), MAX_LAG_DEADLINE, self, can_send_burst, 0);
}

void can_stop_burst(CANService *self, int unused) {
  if (self->burst_message != NULL) {
    ABORT(self->burst_message);
    self->burst_message = NULL;

    #if LOG_LEVEL >= 2
    SCI_WRITE(&sci0, "Stopped burst\n");
    #endif
  }
}

void can_deliver_message(CANService *self, int index) {
  // Get next message to deliver if it exists.

  // Check for each index in the received messages.
  // If we can get to a message where the index is one more than the last delivered, deliver the message.
  // Otherwise

  if (!self->received_msg_used[index]) {
    SCI_WRITE(&sci0, "ERROR | Tried to deliver a CAN message that does not exist at index: ");
    char strBuff[10];
    itoa(index, strBuff, 10);
    SCI_WRITE(&sci0, strBuff);
    SCI_WRITECHAR(&sci0, '\n');
    return;
  }

  CANMsg msgPtr;

  msgPtr.msgId = self->received_msg[index].msgId;
  msgPtr.nodeId = self->received_msg[index].nodeId;
  msgPtr.length = self->received_msg[index].length;
  for (int i = 0; i < 8; i++) {
    msgPtr.buff[i] = self->received_msg[index].buff[i];
  }

  int delivered_msg_id = msgPtr.msgId;

  internal_receive_message(&msgPtr);

  self->received_msg_used[index] = 0;

  int buffer_length = 0;
  for(int i = 0; i < BUFFER_SIZE; i++) {
    if(self->received_msg_used[i]) {
      buffer_length++;
    }
  }
  if(buffer_length == 0) {
    self->preferred_index = 0;
  }

  self->last_delivered_index = delivered_msg_id;
  T_RESET(&self->last_delivered_timer);

  int next_index = find_buffered_message_index(self, (delivered_msg_id + 1) % 128);
  if (next_index != -1) {
    SEND(INTER_ARRIVAL_TIME, MAX_LAG_DEADLINE, self, can_deliver_message, next_index);
  }
}

void can_receive_message(CANService *self, int unused) {
  CANMsg msg;
  CAN_RECEIVE(&can0, &msg);

  // If we have waited more than the inter arrival time,
  // deliver the message immediately,
  // otherwise, put it in the buffer and send the can_deliver_message. Also check if there are any other messages that we should deliver afterwards.

  Time diff = T_SAMPLE(&self->last_delivered_timer);

  #if LOG_LEVEL >= 3
  SCI_WRITE(&sci0, "Received CAN message with ID: ");
  char strBuff[10];
  itoa(msg.msgId, strBuff, 10);
  SCI_WRITE(&sci0, strBuff);
  SCI_WRITE(&sci0, " and time since last delivered message: ");
  itoa(MSEC_OF(diff), strBuff, 10);
  SCI_WRITE(&sci0, strBuff);
  SCI_WRITECHAR(&sci0, '\n');
  #endif

  int i = -1;

  // Check if there is space in the buffer at or after the preferred index.
  for (int offset = 0; offset < BUFFER_SIZE; offset++) {
    int candidate = (self->preferred_index + offset) % BUFFER_SIZE;

    if (!self->received_msg_used[candidate]) {
      i = candidate;
      self->preferred_index = (candidate + 1) % BUFFER_SIZE;
      break;
    }
  }

  if (i == -1) {
    SCI_WRITE(&sci0, "ERROR | CAN receive buffer overflow, dropping message.\n");
    return;
  }

  self->received_msg[i].msgId = msg.msgId;
  self->received_msg[i].nodeId = msg.nodeId;
  self->received_msg[i].length = msg.length;
  for (int j = 0; j < 8; j++) {
    self->received_msg[i].buff[j] = msg.buff[j];
  }
  self->received_msg_used[i] = 1;

  // Check if the message we just got is supposed to be the next index.
  if (msg.msgId == ((self->last_delivered_index + 1) % 128)) {
    Time delay = 0;

    if (diff < INTER_ARRIVAL_TIME) {
      delay = INTER_ARRIVAL_TIME - diff;
    }

    SEND(delay, MAX_LAG_DEADLINE, self, can_deliver_message, i);
  } else {
    #if LOG_LEVEL >= 3
    SCI_WRITE(&sci0, "Received out of order message. Message ID: ");
    char strBuff[10];
    itoa(msg.msgId, strBuff, 10);
    SCI_WRITE(&sci0, strBuff);
    SCI_WRITECHAR(&sci0, '\n');
    #endif
  }

  // if(self->bus_can_missing) {
  //   SCI_WRITE(&sci0, "Leaving Silent Failure\n");
  //   self->bus_can_missing = false;

  //   if(self->conductor_last) {
  //     self->conductor_last = false;
  //     SCI_WRITE(&sci0, "I Now Join As A Musician\n");
  //   }
  // }

  // if (self->force_silent) {
  //   #if LOG_LEVEL >= 1
  //   SCI_WRITE(&sci0, "In force silent mode, skipping received CAN message.\n");
  //   #endif

  //   stop_heartbeat_message(self, 0);

  //   return;
  // }

  // if(self->heartbeat_message == NULL) {
  //   self->heartbeat_message = BEFORE(MAX_LAG_DEADLINE, self, can_send_heartbeat, 0);
  // }

  // internal_receive_message(&msg);
}


