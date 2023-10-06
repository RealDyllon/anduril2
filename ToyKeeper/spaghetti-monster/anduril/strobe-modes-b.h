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

#ifndef STROBE_MODES_B_H
#define STROBE_MODES_B_H

// internal numbering for strobe modes
#ifdef USE_STROBE_STATE
typedef enum {
    thin_thin_thick_1_e,
    thin_thin_thick_2_e,
    thin_thin_thick_3_e,
    thin_thin_thick_4_e,
    thin_thin_thick_5_e,
    strobe_mode_b_END
} strobe_mode_b_te;

const int NUM_STROBES_B = strobe_mode_b_END;

// which strobe mode is active?
strobe_mode_b_te strobe_type_b = 0;
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

#define MINIMUM_PERIOD 40

// party and tactical strobes
#ifdef USE_STROBE_STATE
uint8_t strobe_state_b(Event event, uint16_t arg);
inline void strobe_state_b_iter();
#endif

inline void thin_thin_thick_1_iter();
inline void thin_thin_thick_2_iter();
inline void thin_thin_thick_3_iter();
inline void thin_thin_thick_4_iter();
inline void thin_thin_thick_5_iter();


#endif
