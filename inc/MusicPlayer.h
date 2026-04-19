#ifndef _MusicPlayer_H
#define _MusicPlayer_H

#include "TinyTimber.h"
#include "SoundPlayer.h"
#include <stdbool.h>

#define NOTES_AMOUNT 32
#define MAX_LAG_DEADLINE MSEC(50)

typedef struct {
  Object super;

  int notes[NOTES_AMOUNT];
  int beats[NOTES_AMOUNT];
  int index;
  int tempo;
  int key;
  int playing;
  int muted;

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
  0 \
}

// Public functions
void play_music(MusicPlayer *, int);
void stop_music(MusicPlayer *, int);
void pause_music(MusicPlayer *, int);
void toggle_music(MusicPlayer *, int);

/// @brief Aborts all the messages that are currently pending.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Not used. Required only for the TinyTimber message signature.
void abort_msg(MusicPlayer *, int);


/// @brief This function plays a singular node
/// @param MusicPlayer The MusicPlayer object.
/// @param int The index of the note to play, where 0 is the first index of the period list.
void play_note(MusicPlayer *, int);

/// @brief This function stops the currently playing note.
/// @param MusicPlayer The MusicPlayer object.
/// @param int Not used. Required only for the TinyTimber message signature.
void stop_note(MusicPlayer *, int);


void next_note(MusicPlayer *, int);
void finish_note(MusicPlayer *, int);

// Getters
int get_tempo(MusicPlayer *, int);
int get_key(MusicPlayer *, int);
int get_index(MusicPlayer *, int);
int get_mute(MusicPlayer *, int);

// Setters
void set_tempo(MusicPlayer *, int);
void set_key(MusicPlayer *, int);
void set_index(MusicPlayer *, int);
void set_mute(MusicPlayer *, int);

#endif
