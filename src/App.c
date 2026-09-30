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

void start_failure_mode(App *self, int unused) {
  SYNC(&canService, can_set_force_silent, true);

  reset_board(self, 0);

  SYNC(&canService, stop_heartbeat_message, 0);

  SCI_WRITE(&sci0, "Silent Failure\n");
}

void stop_failure_mode(App *self, int unused) {
  SYNC(&canService, can_set_force_silent, false);
  SYNC(&canService, can_send_heartbeat, 0);

  SEND(HEARTBEAT_TIMEOUT, MAX_LAG_DEADLINE, self, check_active_boards, 0);

  SCI_WRITE(&sci0, "Leave Silent Failure\n");
}

void check_active_boards(App *self, int unused) {
  // Check if we are alone in the network, if so become the conductor and start playing the melody.
  int active_boards = SYNC(&database, get_active_boards, 0);
  if (active_boards == 1) {
    SYNC(&database, set_current_conductor, GROUP_13);
    ASYNC(&musicPlayer, start_playing, 0);
  }
}

void reset_board(App *self, int unused) {
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

int get_time(App *self, int unused) {
}

void print_debug_info(App *self, int unused) {
  int mode = SYNC(&database, get_mode, 0);

  if(mode == CONDUCTOR_MODE) {
    int tempo = SYNC(&musicPlayer, get_tempo, 0);

    char strBuffer[4];
    itoa(tempo, strBuffer, 10);

    SCI_WRITECHAR(&sci0, '\n');
    SCI_WRITE(&sci0, "Current tempo: ");
    SCI_WRITE(&sci0, strBuffer);
    SCI_WRITECHAR(&sci0, '\n');
  } else if(mode == MUSICIAN_MODE) {
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

void print_memberships() {
  int active_boards = SYNC(&database, get_active_boards, 0);
  int current_conductor = SYNC(&database, get_current_conductor, 0);

  char strBuffer[2];
  itoa(active_boards, strBuffer, 10);

  SCI_WRITECHAR(&sci0, '\n');
  SCI_WRITE(&sci0, "Active boards: ");
  SCI_WRITE(&sci0, strBuffer);
  SCI_WRITECHAR(&sci0, '\n');

  for (int i = 0; i < 3; i++) {
    int heartbeat = SYNC(&database, get_heartbeat, i);

    SCI_WRITE(&sci0, "Group ");
    switch (i) {
      case GROUP_7:
        SCI_WRITE(&sci0, " 7");
        break;
      case GROUP_8:
        SCI_WRITE(&sci0, " 8");
        break;
      case GROUP_13:
        SCI_WRITE(&sci0, "13");
        break;
      default:
        SCI_WRITE(&sci0, "unknown");
    }

    SCI_WRITE(&sci0, " - Heartbeat: ");
    if (heartbeat == 1) {
      SCI_WRITE(&sci0, "Alive");
    } else {
      SCI_WRITE(&sci0, " Dead");
    }

    if (current_conductor == i) {
      SCI_WRITE(&sci0, " (Conductor)");
    } else {
      SCI_WRITE(&sci0, " (Musician)");
    }

    SCI_WRITECHAR(&sci0, '\n');
  }
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

  int force_silent = SYNC(&canService, can_get_force_silent, 0);
  int mode = SYNC(&database, get_mode, 0);

  self->buffer[self->bufferLength] = (char)c;

  switch ((char)c)
  {
  case KEY_CLAIM_CONDUCTORSHIP:
    SYNC(&canService, can_send_conductor_claim, 0);
    break;
  case KEY_SUBMIT:
    SCI_WRITECHAR(&sci0, '\n');

    /* Finish the digit/number */
    finish_input(self, 0);

    char strBuffer[10];
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

    if (mode == CONDUCTOR_MODE) {
      int key = self->history[0];

      SYNC(&canService, can_send_set_key, key);
    }
    break;
  case KEY_SET_TEMPO:
    finish_input(self, 0);

    if (mode == CONDUCTOR_MODE) {
      int tempo = self->history[0];

      SYNC(&canService, can_send_set_tempo, tempo);
    }
    break;
  case KEY_MUSIC_STOP:
    if (mode == CONDUCTOR_MODE) {
      SYNC(&canService, can_send_stop, 0);
    }
    break;
  case KEY_MUSIC_PLAY:
  case KEY_MUSIC_PLAY_2:
    if (mode == CONDUCTOR_MODE) {
      SYNC(&canService, can_send_play, 0);
    }
    break;
  case KEY_MUSIC_RESTART:
    if(mode == CONDUCTOR_MODE) {
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
    if (mode == CONDUCTOR_MODE) {
      SYNC(&canService, can_send_reset, 0);
    }
    break;
  case KEY_FAILURE_1:
    if(force_silent) {
      stop_failure_mode(self, 0);
    } else {
      start_failure_mode(self, 0);
    }
    break;
  case KEY_FAILURE_2:
    if(!force_silent) {
      start_failure_mode(self, 0);

      int time_to_wait = (CURRENT_OFFSET() % 5) + 6; // Random time between 6 and 10 seconds.
      SEND(SEC(time_to_wait), MAX_LAG_DEADLINE, self, stop_failure_mode, 0);
    }
    break;
  case KEY_PRINT_MEMBERSHIPS:
    print_memberships();
    break;
  case KEY_SEND_ONE_MESSAGE:
    SYNC(&canService, can_send_one_message, 0);
    break;
  case KEY_START_BURST:
    SYNC(&canService, can_start_burst, 0);
    break;
  case KEY_STOP_BURST:
    SYNC(&canService, can_stop_burst, 0);
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

  //SYNC(&canService, can_send_heartbeat, 0);

  SYNC(&canService, reset_timer, 0);
}

int main() {
  INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
  INSTALL(&can0, can_interrupt, CAN_IRQ0);
  TINYTIMBER(&app, startApp, 0);
  return 0;
}
