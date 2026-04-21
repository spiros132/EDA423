#ifndef _DATABASE_H
#define _DATABASE_H

#include "TinyTimber.h"


#define GROUP_7  0
#define GROUP_8  1
#define GROUP_13 2


/// @brief The database struct is used to save all the data that needs to be shared between objects.
typedef struct {
  Object super;

  /// @brief A boolean value of if we have gotten a heartbeat at all.
  int heartbeats[3];

  /// @brief The last heartbeat time we have gotten from each group.
  int last_heartbeats[3];

  /// @brief The current conductor of the group.
  int current_conductor;

  /// @brief The current note playing.
  int current_note_playing;

  /// @brief The current group playing.
  int current_group_playing;
} Database;

#define initDatabase() \
{ \
  initObject(), \
  /* Heartbeats */ \
  {0, 0, 0}, \
  /* Last heartbeats */ \
  {0, 0, 0}, \
  /* Current conductor */ \
  GROUP_7, \
  /* Current note playing */ \
  0, \
  /* Current group playing */ \
  0 \
}

/// @brief Update the heartbeat of a group.
/// @param Database The database object.
/// @param int The group to update the heartbeat for.
void update_heartbeat(Database *, int);

/// @brief Update the last heartbeat time of a group.
/// @param Database The database object.
/// @param int The group to update the last heartbeat for.
void update_last_heartbeat(Database *, int);

/// @brief Update the current conductor.
/// @param Database The database object.
/// @param int The new current conductor.
void update_current_conductor(Database *, int);

/// @brief Update the current note playing.
/// @param Database The database object.
/// @param int The new current note playing.
void update_current_note_playing(Database *, int);

/// @brief Update the current group playing.
/// @param Database The database object.
/// @param int The new current group playing.
void update_current_group_playing(Database *, int);


/// @brief Get the heartbeat of a group.
/// @param Database The database object.
/// @param int The group to get the heartbeat for.
/// @return If we have gotten the heartbeat of the group.
int get_heartbeat(Database *, int);

/// @brief Get the last heartbeat time of a group.
/// @param Database The database object.
/// @param int The group to get the last heartbeat for.
/// @return The last heartbeat time of the group.
int get_last_heartbeat(Database *, int);

/// @brief Get the current active conductor.
/// @param Database The database object.
/// @param int Unused parameter.
/// @return The current conductor of the group.
int get_current_conductor(Database *, int);

/// @brief Get the current note playing.
/// @param Database The database object.
/// @param int Unused parameter.
/// @return The current note playing.
int get_current_note_playing(Database *, int);

/// @brief Get the current group playing.
/// @param Database The database object.
/// @param int Unused parameter.
/// @return The current group playing.
int get_current_group_playing(Database *, int);

#endif
