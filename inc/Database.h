#ifndef _DATABASE_H
#define _DATABASE_H

#include "TinyTimber.h"
#include "Definitions.h"

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

  /// @brief The groups that are claiming to be conductors.
  int conductor_claims[3];

  /// @brief The messages that are sent to check if we have gotten a heartbeat from the other groups.
  Msg check_heartbeat_msgs[3];

  Msg check_conductor_claim_msg;
} Database;

#define initDatabase() \
{ \
  initObject(), \
  /* Heartbeats */ \
  {0, 0, 0}, \
  /* Last heartbeats */ \
  {0, 0, 0}, \
  /* Current conductor */ \
  -1, \
  /* Current note playing */ \
  0, \
  /* Current group playing */ \
  0, \
  /* Conductor claims */ \
  {0, 0, 0}, \
  /* Check heartbeat messages */ \
  {NULL, NULL, NULL}, \
  /* Check conductor claim message */ \
  NULL \
}

/// @brief Update the heartbeat of a group and set it to true.
/// @param Database The database object.
/// @param int The group to update the heartbeat for.
void update_heartbeat_on(Database *, int);

/// @brief Update the heartbeat of a group and set it to false.
/// @param Database The database object.
/// @param int The group to update the heartbeat for.
void update_heartbeat_off(Database *, int);

/// @brief Update the last heartbeat time of a group.
/// @param Database The database object.
/// @param int The group to update the last heartbeat for.
void update_last_heartbeat(Database *, int);

/// @brief Update the current conductor.
/// @param Database The database object.
/// @param int The new current conductor.
void update_current_conductor(Database *, int);

/// @brief Update the current conductor.
/// @param Database The database object.
/// @param int The new current conductor.
void set_current_conductor(Database *, int);

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

/// @brief Get the amount of active boards based on the heartbeats.
/// @param Database The database object.
/// @param int Unused parameter.
/// @return The amount of active boards.
int get_active_boards(Database *, int);


/// @brief Check the heartbeat for the group 7. This function should be called with a delay and aborted if we get a heartbeat before the delay is over.
/// @param Database The database object.
/// @param int The last heartbeat time to check against.
void check_heartbeat_7(Database *, int);

/// @brief Check the heartbeat for the group 8. This function should be called with a delay and aborted if we get a heartbeat before the delay is over.
/// @param Database The database object.
/// @param int The last heartbeat time to check against.
void check_heartbeat_8(Database *, int);

/// @brief Check the heartbeat for the group 13. This function should be called with a delay and aborted if we get a heartbeat before the delay is over.
/// @param Database The database object.
/// @param int The last heartbeat time to check against.
void check_heartbeat_13(Database *, int);

/// @brief Check which groups have called for a conductor claim and update the current conductor accordingly. If there are multiple claims, the group with the lowest rank gets selected.
/// @param Database The database object.
/// @param int Unused parameter.
void check_conductor_claim(Database *, int);


#endif
