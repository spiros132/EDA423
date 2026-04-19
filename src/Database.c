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
