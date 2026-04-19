#include "App.h"
#include "SoundPlayer.h"
#include "MusicPlayer.h"
#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include <stdlib.h>
#include <math.h>

extern MusicPlayer musicPlayer;

void set_playing(SoundPlayer *self, int value) {
  if (!value) {
    ABORT(self->play_tone_msg);
    self->play_tone_msg = NULL;
    DAC_REG = 0;
  }

  self->playing = value;
}
int get_playing(SoundPlayer *self, int unused) {
  return self->playing;
}

void set_period(SoundPlayer *self, int value) {
  self->current_period = value;
}
int get_period(SoundPlayer *self, int unused) {
  return self->current_period;
}

void set_volume(SoundPlayer *self, int value) {
  self->volume = value;
}
int get_volume(SoundPlayer *self, int unused) {
  return self->volume;
}

/* Plays the tone */
void play_tone(SoundPlayer *self, int play) {
  if(!(self->playing)){
    ABORT(self->play_tone_msg);
    self->play_tone_msg = NULL;
    DAC_REG = 0;
    return;
  }

  int muted = SYNC(&musicPlayer, get_mute, 0);
  if(play && (muted == 0)){
    DAC_REG = self->volume;
  }
  else{
    DAC_REG = 0;
  }

  self->play_tone_msg = SEND(USEC(self->current_period), MSEC(50), self, play_tone, !play);
}
