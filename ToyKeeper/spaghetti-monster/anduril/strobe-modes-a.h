/*
 * strobe-modes.h: Strobe modes for Anduril.
 *
 * Copyright (C) 2017 Selene Scriven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STROBE_MODES_H
#define STROBE_MODES_H

// internal numbering for strobe modes
#ifdef USE_STROBE_STATE
typedef enum {
    custom_flasher_0_e,
    custom_flasher_1_e,
    custom_flasher_2_e,
    custom_flasher_3_e,
    custom_flasher_4_e,
    strobe_mode_a_END
} strobe_mode_te;

const int NUM_STROBES = strobe_mode_a_END;

// which strobe mode is active?
#ifdef USE_CANDLE_MODE
strobe_mode_te strobe_type = candle_mode_e;
#else
strobe_mode_te strobe_type = 0;
#endif
#endif


// full FET strobe can be a bit much...  use max regulated level instead,
// if there's a bright enough regulated level
#ifndef STROBE_BRIGHTNESS
#ifdef MAX_Nx7135
#define STROBE_BRIGHTNESS MAX_Nx7135
#else
#define STROBE_BRIGHTNESS MAX_LEVEL
#endif
#endif

// some drivers need to keep the regulator chip on between pulses,
// so set this to 1 to keep the light on at moon mode between pulses,
// and thus keep the regulator powered up for the next flash
#ifndef STROBE_OFF_LEVEL
#define STROBE_OFF_LEVEL 0
#endif

// party and tactical strobes
#ifdef USE_STROBE_STATE
uint8_t strobe_state_a(Event event, uint16_t arg);
inline void strobe_state_iter();
#endif

#if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
// party / tactical strobe timing
uint8_t strobe_delays[] = { 41, 67 };  // party strobe 24 Hz, tactical strobe 10 Hz
inline void party_tactical_strobe_mode_iter(uint8_t st);
#endif

#ifdef USE_LIGHTNING_MODE
inline void lightning_storm_iter();
#endif

// bike mode config options
#ifdef USE_BIKE_FLASHER_MODE
#define MAX_BIKING_LEVEL 120  // should be 127 or less
#define MINIMUM_PERIOD 100
uint8_t bike_flasher_brightness = MAX_1x7135;
inline void customer_flasher_0_iter();
inline void customer_flasher_1_iter();
inline void customer_flasher_2_iter();
inline void customer_flasher_3_iter();
inline void customer_flasher_4_iter();
//inline void thin_thin_thick_1_iter();
//inline void thin_thin_thick_2_iter();
//inline void thin_thin_thick_3_iter();
//inline void thin_thin_thick_4_iter();

#endif

#ifdef USE_CANDLE_MODE
#include "candle-mode.h"
#endif


#if defined(USE_POLICE_STROBE_MODE) || defined(USE_SOS_MODE_IN_FF_GROUP)
#define USE_BORING_STROBE_STATE
#include "ff-strobe-modes.h"
#endif

#endif
