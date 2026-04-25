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
  int playing;

  Msg next_note_msg;
  Msg finish_note_msg;
} MusicPlayer;

#define initMusicPlayer() \
{ \
  initObject(), \
  {0, 2, 4, 0, 0, 2, 4, 0, 4, 5, 7, 4, 5, 7, 7, 9, 7, 5, 4, 0, 7, 9, 7, 5, 4, 0, 0, -5, 0, 0, -5, 0}, \
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 2, 2, 4, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 2,  2, 4, 2,  2, 4}, \
  0, \
  120, \
  0, \
  0, \
  NULL, \
  NULL \
}


/// @brief Start playing the music.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void play_music(MusicPlayer *, int);

/// @brief Stop playing the music and reset the index to 0.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void stop_music(MusicPlayer *, int);

/// @brief Pauses the music without resetting the index.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void pause_music(MusicPlayer *, int);

/// @brief Toggles the play/pause state of the music player.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void toggle_music(MusicPlayer *, int);


/// @brief Aborts all the messages that are currently pending.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Not used. Required only for the TinyTimber message signature.
void abort_msg(MusicPlayer *, int);


/// @brief Start playing the song by yourself, deprecated functionality for the HA1.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void next_note(MusicPlayer *, int);

/// @brief This function is called when a note has finished playing. Should ONLY be called from the `next_note` function.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Unused parameter.
void finish_note(MusicPlayer *, int);


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


#endif
