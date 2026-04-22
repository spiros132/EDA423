#include "Database.h"
#include "TinyTimber.h"
#include "Definitions.h"
#include "sciTinyTimber.h"
#include "stdio.h"
#include "App.h"

extern App app;
extern Serial sci0;

void update_heartbeat_on(Database *self, int group) {
  self->heartbeats[group] = 1;
}

void update_heartbeat_off(Database *self, int group) {
  self->heartbeats[group] = 0;
}

void update_last_heartbeat(Database *self, int group) {
  int current_time = CURRENT_OFFSET();

  // Also call the function that will check in a bit if we have gotten a heartbeat from the other groups.
  switch (group) {
    case GROUP_7:
      if (self->check_heartbeat_msgs[GROUP_7] != NULL) {
        ABORT(self->check_heartbeat_msgs[GROUP_7]);
        self->check_heartbeat_msgs[GROUP_7] = NULL;
      }

      self->check_heartbeat_msgs[GROUP_7] = SEND(HEARTBEAT_TIMEOUT - MAX_LAG_DEADLINE, MAX_LAG_DEADLINE, self, check_heartbeat_7, current_time);
      break;
    case GROUP_8:
      if (self->check_heartbeat_msgs[GROUP_8] != NULL) {
        ABORT(self->check_heartbeat_msgs[GROUP_8]);
        self->check_heartbeat_msgs[GROUP_8] = NULL;
      }

      self->check_heartbeat_msgs[GROUP_8] = SEND(HEARTBEAT_TIMEOUT - MAX_LAG_DEADLINE, MAX_LAG_DEADLINE, self, check_heartbeat_8, current_time);
      break;
    case GROUP_13:
      if (self->check_heartbeat_msgs[GROUP_13] != NULL) {
        ABORT(self->check_heartbeat_msgs[GROUP_13]);
        self->check_heartbeat_msgs[GROUP_13] = NULL;
      }

      self->check_heartbeat_msgs[GROUP_13] = SEND(HEARTBEAT_TIMEOUT - MAX_LAG_DEADLINE, MAX_LAG_DEADLINE, self, check_heartbeat_13, current_time);
      break;
    default:
      SCI_WRITE(&sci0, "Invalid group for heartbeat update\n");
      return;
  }

  self->last_heartbeats[group] = current_time;
}

void update_current_conductor(Database *self, int conductor) {
  self->conductor_claims[conductor] = 1;

  if (self->check_conductor_claim_msg == NULL) {
    self->check_conductor_claim_msg = SEND(CONDUCTOR_CLAIM_TIMEOUT, MAX_LAG_DEADLINE, self, check_conductor_claim, 0);
  }
}

void update_current_note_playing(Database *self, int note) {
  self->current_note_playing = note;
}

void update_current_group_playing(Database *self, int group) {
  self->current_group_playing = group;
}


int get_heartbeat(Database *self, int group) {
  if (group < 0 || group > 2) {
    return -1;
  }

  return self->heartbeats[group];
}

int get_last_heartbeat(Database *self, int group) {
  if (group < 0 || group > 2) {
    return -1;
  }

  return self->last_heartbeats[group];
}

int get_current_conductor(Database *self, int unused) {
  return self->current_conductor;
}

int get_current_note_playing(Database *self, int unused) {
  return self->current_note_playing;
}

int get_current_group_playing(Database *self, int unused) {
  return self->current_group_playing;
}


void check_heartbeat_7(Database *self, int last_heartbeat_time) {
  update_heartbeat_off(self, GROUP_7);

  // No heartbeat from the conductor
  if(self->current_conductor == GROUP_7) {
    if (self->heartbeats[GROUP_8] == 1) {
      self->current_conductor = GROUP_8;

      #if LOG_LEVEL >= 1
      SCI_WRITE(&sci0, "\nNo heartbeat from conductor, switching to new conductor: 8\n");
      #endif

    } else if (self->heartbeats[GROUP_13] == 1) {
      self->current_conductor = GROUP_13;

      #if LOG_LEVEL >= 1
      SCI_WRITE(&sci0, "\nNo heartbeat from conductor, switching to new conductor: 13\n");
      #endif
    }
  }
}

void check_heartbeat_8(Database *self, int last_heartbeat_time) {
  update_heartbeat_off(self, GROUP_8);

  if (self->heartbeats[GROUP_13] == 1) {
    self->current_conductor = GROUP_13;

    #if LOG_LEVEL >= 1
    SCI_WRITE(&sci0, "\nNo heartbeat from conductor, switching to new conductor: 13\n");
    #endif
  }
}

void check_heartbeat_13(Database *self, int last_heartbeat_time) {
  update_heartbeat_off(self, GROUP_13);

  if (self->heartbeats[GROUP_13] == 1) {
    SCI_WRITE(&sci0, "\nWe shouldn't be able to get here.\n");
  }
}

void check_conductor_claim(Database *self, int unused) {
  self->check_conductor_claim_msg = NULL;

  // The first conductor found with the lowest rank will become the conductor.
  for (int i = 0; i < 3; i++) {
    if (self->conductor_claims[i] == 1) {
      self->current_conductor = i;

      #if LOG_LEVEL >= 1
      char strBuffer[2];
      sprintf(strBuffer, "%d", i);

      SCI_WRITECHAR(&sci0, '\n');
      SCI_WRITE(&sci0, "Chosen new conductor: ");
      SCI_WRITE(&sci0, strBuffer);
      SCI_WRITECHAR(&sci0, '\n');
      #endif

      int mode = SYNC(&app, get_mode, 0);

      if (i == GROUP_13) {
        SYNC(&app, set_mode, CONDUCTOR_MODE);

        SCI_WRITE(&sci0, "\nClaimed Conductorship\n");
      } else if(mode == CONDUCTOR_MODE) {
        SYNC(&app, set_mode, MUSICIAN_MODE);

        SCI_WRITE(&sci0, "\nConductorship Void\n");
      }

      return;
    }
  }
}
