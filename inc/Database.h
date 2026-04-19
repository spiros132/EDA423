#ifndef _DATABASE_H
#define _DATABASE_H

#include "TinyTimber.h"


#define GROUP_7  0
#define GROUP_8  1
#define GROUP_13 2


/// @brief The database struct is used to save all the data about heartbeats, conductor claims, conductor messages and other CAN messages.
typedef struct {
  Object super;

  /// @brief A boolean value of if we have gotten a heartbeat at all.
  int heartbeats[3];

  /// @brief The last heartbeat time we have gotten from each group.
  int last_heartbeats[3];

  int current_conductor;
} Database;

#define initDatabase() \
{ \
  initObject(), \
  {0, 0, 0}, \
  {0, 0, 0}, \
  GROUP_7 \
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

#endif
