#include "App.h"
#include "SoundPlayer.h"
#include "MusicPlayer.h"
#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include <stdlib.h>
#include <math.h>

extern MusicPlayer musicPlayer;

#if LOG_LEVEL >= 2
// Only log level 2 needs the serial interface here.
extern Serial sci0;
#endif


void soundplayer_play_tone(SoundPlayer *self, int play) {
  if(!(self->playing)){
    if(self->play_tone_msg != NULL) {
      ABORT(self->play_tone_msg);
      self->play_tone_msg = NULL;
    }

    DAC_REG = 0;

    #if LOG_LEVEL >= 2
    SCI_WRITE(&sci0, "INFO | Sound player is not playing, cannot play tone\n");
    #endif

    return;
  }

  int muted = self->muted;
  if (play && (muted == 0)) {
    DAC_REG = self->volume;

    #if LOG_LEVEL >= 3
    SCI_WRITE(&sci0, "INFO | Playing tone\n");
    #endif
  } else {
    DAC_REG = 0;

    #if LOG_LEVEL >= 3
    SCI_WRITE(&sci0, "INFO | Not playing tone\n");
    #endif
  }

  self->play_tone_msg = SEND(USEC(self->current_period), MSEC(50), self, soundplayer_play_tone, !play);
}


void soundplayer_set_playing(SoundPlayer *self, int value) {
  if (!value) {
    if (self->play_tone_msg != NULL) {
      ABORT(self->play_tone_msg);
      self->play_tone_msg = NULL;
    }
    DAC_REG = 0;
  }

  self->playing = value;

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "INFO | Sound player set playing: ");
  if (value) {
    SCI_WRITE(&sci0, "true\n");
  } else {
    SCI_WRITE(&sci0, "false\n");
  }
  #endif
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

  #if LOG_LEVEL >= 2
  // Maximum characters are three, since volume can be between 0 and 20, and also null terminator.
  char strBuffer[3];
  itoa(value, strBuffer, 10);

  SCI_WRITECHAR(&sci0, '\n');
  SCI_WRITE(&sci0, "INFO | Volume set: ");
  SCI_WRITE(&sci0, strBuffer);
  SCI_WRITECHAR(&sci0, '\n');
  #endif
}

void soundplayer_set_muted(SoundPlayer *self, int muted) {
  self->muted = muted;
}

int soundplayer_get_muted(SoundPlayer *self, int unused) {
  return self->muted;
}


void soundplayer_increase_volume(SoundPlayer *self, int unused) {
  int new_volume = self->volume + 1;
  soundplayer_set_volume(self, new_volume);

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "INFO | Increased volume\n");
  #endif
}

void soundplayer_decrease_volume(SoundPlayer *self, int unused) {
  int new_volume = self->volume - 1;
  soundplayer_set_volume(self, new_volume);

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "INFO | Decreased volume\n");
  #endif
}

void soundplayer_toggle_muted(SoundPlayer *self, int unused) {
  int new_muted = !self->muted;
  soundplayer_set_muted(self, new_muted);

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "INFO | Toggled mute\n");
  #endif
}

void soundplayer_reset_board(SoundPlayer *self, int unused) {
  if (self->play_tone_msg != NULL) {
    ABORT(self->play_tone_msg);
    self->play_tone_msg = NULL;
  }

  DAC_REG = 0;
  self->playing = false;
  self->current_period = 0;

  #if LOG_LEVEL >= 2
  SCI_WRITE(&sci0, "\nINFO | Sound player reset to default state\n");
  #endif
}
