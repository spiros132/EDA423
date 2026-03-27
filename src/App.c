#include "App.h"
#include "MusicPlayer.h"
#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include "CANDefinitions.h"
#include <stdlib.h> 
#include <math.h>

extern App app;
extern MusicPlayer musicPlayer;
extern SoundPlayer soundPlayer;
extern Can can0;
extern Serial sci0;

/* Change tone based on key/offset */
int x(int k) {
  return k - (-10);
}

/* Lookup and print the Broder Jacob periods */
void periodLookup(App *self, int key) {
  if(-5 > key && key > 5) return;

  SCI_WRITE(&sci0, "This is the period list: [");

  for (int i = 0; i < sizeof(self->song) / sizeof(self->song[0]); i++) {
    if (i != 0) {
      SCI_WRITE(&sci0, ", ");
    }
    
    char strBuffer[BUFFERLENGTH];
    itoa(self->period[ x(self->song[i] + key) ] , strBuffer, 10);

    SCI_WRITE(&sci0, strBuffer);
  }

  SCI_WRITE(&sci0, "]\n");
}

void finish_input(App *self, int unused) {
  self->buffer[self->bufferLength] = '\0';

  /* Move every value downwards in the history list */
  self->history[2] = self->history[1];
  self->history[1] = self->history[0];
  self->history[0] = atoi(self->buffer);

  if (self->historyLength < 3) {
    self->historyLength++;
  }

  clear_input(self, unused);
}

void clear_input(App *self, int unused) {
  self->bufferLength = 0;
}

void send_can_message(unsigned char msgId, char buff[]) {
  CANMsg msg;

  msg.nodeId = 1;
  msg.msgId = msgId;

  // Copy over the buffer
  for(int i = 0; i < CAN_BUFSIZE; i++) {
    // Check for null terminator;
    msg.buff[i] = (unsigned char)buff[i];

    if (buff[i] == 0) {
      msg.length = i;
      break;
    }
  }

  CAN_SEND(&can0, &msg);
}

void send_can_message_int(unsigned char msgId, int value) {
  char buffer[CAN_BUFSIZE];
  itoa(value, buffer, 10);
  send_can_message(msgId, buffer);
}

void send_can_message_null(unsigned char msgId) {
  char buffer[CAN_BUFSIZE] = {0};
  send_can_message(msgId, buffer);
}

void receiver(App *self, int unused) {
  CANMsg msg;
  CAN_RECEIVE(&can0, &msg);

  int value = atoi((const char *)msg.buff);
  
  if (msg.msgId == FLAG_START) {
    SCI_WRITE(&sci0, "Start can command \n");
    if(self->mode == CONDUCTOR_MODE) return;
    SYNC(&musicPlayer, play_music, 0);
  } else if (msg.msgId == FLAG_STOP) {
    SCI_WRITE(&sci0, "Stop can command \n");
    if(self->mode == CONDUCTOR_MODE) return;
    SYNC(&musicPlayer, stop_music, 0);
  } else if (msg.msgId == FLAG_MUTE) {
    SCI_WRITE(&sci0, "Mute can command \n");
    if(self->mode == CONDUCTOR_MODE) return;
    self->mute = true;
  } else if (msg.msgId == FLAG_UNMUTE) {
    SCI_WRITE(&sci0, "Unmute can command \n");
    if(self->mode == CONDUCTOR_MODE) return;
    self->mute = false;
  } else if (msg.msgId == FLAG_KEY) {
    SCI_WRITE(&sci0, "Change key can command \n");
    if(self->mode == CONDUCTOR_MODE) return;
    ASYNC(&musicPlayer, set_key, value);
  } else if (msg.msgId == FLAG_TEMPO) {
    SCI_WRITE(&sci0, "Change tempo can command \n");
    if(self->mode == CONDUCTOR_MODE) return;
    ASYNC(&musicPlayer, set_tempo, value);
  } else if (msg.msgId == FLAG_VOLUME) {
    SCI_WRITE(&sci0, "Change volume can command \n");
    if(self->mode == CONDUCTOR_MODE) return;
    ASYNC(&soundPlayer, set_volume, value);
  }

  char buff[CAN_BUFSIZE];
  for(int i = 0; i < 8; i++) {
    buff[i] = (char)msg.buff[i];
  }
  buff[7] = '\0';

  SCI_WRITE(&sci0, "Buffer: ");
  SCI_WRITE(&sci0, buff);
  SCI_WRITECHAR(&sci0, '\n');
}

void reader(App *self, int c) {
  if ((char)c == 'y') {
    if (self->mode == MUSICIAN_MODE) {
      SCI_WRITE(&sci0, "Switching to conductor mode \n");
      self->mode = CONDUCTOR_MODE;
    }
    else {
      SCI_WRITE(&sci0, "Switching to musician mode \n");
      self->mode = MUSICIAN_MODE;
    }
  }

  /* Switch for all possible inputs.
   * Accepts 0-9, '-', 'q', 'a', 'w', 's', 'e', 'f', 'm'.
   * See README for use cases.
   */

  char strBuffer[BUFFERLENGTH];

  self->buffer[self->bufferLength] = (char)c;
  switch ((char)c)
  {
  case DELIMITER:
    SCI_WRITECHAR(&sci0, '\n');

    /* Finish the digit/number */
    finish_input(self, 0);

    char strBuffer[BUFFERLENGTH];
    itoa(self->history[0], strBuffer, 10);

    SCI_WRITE(&sci0, "Entered integer: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    break;
  case 'f':
    /* Reset button */
    self->historyLength = 0;
    self->history[0] = 0;
    self->history[1] = 0;
    self->history[2] = 0;
    SCI_WRITE(&sci0, "The 3-history has been erased.\n");
    break;
  case 'q':
    /* Increase the volume */
    if (self->mode == CONDUCTOR_MODE) {
      ASYNC(&musicPlayer, increase_volume, 1);
    }
    self->volume++;
    if(self->volume > 20) {
      self->volume = 20;
    }
    send_can_message_int(FLAG_VOLUME, self->volume);
    break;
  case 'a':
    /* Decrease the volume */
    if (self->mode == CONDUCTOR_MODE) {
      ASYNC(&musicPlayer, decrease_volume, 1);
    }
    self->volume--;
    if(self->volume < 0) {
      self->volume = 0;
    }
    send_can_message_int(FLAG_VOLUME, self->volume);
    break;
  case 'm':
    /* Toggle the mute */
    int mute = SYNC(&musicPlayer, get_mute, 0);
    if (self->mode == CONDUCTOR_MODE) {
       ASYNC(&musicPlayer, set_mute, !mute);
    }
    if(mute) {
      send_can_message_null(FLAG_MUTE);
    } else {
      send_can_message_null(FLAG_UNMUTE);
    }
    break;
  case 'v':
    // Change the volume through the input buffer.
    // Finish the input
    SCI_WRITECHAR(&sci0, '\n');

    /* Finish the digit/number */
    finish_input(self, 0);

    int volume = self->history[0];
    self->volume = volume;

    if (self->mode == CONDUCTOR_MODE) {
      ASYNC(&musicPlayer, set_volume, volume);
    }
    send_can_message_int(FLAG_VOLUME, volume);

    itoa(volume, strBuffer, 10);

    SCI_WRITE(&sci0, "Volume set: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    break;
  case 'k':
    // Change the key through the input buffer.
    // Finish the input
    SCI_WRITECHAR(&sci0, '\n');

    /* Finish the digit/number */
    finish_input(self, 0);

    int key = self->history[0];
    self->key = key;

    if (self->mode == CONDUCTOR_MODE) {
      ASYNC(&musicPlayer, set_key, key);
    }
    
    send_can_message_int(FLAG_KEY, key);

    itoa(key, strBuffer, 10);

    SCI_WRITE(&sci0, "Key set: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    break;
  case 't':
    // Change the key through the input buffer.
    // Finish the input
    SCI_WRITECHAR(&sci0, '\n');

    /* Finish the digit/number */
    finish_input(self, 0);

    int tempo = self->history[0];
    self->tempo = tempo;

    if (self->mode == CONDUCTOR_MODE) {
      ASYNC(&musicPlayer, set_tempo, tempo);
    }
    send_can_message_int(FLAG_TEMPO, tempo);

    itoa(tempo, strBuffer, 10);

    SCI_WRITE(&sci0, "Tempo set: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    break;
  case 's':
    if (self->mode == CONDUCTOR_MODE) {
      SYNC(&musicPlayer, stop_music, 0);
    }
    send_can_message_null(FLAG_STOP);
    break;
  case ' ':
  case 'p':
    if (self->mode == CONDUCTOR_MODE) {
      SYNC(&musicPlayer, toggle_music, 0);
    }
    self->playing = !self->playing;

    if (self->playing) {
      send_can_message_null(FLAG_START);
    } else {
      send_can_message_null(FLAG_STOP);
    }
    break;
  case '-':
    if (self->bufferLength != 0)
      return;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    /* Digit or '-' */
    SCI_WRITECHAR(&sci0, self->buffer[self->bufferLength]);
    self->bufferLength++;
  default:
    return;
  }
}

void startApp(App *self, int arg) {
  CANMsg msg;

  CAN_INIT(&can0);
  SCI_INIT(&sci0);
  SCI_WRITE(&sci0, "Hello, hello...\n");
  SCI_WRITE(&sci0, "Enter integer: \n");
}

int main() {
  INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
  INSTALL(&can0, can_interrupt, CAN_IRQ0);
  TINYTIMBER(&app, startApp, 0);
  return 0;
}
