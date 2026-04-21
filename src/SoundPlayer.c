#include "App.h"
#include "SoundPlayer.h"
#include "MusicPlayer.h"
#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

extern MusicPlayer musicPlayer;
extern Serial sci0;

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
  if (value < 0) {
    value = 0;
  } else if (value > 20) {
    value = 20;
  }

  self->volume = value;

  // Maximum characters are three, since volume can be between 0 and 20, and also null terminator.
  char strBuffer[3];
  sprintf(strBuffer, "%d", value);

  SCI_WRITECHAR(&sci0, '\n');
  SCI_WRITE(&sci0, "Volume set: ");
  SCI_WRITE(&sci0, strBuffer);
  SCI_WRITECHAR(&sci0, '\n');
}


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

void soundplayer_increase_volume(SoundPlayer *self, int unused) {
  int new_volume = self->volume + 1;
  soundplayer_set_volume(self, new_volume);
}

void soundplayer_decrease_volume(SoundPlayer *self, int unused) {
  int new_volume = self->volume - 1;
  soundplayer_set_volume(self, new_volume);
}
