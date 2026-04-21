#include "App.h"
#include "SoundPlayer.h"
#include "MusicPlayer.h"
#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include <stdlib.h>
#include <math.h>

extern MusicPlayer musicPlayer;

void soundplayer_set_playing(SoundPlayer *self, int value) {
  if (!value) {
    ABORT(self->play_tone_msg);
    self->play_tone_msg = NULL;
    DAC_REG = 0;
  }

  self->playing = value;
}

void soundplayer_set_period(SoundPlayer *self, int value) {
  self->current_period = value;
}

void soundplayer_set_volume(SoundPlayer *self, int value) {
  self->volume = value;
}


/* Plays the tone */
void soundplayer_play_tone(SoundPlayer *self, int play) {
  if(!(self->playing)){
    ABORT(self->play_tone_msg);
    self->play_tone_msg = NULL;
    DAC_REG = 0;
    return;
  }

  int muted = self->muted;
  if(play && (muted == 0)){
    DAC_REG = self->volume;
  }
  else{
    DAC_REG = 0;
  }

  self->play_tone_msg = SEND(USEC(self->current_period), MSEC(50), self, soundplayer_play_tone, !play);
}
