#include "Database.h"
#include "TinyTimber.h"

void update_heartbeat(Database *self, int group) {
  self->heartbeats[group] = 1;
}

void update_last_heartbeat(Database *self, int group) {
  self->last_heartbeats[group] = CURRENT_OFFSET();
}

void update_current_conductor(Database *self, int conductor) {
  self->current_conductor = conductor;
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
