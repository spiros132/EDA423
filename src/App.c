#include "App.h"
#include "MusicPlayer.h"
#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include "Definitions.h"
#include "Database.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"
#include "CANService.h"
#include <stdlib.h>
#include <math.h>

extern App app;
extern MusicPlayer musicPlayer;
extern SoundPlayer soundPlayer;
extern Database database;
extern CANService canService;

extern Can can0;
extern Serial sci0;

/* Change tone based on key/offset */
int x(int k) {
  return k - (-10);
}

int get_mode(App *self, int unused) {
  return self->mode;
}

void set_mode(App *self, int mode) {
  self->mode = mode;
}

void start_failure_mode(App *self, int unused) {
  self->silent_failure = true;

  reset_board(self, 0);

  #if LOG_LEVEL >= 1
  SCI_WRITE(&sci0, "Entered silent failure mode.\n");
  #endif
}

void stop_failure_mode(App *self, int unused) {
  self->silent_failure = false;

  heartbeat_tick(self, 0);

  #if LOG_LEVEL >= 1
  SCI_WRITE(&sci0, "Exited silent failure mode.\n");
  #endif
}

void reset_board(App *self, int unused) {
  self->mode = MUSICIAN_MODE;

  BEFORE(MAX_LAG_DEADLINE, &database, database_reset_board, 0);
  BEFORE(MAX_LAG_DEADLINE, &musicPlayer, musicplayer_reset_board, 0);
  BEFORE(MAX_LAG_DEADLINE, &soundPlayer, soundplayer_reset_board, 0);
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

void heartbeat_tick(App *self, int unused) {
  if (self->heartbeat_msg != NULL) {
    ABORT(self->heartbeat_msg);
    self->heartbeat_msg = NULL;

    #if LOG_LEVEL >= 1
    SCI_WRITE(&sci0, "ERROR | Duplicate heartbeat message. Aborting the last one.\n");
    #endif
  }

  if (self->silent_failure) {
    SCI_WRITE(&sci0, "ERROR | Heartbeat message can't be sent, we are in the silent failure mode.\n");
    return;
  }

  // Send the heartbeat to ourselves as well.
  BEFORE(MAX_LAG_DEADLINE, &database, update_last_heartbeat, GROUP_13);

  // Send a heartbeat message every 200ms.
  if(self->mode == MUSICIAN_MODE) {
    SYNC(&canService, can_send_heartbeat, 0);
  } else if (self->mode == CONDUCTOR_MODE) {
    int tempo = SYNC(&musicPlayer, get_tempo, 0);
    int key = SYNC(&musicPlayer, get_key, 0);

    int args = can_send_conductor_state_pack(tempo, key);
    SYNC(&canService, can_send_conductor_state, args);
  }

  self->heartbeat_msg = SEND(MSEC(200), MAX_LAG_DEADLINE, self, heartbeat_tick, 0);
}

void print_debug_info(App *self, int unused) {
  if(self->mode == CONDUCTOR_MODE) {
    int tempo = SYNC(&musicPlayer, get_tempo, 0);

    char strBuffer[4];
    itoa(tempo, strBuffer, 10);

    SCI_WRITECHAR(&sci0, '\n');
    SCI_WRITE(&sci0, "Current tempo: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
  } else if(self->mode == MUSICIAN_MODE) {
    // Get if the board is muted or not.
    int muted = SYNC(&soundPlayer, soundplayer_get_muted, 0);
    if (muted)  {
      SCI_WRITE(&sci0, "MUTED\n");
    }
  } else {
    SCI_WRITE(&sci0, "Unknown mode in print_debug_info.\n");
  }

  self->debug_msg = SEND(PRINT_DEBUG_INFO_PERIOD, MAX_LAG_DEADLINE, self, print_debug_info, 0);
}

void clear_input(App *self, int unused) {
  self->bufferLength = 0;
}

void receiver(App *self, int unused) {
  SYNC(&canService, can_receive_message, 0);
}

void reader(App *self, int c) {
  /* Switch for all possible inputs.
   * Accepts 0-9, '-', 'q', 'a', 'w', 's', 'e', 'f', 'm'.
   * See README for use cases.
   */

  self->buffer[self->bufferLength] = (char)c;

  switch ((char)c)
  {
  case KEY_CLAIM_CONDUCTORSHIP:
    can_send_conductor_claim(&canService, 0);
    break;
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
    ASYNC(&soundPlayer, soundplayer_increase_volume, 0);
    break;
  case KEY_VOLUME_DEC:
    /* Decrease the volume */
    ASYNC(&soundPlayer, soundplayer_decrease_volume, 0);
    break;
  case KEY_MUTE:
    ASYNC(&soundPlayer, soundplayer_toggle_muted, 0);
    break;
  case KEY_SET_VOLUME:
    /* Finish the digit/number */
    finish_input(self, 0);

    int volume = self->history[0];

    // Send the volume down to the sound player.
    ASYNC(&soundPlayer, soundplayer_set_volume, volume);
    break;
  case KEY_SET_KEY:
    finish_input(self, 0);

    if (self->mode == CONDUCTOR_MODE) {
      int key = self->history[0];

      SYNC(&canService, can_send_set_key, key);
    }
    break;
  case KEY_SET_TEMPO:
    finish_input(self, 0);

    if (self->mode == CONDUCTOR_MODE) {
      int tempo = self->history[0];

      SYNC(&canService, can_send_set_tempo, tempo);
    }
    break;
  case KEY_MUSIC_STOP:
    if (self->mode == CONDUCTOR_MODE) {
      SYNC(&canService, can_send_stop, 0);
    }
    break;
  case KEY_MUSIC_PLAY:
  case KEY_MUSIC_PLAY_2:
    if (self->mode == CONDUCTOR_MODE) {
      SYNC(&canService, can_send_play, 0);
    }
    break;
  case KEY_MUSIC_RESTART:
    if(self->mode == CONDUCTOR_MODE) {
      SYNC(&canService, can_send_stop, 0);
      SYNC(&canService, can_send_play, 0);
    }
    break;
  case KEY_TOGGLE_DEBUG_PRINTING:
    if (self->debug_msg != NULL) {
      ABORT(self->debug_msg);
      self->debug_msg = NULL;
      SCI_WRITE(&sci0, "Debug printing disabled.\n");
    } else {
      self->debug_msg = SEND(PRINT_DEBUG_INFO_PERIOD, MAX_LAG_DEADLINE, self, print_debug_info, 0);
      SCI_WRITE(&sci0, "Debug printing enabled.\n");
    }
    break;
  case KEY_CONDUCTOR_RESET:
    if (self->mode == CONDUCTOR_MODE) {
      SYNC(&canService, can_send_reset, 0);
    }
    break;
  case KEY_FAILURE_1:
    if(self->silent_failure) {
      stop_failure_mode(self, 0);
    } else {
      start_failure_mode(self, 0);
    }
    break;
  case KEY_FAILURE_2:
    if(self->silent_failure != true) {
      start_failure_mode(self, 0);
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

  heartbeat_tick(self, 0);

  // Check for other conductors.
  //int current_time = CURRENT_OFFSET();

  //SEND(MSEC(200), MAX_LAG_DEADLINE, &database, check_heartbeat_7, current_time);
  //SEND(MSEC(200), MAX_LAG_DEADLINE, &database, check_heartbeat_8, current_time);
}

int main() {
  INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
  INSTALL(&can0, can_interrupt, CAN_IRQ0);
  TINYTIMBER(&app, startApp, 0);
  return 0;
}
