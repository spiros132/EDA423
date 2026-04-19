#include "App.h"
#include "MusicPlayer.h"
#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include "CANHelper.h"
#include "Musician.h"
#include "Conductor.h"
#include "Definitions.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

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

void receiver(App *self, int unused) {
  CANMsg msg;
  CAN_RECEIVE(&can0, &msg);

  if(self->mode == MUSICIAN_MODE) {
    musician_receive_can_message(&msg);
  } else if (self->mode == CONDUCTOR_MODE) {
    conductor_receive_can_message(&msg);
  } else {
    SCI_WRITE(&sci0, "Mode is invalid in receiver: ");

    char strBuffer[10];
    sprintf(strBuffer, "%d", self->mode);
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
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
  case KEY_SUBMIT:
    SCI_WRITECHAR(&sci0, '\n');

    /* Finish the digit/number */
    finish_input(self, 0);

    char strBuffer[BUFFERLENGTH];
    itoa(self->history[0], strBuffer, 10);

    SCI_WRITE(&sci0, "Entered integer: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    break;
  case KEY_RESET:
    /* Reset button */
    self->historyLength = 0;
    self->history[0] = 0;
    self->history[1] = 0;
    self->history[2] = 0;
    SCI_WRITE(&sci0, "The 3-history has been erased.\n");
    break;
  case KEY_VOLUME_INC:
    /* Increase the volume */
    SCI_WRITECHAR(&sci0, '\n');

    int inc_volume = SYNC(&musicPlayer, get_volume, 0) + 1;

    if (inc_volume > 20) {
      inc_volume = 20;
    }

    self->volume = inc_volume;
    ASYNC(&musicPlayer, set_volume, inc_volume);

    itoa(inc_volume, strBuffer, 10);

    SCI_WRITE(&sci0, "Volume set: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    break;
  case KEY_VOLUME_DEC:
    /* Decrease the volume */
    SCI_WRITECHAR(&sci0, '\n');

    int dec_volume = SYNC(&musicPlayer, get_volume, 0) - 1;

    if (dec_volume < 0) {
      dec_volume = 0;
    }

    self->volume = dec_volume;
    ASYNC(&musicPlayer, set_volume, dec_volume);

    itoa(dec_volume, strBuffer, 10);

    SCI_WRITE(&sci0, "Volume set: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    break;
  case KEY_MUTE:
    /* Toggle the mute */
    int mute = SYNC(&musicPlayer, get_mute, 0);
    if (self->mode == CONDUCTOR_MODE) {
       ASYNC(&musicPlayer, set_mute, !mute);
    }
    if(mute) {
      //send_can_message_null(FLAG_MUTE);
    } else {
      //send_can_message_null(FLAG_UNMUTE);
    }
    break;
  case KEY_SET_VOLUME:
    // Change the volume through the input buffer.
    // Finish the input
    SCI_WRITECHAR(&sci0, '\n');

    /* Finish the digit/number */
    finish_input(self, 0);

    int volume = self->history[0];

    if (volume < 0) {
      volume = 0;
    } else if (volume > 20) {
      volume = 20;
    }

    self->volume = volume;
    ASYNC(&musicPlayer, set_volume, volume);

    itoa(volume, strBuffer, 10);

    SCI_WRITE(&sci0, "Volume set: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    break;
  case KEY_SET_KEY:
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

    send_can_message_int(PRIORITY_CONDUCTOR, TYPE_SET_KEY, key);

    itoa(key, strBuffer, 10);

    SCI_WRITE(&sci0, "Key set: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    break;
  case KEY_SET_TEMPO:
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
    send_can_message_int(PRIORITY_CONDUCTOR, TYPE_SET_TEMPO, tempo);

    itoa(tempo, strBuffer, 10);

    SCI_WRITE(&sci0, "Tempo set: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
    break;
  case KEY_MUSIC_STOP:
    if (self->mode == CONDUCTOR_MODE) {
      SYNC(&musicPlayer, stop_music, 0);
    }
    send_can_message_null(PRIORITY_CONDUCTOR, TYPE_RESET);
    break;
  case KEY_MUSIC_PLAY:
  case KEY_MUSIC_PLAY_2:
    if (self->mode == CONDUCTOR_MODE) {
      SYNC(&musicPlayer, toggle_music, 0);
    }
    self->playing = !self->playing;

    if (self->playing) {
      send_can_message_null(PRIORITY_CONDUCTOR, TYPE_PLAY);
    } else {
      send_can_message_null(PRIORITY_CONDUCTOR, TYPE_STOP);
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
