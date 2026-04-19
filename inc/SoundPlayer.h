#ifndef _SoundPlayer_H
#define _SoundPlayer_H

#include "TinyTimber.h"
#include <stdbool.h>

#define DAC_REG (*(volatile uint8_t *)0x4000741C)

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

/// @brief Starts playing the tone
/// @param SoundPlayer The SoundPlayer object
/// @param int A boolean value to start or stop the tone.
void play_tone(SoundPlayer *, int);


/// @brief Sets the playing state of the SoundPlayer
/// @param SoundPlayer The SoundPlayer object
/// @param int A boolean value to set the playing state.
void set_playing(SoundPlayer *, int);

/// @brief Gets the playing state of the SoundPlayer
/// @param SoundPlayer The SoundPlayer object
/// @param int Unused parameter
/// @return The playing state of the SoundPlayer
int get_playing(SoundPlayer *, int);


/// @brief Sets the period of the current tone
/// @param SoundPlayer The SoundPlayer object
/// @param int The period of the current tone
void set_period(SoundPlayer *, int);

/// @brief Gets the period of the current tone
/// @param SoundPlayer The SoundPlayer object
/// @param int Unused parameter
/// @return The period of the current tone
int get_period(SoundPlayer *, int);


/// @brief Sets the volume of the SoundPlayer
/// @param SoundPlayer The SoundPlayer object
/// @param int The volume of the SoundPlayer
void set_volume(SoundPlayer *, int);

/// @brief Gets the volume of the SoundPlayer
/// @param SoundPlayer The SoundPlayer object
/// @param int Unused parameter
/// @return The volume of the SoundPlayer
int get_volume(SoundPlayer *, int);

#endif
