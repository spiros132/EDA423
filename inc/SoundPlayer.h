#ifndef _SoundPlayer_H
#define _SoundPlayer_H

#include "TinyTimber.h"
#include <stdbool.h>

#define DAC_REG (*(volatile uint8_t *)0x4000741C)

typedef struct {
  Object super;

  /// @brief A boolean value indicating if the player is currently playing a tone.
  int playing;

  /// @brief The period of the current tone.
  int current_period;

  /// @brief The volume of the SoundPlayer.
  int volume;

  /// @brief Whether the player is muted or not.
  int muted;

  Msg play_tone_msg;
} SoundPlayer;

#define initSoundPlayer() \
{ \
  initObject(), \
  /* Playing */ \
  0, \
  /* Current Period */ \
  0, \
  /* Volume */ \
  5, \
  /* Muted */ \
  0, \
  /* play_tone_msg */ \
  NULL \
}

/// @brief Starts playing the tone
/// @param SoundPlayer The SoundPlayer object
/// @param int A boolean value to start or stop the tone.
void soundplayer_play_tone(SoundPlayer *, int);


/// @brief Sets the playing state of the SoundPlayer
/// @param SoundPlayer The SoundPlayer object
/// @param int A boolean value to set the playing state.
void soundplayer_set_playing(SoundPlayer *, int);

/// @brief Sets the period of the current tone
/// @param SoundPlayer The SoundPlayer object
/// @param int The period of the current tone
void soundplayer_set_period(SoundPlayer *, int);

/// @brief Sets the volume of the SoundPlayer
/// @param SoundPlayer The SoundPlayer object
/// @param int The volume of the SoundPlayer
void soundplayer_set_volume(SoundPlayer *, int);

/// @brief Sets the muted state of the SoundPlayer
/// @param SoundPlayer The SoundPlayer object
/// @param int A boolean value to set the muted state.
void soundplayer_set_muted(SoundPlayer *, int);


/// @brief Increases the volume of the sound player by 1.
/// @param SoundPlayer The SoundPlayer object.
/// @param int Unused parameter.
void soundplayer_increase_volume(SoundPlayer *, int);

/// @brief Decreases the volume of the sound player by 1.
/// @param SoundPlayer The SoundPlayer object.
/// @param int Unused parameter.
void soundplayer_decrease_volume(SoundPlayer *, int);


#endif
