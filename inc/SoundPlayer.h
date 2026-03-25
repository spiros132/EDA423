#ifndef _SoundPlayer_H
#define _SoundPlayer_H

#include "TinyTimber.h"
#include <stdbool.h>

typedef struct {
  Object super;

  /* Bool to stop the recursive calls of the player */
  int playing;

  /* Period of the current tone */
  int current_period;

  /* Volume for the player */
  int volume;

  Msg play_tone_msg;
} SoundPlayer;

#define toSoundPlayer(child) ((SoundPlayer*) child)

#define initSoundPlayer() \
{ \
  initObject(), \
  0, \
  0, \
  5, \
}

/* Plays the tone. */
void play_tone(SoundPlayer *, int);
void background_task(SoundPlayer *, int);

/* Getters and setters */
void set_playing(SoundPlayer *, int);
int get_playing(SoundPlayer *, int);
void set_period(SoundPlayer *, int);
int get_period(SoundPlayer *, int);
void set_volume(SoundPlayer *, int);
int get_volume(SoundPlayer *, int);

#endif
