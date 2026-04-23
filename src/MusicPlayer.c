#include "MusicPlayer.h"
#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include "Definitions.h"
#include "Conductor.h"
#include "App.h"
#include "Database.h"
#include <stdlib.h>
#include <math.h>

extern App app;
extern SoundPlayer soundPlayer;
extern Serial sci0;
extern Database database;

// HELPER FUNCTIONS

/* Calculate the period for the specified frequeny index */
int calc_period(int frequency_index)
{
  double frequency = 440.0 * pow(2.0, (double)frequency_index / 12.0);
  double period = 500000.0 / frequency;
  return (int)period;
}

// Getters
int get_tempo(MusicPlayer *self, int unused)
{
  return self->tempo;
}

int get_key(MusicPlayer *self, int unused)
{
  return self->key;
}

int get_index(MusicPlayer *self, int unused)
{
  return self->index;
}


void set_tempo(MusicPlayer *self, int tempo)
{
  // Clamp the tempo
  if (tempo < 60)
  {
    tempo = 60;
  }
  else if (tempo > 240)
  {
    tempo = 240;
  }

  self->tempo = tempo;
}

void set_key(MusicPlayer *self, int key)
{
  // Clamp the key
  if (key < -5)
  {
    key = -5;
  }
  else if (key > 5)
  {
    key = 5;
  }

  self->key = key;
}

void set_index(MusicPlayer *self, int index)
{
  // Clamp the key
  if (index < 0)
  {
    index = 0;
  }
  if (index > NOTES_AMOUNT - 1)
  {
    index = NOTES_AMOUNT - 1;
  }

  self->index = index;
}


/* Plays the tone */
void play_music(MusicPlayer *self, int unused)
{
  if (!self->playing) {
    self->playing = true;

    // Call the next_note function
    self->next_note_msg = ASYNC(self, next_note, 0);
  }
}

// Stops the music completely, next time you play it starts from the start.
void stop_music(MusicPlayer *self, int unused)
{
  pause_music(self, unused);
  set_index(self, 0);
}

// Pauses the music, it continues from where it's left off
void pause_music(MusicPlayer *self, int unused)
{
  if (self->playing) {
    abort_msg(self, 0);

    self->playing = false;
    SYNC(&soundPlayer, soundplayer_set_playing, false);
  }
}

// Toggle music between play / pause
void toggle_music(MusicPlayer *self, int unused)
{
  int playing = self->playing;

  if (playing)
  {
    pause_music(self, unused);
  }
  else
  {
    play_music(self, unused);
  }
}

void abort_msg(MusicPlayer *self, int unused)
{
  ABORT(self->next_note_msg);
  ABORT(self->finish_note_msg);

  self->next_note_msg = NULL;
  self->finish_note_msg = NULL;
}

void play_note(MusicPlayer *self, int index)
{
  int period = calc_period(self->notes[index] + self->key);
  SYNC(&soundPlayer, soundplayer_set_playing, true);
  ASYNC(&soundPlayer, soundplayer_set_period, period);

  // Schedule the play_tone function to run.
  BEFORE(MAX_LAG_DEADLINE, &soundPlayer, soundplayer_play_tone, period);

  // Get the beat time, convert from beats per minute, to how many milliseconds for one beat.
  // In the beats list we count a half beat as 1x, one beat as 2x and two beats as 4x.
  // 30000 = (60/2 (because of the above reason)) * 1000 (for milliseconds)
  int mspb = self->beats[index] * (30000.0 / self->tempo) - 50;
  // bpm = beats/minute
  // x = millisekunder/beat
  // 60/bpm = (60/2*minute)/beat * 1000

  self->finish_note_msg = SEND(MSEC(mspb), MAX_LAG_DEADLINE, self, stop_note, 0);
}

void stop_note(MusicPlayer *self, int unused)
{
  SYNC(&soundPlayer, soundplayer_set_playing, false);
  SCI_WRITE(&sci0, "Finished playing note\n");
}

void next_note(MusicPlayer *self, int unused)
{
  // Change the current note:
  // - Get the period of the node with the index "index"
  // - Set the period on the sound player
  // - Set the current index and do mod on it in case it gets to the end

  // Check if we are just musician or conductor as well.
  // Check if we are supposed to be playing this.

  int mode = SYNC(&app, get_mode, 0);
  int active_boards = SYNC(&database, get_active_boards, 0);


  int should_we_play = (self->index + GROUP_13) % active_boards == 0;

  // Check if we SHOULD play the note.
  if (should_we_play)
  {
    int period = calc_period(self->notes[self->index] + self->key);
    ASYNC(&soundPlayer, soundplayer_set_period, period);

    /* Activate the musicPlayer */
    SYNC(&soundPlayer, soundplayer_set_playing, true);
    /* Play the tone */
    BEFORE(MAX_LAG_DEADLINE, &soundPlayer, soundplayer_play_tone, 100);
  }

  // Get the beat time, convert from beats per minute, to how many milliseconds for one beat.
  // In the beats list we count a half beat as 1x, one beat as 2x and two beats as 4x.
  // 30000 = (60/2 (because of the above reason)) * 1000 (for milliseconds)
  int mspb = self->beats[self->index] * (30000.0 / self->tempo) - 50;
  // bpm = beats/minute
  // x = millisekunder/beat
  // 60/bpm = (60/2*minute)/beat * 1000

  // Schedule the finish_note function
  self->finish_note_msg = SEND(MSEC(mspb), MAX_LAG_DEADLINE, self, finish_note, 0);

  self->index = (self->index + 1) % NOTES_AMOUNT;

  // If we are the conductor, we should also send a message to the musicians with which current node we are playing.
  if (mode == CONDUCTOR_MODE) {
    conductor_send_note_to_play(self->index);
  }
}

void finish_note(MusicPlayer *self, int unused)
{
  SYNC(&soundPlayer, soundplayer_set_playing, false);

  // Schedule the next_note function
  // Run next_note after 50 ms and make it so that it should have finished putting all the settings etc after 100
  self->next_note_msg = SEND(MSEC(50), MAX_LAG_DEADLINE, self, next_note, 0);
}
