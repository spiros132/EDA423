#ifndef _MusicPlayer_H
#define _MusicPlayer_H

#include "TinyTimber.h"
#include "SoundPlayer.h"
#include <stdbool.h>

#define NOTES_AMOUNT 32


typedef struct {
  Object super;

  int notes[NOTES_AMOUNT];
  int beats[NOTES_AMOUNT];
  int index;
  int tempo;
  int key;

  int is_playing;
} MusicPlayer;

#define initMusicPlayer() \
{ \
  initObject(), \
  {0, 2, 4, 0, 0, 2, 4, 0, 4, 5, 7, 4, 5, 7, 7, 9, 7, 5, 4, 0, 7, 9, 7, 5, 4, 0, 0, -5, 0, 0, -5, 0}, \
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 2, 2, 4, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 2,  2, 4, 2,  2, 4}, \
  0, \
  120, \
  0, \
  false \
}

/// @brief Start playing the music.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void start_playing(MusicPlayer *, int);

/// @brief Pauses the music without resetting the index.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void pause_playing(MusicPlayer *, int);

/// @brief Stops playing the music and reset the index to 0.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void stop_playing(MusicPlayer *, int);

/// @brief Toggles the play/pause state of the music player.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void toggle_playing(MusicPlayer *, int);

/// @brief Checks if the music player is currently playing.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
/// @return 1 if playing, 0 otherwise.
int is_playing(MusicPlayer *, int);


/// @brief Start playing the song by yourself, deprecated functionality for the HA1.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void next_note(MusicPlayer *, int);

/// @brief This function is called when a note has finished playing. Should ONLY be called from the `next_note` function.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void finish_note(MusicPlayer *, int);


void play_note(MusicPlayer *, int);

void stop_note(MusicPlayer *, int);

/// @brief This function will play a note from start to finish with no interruptions. This function should ONLY be called as a musician.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void play_note(MusicPlayer *, int);

/// @brief This function will stop the currently playing note immediately.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void stop_note(MusicPlayer *, int);


/// @brief Gets the tempo of the music player.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
/// @return The tempo of the music player.
int get_tempo(MusicPlayer *, int);

/// @brief Gets the key of the music player.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
/// @return The key of the music player.
int get_key(MusicPlayer *, int);

/// @brief Gets the index of the music player.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
/// @return The index of the music player.
int get_index(MusicPlayer *, int);



/// @brief Sets the tempo of the music player.
/// @param MusicPlayer The MusicPlayer object.
/// @param int The tempo to set.
void set_tempo(MusicPlayer *, int);

/// @brief Sets the key of the music player.
/// @param MusicPlayer The MusicPlayer object.
/// @param int The key to set.
void set_key(MusicPlayer *, int);

/// @brief Sets the index of the music player. The index determines which note is currently playing.
/// @param MusicPlayer The MusicPlayer object.
/// @param int The index to set.
void set_index(MusicPlayer *, int);

/// @brief Resets music player object to it's default state.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void musicplayer_reset_board(MusicPlayer *, int);

#endif
