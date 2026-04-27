#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

/**
 * This file contains all the definitions that are needed.
 */


/* Modes of Operation Definitions */

#define CONDUCTOR_MODE 0
#define MUSICIAN_MODE  1

/* Group Definitions */

#define GROUP_7  0
#define GROUP_8  1
#define GROUP_13 2

/* CAN Message Definitions */

/// @brief First priority, used for heartbeat messages.
#define PRIORITY_HEARTBEAT 0
/// @brief Second priority, used for conductor claim messages.
#define PRIORITY_CONDUCTOR_CLAIM 1
/// @brief Third priority, used for conductor messages.
#define PRIORITY_CONDUCTOR 7

/// @brief Message type for heartbeat messages.
#define TYPE_HEARTBEAT 0
/// @brief Message type for conductor send state messages.
#define TYPE_CONDUCTOR_SEND_STATE 1
/// @brief Message type for conductor claim messages.
#define TYPE_CONDUCTOR_CLAIM 2
/// @brief Message type for note to play messages.
#define TYPE_NOTE_TO_PLAY 3
/// @brief Message type for reset messages, resetting the tempo and key of the musician boards.
#define TYPE_RESET 4
/// @brief Message type for play messages.
#define TYPE_PLAY 5
/// @brief Message type for stop messages.
#define TYPE_STOP 6
/// @brief Message type for set tempo messages.
#define TYPE_SET_TEMPO 7
/// @brief Message type for set key messages.
#define TYPE_SET_KEY 8


/* Timing Definitions */

/// @brief The period for printing out the debug information about the current tempo and key.
#define PRINT_DEBUG_INFO_PERIOD SEC(5)

/// @brief The time between two heartbeat messages to check if the other boards are alive.
#define HEARTBEAT_TIMEOUT MSEC(250)

/// @brief The maximum deadline to have so that there's no lag.
#define MAX_LAG_DEADLINE MSEC(50)

/// @brief The timeout for claiming to be a conductor.
#define CONDUCTOR_CLAIM_TIMEOUT MSEC(2000)


/* Key Definitions */

/// @brief The key for submitting the input buffer as a value.
#define KEY_SUBMIT 'e'
/// @brief The key for clearing the input buffer.
#define KEY_RESET 'f'

/// @brief The key for increasing the volume.
#define KEY_VOLUME_INC 'q'
/// @brief The key for decreasing the volume.
#define KEY_VOLUME_DEC 'a'

/// @brief The key for muting/unmuting the music.
#define KEY_MUTE 't'

/// @brief The key for setting the volume to a specific value.
#define KEY_SET_VOLUME 'z'
/// @brief The key for setting the key of the music to a specific value.
#define KEY_SET_KEY 'n'
/// @brief The key for setting the tempo to a specific value.
#define KEY_SET_TEMPO 'c'

/// @brief The key for stopping the music.
#define KEY_MUSIC_STOP 's'
/// @brief The key for toggling the music (play/stop).
#define KEY_MUSIC_PLAY 'p'
/// @brief The key for toggling the music (play/stop).
#define KEY_MUSIC_PLAY_2 ' '
/// @brief The key for restarting the music.
#define KEY_MUSIC_RESTART 'u'


/// @brief The key for enabling debug printing of tempo and key.
#define KEY_TOGGLE_DEBUG_PRINTING 'd'

/// @brief The key for resetting the conductors values, both tempo and key.
#define KEY_CONDUCTOR_RESET 'r'

/// @brief The key for conductorship claim.
#define KEY_CLAIM_CONDUCTORSHIP 'y'

#define KEY_PRINT_MEMBERSHIPS 'm'

/* Failure Definitions */

#define KEY_FAILURE_1 'j'
#define KEY_FAILURE_2 'k'


/* Problem 5 */
#define KEY_SEND_ONE_MESSAGE 'o'
#define KEY_START_BURST 'b'
#define KEY_STOP_BURST 'x'

#define INTER_ARRIVAL_TIME MSEC(1000)
#define BUFFER_SIZE 100

/* Debug Definitions */

/// @brief The log level for the project. 0 means no logs, 1 means only important logs, 2 means all logs, 3 means all logs plus some spam logs.
#define LOG_LEVEL 2

#endif
