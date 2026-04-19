#include "App.h"
#include "MusicPlayer.h"
#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include "CANHelper.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


extern App app;
extern MusicPlayer musicPlayer;
extern SoundPlayer soundPlayer;
extern Can can0;
extern Serial sci0;

/// @brief This function should only be called if we are in musician mode.
/// @param msg The CAN message received.
void musician_receive_can_message(CANMsg* msg) {
  int priority = get_priority(msg->msgId);

  if (priority == PRIORITY_HEARTBEAT) {
    // Heartbeat message.
  } else if (priority == PRIORITY_CONDUCTOR_CLAIM) {
    // Conductor claim message.

  } else if (priority == PRIORITY_CONDUCTOR) {
    // Conductor message.
    int type = get_type(msg->buff);
    int buff_value = atoi((const char *)msg->buff);

    if (type == TYPE_NOTE_TO_PLAY) {
      ASYNC(&musicPlayer, play_note, buff_value);
    } else if (type == TYPE_RESET) {
      ASYNC(&musicPlayer, stop_music, 0);
    } else if (type == TYPE_PLAY) {
      ASYNC(&musicPlayer, toggle_music, 0);
    } else if (type == TYPE_STOP) {
      ASYNC(&musicPlayer, stop_music, 0);
    } else if (type == TYPE_SET_TEMPO) {
      ASYNC(&musicPlayer, set_tempo, buff_value);
    } else if (type == TYPE_SET_KEY) {
      ASYNC(&musicPlayer, set_key, buff_value);
    } else {
      SCI_WRITE(&sci0, "Received type: ");

      // Char can only be maximum three characters long + null terminator.
      char strBuffer[10];
      sprintf(strBuffer, "%d", type);
      SCI_WRITE(&sci0, strBuffer);
      SCI_WRITECHAR(&sci0, '\n');
    }
  } else {
    SCI_WRITE(&sci0, "Received CAN message with ID: ");

    // Char can only be maximum three characters long + null terminator.
    char strBuffer[4];
    sprintf(strBuffer, "%d", msg->msgId);
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
  }
}

