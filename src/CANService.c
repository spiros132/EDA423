#include "CANService.h"
#include "canTinyTimber.h"
#include "Definitions.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"
#include "Database.h"

#if LOG_LEVEL >= 1
// DEBUG MODE
#include "sciTinyTimber.h"
#include "stdlib.h"

extern Serial sci0;
#endif

extern Can can0;
extern MusicPlayer musicPlayer;
extern SoundPlayer soundPlayer;
extern Database database;


/* HELPER FUNCTIONS */

void send_can_message(CANService *self, unsigned char msgId, unsigned char buffer[8]) {
  // Check if we are in failure mode, either with force silent or if the bus is missing.
  if(self->force_silent || self->bus_can_missing) {
    #if LOG_LEVEL >= 1
    SCI_WRITE(&sci0, "In failure mode, not sending CAN message.\n");
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

  //can_receive_message(self, 0);

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


/* FORCE SILENT FUNCTIONALITY */

void can_set_force_silent(CANService *self, int new_value) {
  self->force_silent = new_value;
}

int can_get_force_silent(CANService *self, int unused) {
  return self->force_silent;
}


/* SEND FUNCTIONS */

void can_send_heartbeat(CANService *self, int unused) {
  send_can_message_null(self, PRIORITY_HEARTBEAT, TYPE_HEARTBEAT);

  #if LOG_LEVEL >= 3
  SCI_WRITE(&sci0, "Sent heartbeat message.\n");
  #endif
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

void can_receive_message(CANService *self, int unused) {
  if (self->force_silent) {
    #if LOG_LEVEL >= 1
    SCI_WRITE(&sci0, "In force silent mode, ignoring received CAN message.\n");
    #endif

    return;
  }

  self->bus_can_missing = false;

  CANMsg msg;
  CAN_RECEIVE(&can0, &msg);


  // Since we got a can message, reset the silent failure to false.
  int group = msg.nodeId;
  int priority = get_priority(msg.msgId);
  int type = get_type(msg.buff);

  if(group != GROUP_7 && group != GROUP_8 && group != GROUP_13) {
    SCI_WRITE(&sci0, "ERROR | Received CAN message with invalid group: ");

    char strBuffer[4];
    itoa(group, strBuffer, 10);
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
        itoa(group, strBuffer, 10);
        SCI_WRITE(&sci0, strBuffer);
        SCI_WRITECHAR(&sci0, '\n');
        #endif

        return;
      }

      // First two bytes are the tempo, the third byte is the key.
      short tempo = get_buffer_short_at(msg.buff, 1);
      signed char key = get_buffer_char_at(msg.buff, 3);

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
      #if LOG_LEVEL >= 1
      SCI_WRITE(&sci0, "ERROR | No conductor has been chosen yet but we got a conductor message.\n");
      #endif

      return;
    }

    if(current_conductor != group) {
      #if LOG_LEVEL >= 1
      SCI_WRITE(&sci0, "ERROR | Received conductor message from wrong conductor, node: ");
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
        char note = (char)msg.buff[1];
        ASYNC(&musicPlayer, set_index, note);
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
        short tempo = get_buffer_short_at(msg.buff, 1);
        ASYNC(&musicPlayer, set_tempo, tempo);
        break;
      case TYPE_SET_KEY:
        signed char key = get_buffer_char_at(msg.buff, 1);
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
    itoa(msg.msgId, strBuffer, 10);
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
  }
}

