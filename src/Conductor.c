#include "App.h"
#include "MusicPlayer.h"
#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include "CANHelper.h"
#include "Conductor.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#if LOG_LEVEL >= 1
extern Serial sci0;
#endif


void conductor_send_state(short tempo, char key) {
  unsigned char buff[8] = {0};

  // Two bytes for the tempo.
  buff[0] = TYPE_CONDUCTOR_SEND_STATE;
  buff[1] = (tempo >> (1 * sizeof(unsigned char))) & 0xFF;
  buff[2] = (tempo >> (0 * sizeof(unsigned char))) & 0xFF;

  // One byte for the key.
  buff[3] = key & 0xFF;

  send_can_message(PRIORITY_HEARTBEAT, buff);

  #if LOG_LEVEL >= 3
  char strBuff[10];

  sprintf(strBuff, "%d", tempo);
  SCI_WRITE(&sci0, "\nSent tempo: ");
  SCI_WRITE(&sci0, strBuff);
  SCI_WRITECHAR(&sci0, '\n');

  sprintf(strBuff, "%d", key);
  SCI_WRITE(&sci0, "\nSent key: ");
  SCI_WRITE(&sci0, strBuff);
  SCI_WRITECHAR(&sci0, '\n');
  #endif
}

void conductor_send_note_to_play(char note) {
  unsigned char buff[8] = {0};

  buff[0] = TYPE_NOTE_TO_PLAY;
  buff[1] = note & 0xFF;

  send_can_message(PRIORITY_CONDUCTOR, buff);

  #if LOG_LEVEL >= 3
  char strBuff[10];
  sprintf(strBuff, "%d", note);
  SCI_WRITE(&sci0, "\nSent note: ");
  SCI_WRITE(&sci0, strBuff);
  SCI_WRITECHAR(&sci0, '\n');
  #endif
}

void conductor_send_reset() {
  send_can_message_null(PRIORITY_CONDUCTOR, TYPE_RESET);

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "\nSent reset message.\n");
  #endif
}

void conductor_send_play() {
  send_can_message_null(PRIORITY_CONDUCTOR, TYPE_PLAY);

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "\nSent play message.\n");
  #endif
}

void conductor_send_stop() {
  send_can_message_null(PRIORITY_CONDUCTOR, TYPE_STOP);

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "\nSent stop message.\n");
  #endif
}

void conductor_send_tempo(short tempo) {
  unsigned char buff[8] = {0};
  buff[0] = TYPE_SET_TEMPO;

  // Two bytes for the tempo.
  buff[1] = (tempo >> (1 * sizeof(unsigned char))) & 0xFF;
  buff[2] = (tempo >> (0 * sizeof(unsigned char))) & 0xFF;

  send_can_message(PRIORITY_CONDUCTOR, buff);

  #if LOG_LEVEL >= 2
  char strBuff[10];
  sprintf(strBuff, "%d", tempo);
  SCI_WRITE(&sci0, "\nSent tempo: ");
  SCI_WRITE(&sci0, strBuff);
  SCI_WRITECHAR(&sci0, '\n');
  #endif
}

void conductor_send_key(char key) {
  unsigned char buff[8] = {0};
  buff[0] = TYPE_SET_KEY;
  buff[1] = key & 0xFF;

  send_can_message(PRIORITY_CONDUCTOR, buff);

  #if LOG_LEVEL >= 2
  char strBuff[10];
  sprintf(strBuff, "%d", key);
  SCI_WRITE(&sci0, "\nSent key: ");
  SCI_WRITE(&sci0, strBuff);
  SCI_WRITECHAR(&sci0, '\n');
  #endif
}
