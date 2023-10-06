/*
 * strobe-modes.c: Strobe modes for Anduril.
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

#ifndef STROBE_MODES_C
#define STROBE_MODES_C

#include "strobe-modes-a.h"

#ifdef USE_STROBE_STATE
uint8_t strobe_state_a(Event event, uint16_t arg) {
    static int8_t ramp_direction = 1;

    // 'st' reduces ROM size slightly
    strobe_mode_te st = strobe_type;

    if (0) {}  // placeholder
    // init anything which needs to be initialized
    else if (event == EV_enter_state) {
        ramp_direction = 1;
        return MISCHIEF_MANAGED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: rotate through strobe/flasher modes
    else if (event == EV_2clicks) {
        // 0 - candle
        // 1 to 4 - ascending strobe
        // 5 to 8 - thin thin thick
        strobe_type = (st + 1) % NUM_STROBES;
//        strobe_type = (st + 1) % 4 + 1; // 1 to 4
        save_config();
        return MISCHIEF_MANAGED;
    }
    // 3 clicks: switch to strobe_b_mode
    else if (event == EV_3clicks) {
        set_state(strobe_state_b, 0);
        set_level(0);
        return MISCHIEF_MANAGED;
    }
    // 4 clicks: rotate backward through strobe/flasher modes
    else if (event == EV_4clicks) {
        set_state(strobe_state_classic, 0);
        set_level(0);
        return MISCHIEF_MANAGED;
    }
    // hold: change speed (go faster)
    //       or change brightness (brighter)
    else if (event == EV_click1_hold) {
        if (0) {}  // placeholder


        // lightning has no adjustments
        //else if (st == lightning_storm_e) {}

        // TODO:
        // biking mode brighter
        #ifdef USE_BIKE_FLASHER_MODE
//        else if (st == bike_flasher_e) {
            bike_flasher_brightness += ramp_direction;
            if (bike_flasher_brightness < 2) bike_flasher_brightness = 2;
            else if (bike_flasher_brightness > MAX_BIKING_LEVEL) bike_flasher_brightness = MAX_BIKING_LEVEL;
            set_level(bike_flasher_brightness);
//        }
        #endif

        return MISCHIEF_MANAGED;
    }
    // reverse ramp direction on hold release
    // ... and save new strobe settings
    else if (event == EV_click1_hold_release) {
        ramp_direction = -ramp_direction;
        save_config();
        return MISCHIEF_MANAGED;
    }
    // click, hold: change speed (go slower)
    //       or change brightness (dimmer)
    else if (event == EV_click2_hold) {
        ramp_direction = 1;

        if (0) {}  // placeholder

        return MISCHIEF_MANAGED;
    }
    // release hold: save new strobe settings
    else if (event == EV_click2_hold_release) {
        save_config();
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_MOMENTARY_MODE
    // 5 clicks: go to momentary mode (momentary strobe)
    else if (event == EV_5clicks) {
        set_state(momentary_state, 0);
        set_level(0);
        return MISCHIEF_MANAGED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}

// runs repeatedly in FSM loop() whenever UI is in strobe_state or momentary strobe
inline void strobe_state_iter() {
    uint8_t st = strobe_type;  // can't use switch() on an enum

    switch(st) {
        // 50,50
        // 100, 50
        // 150, 50
        // 200, 50
        case custom_flasher_0_e:
            customer_flasher_0_iter();
            break;
        case custom_flasher_1_e:
            customer_flasher_1_iter();
            break;
        case custom_flasher_2_e:
            customer_flasher_2_iter();
            break;
        case custom_flasher_3_e:
            customer_flasher_3_iter();
            break;
    }
}
#endif  // ifdef USE_STROBE_STATE


inline void customer_flasher_0_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD * 1); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms
}

inline void customer_flasher_1_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD * 2); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms
}

inline void customer_flasher_2_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD * 3); // times are off - should be 2500ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD);// times are off - should be 2500ms
}

inline void customer_flasher_3_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD * 4);
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD);
}

inline void customer_flasher_4_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    // random duration
    uint8_t random_duration_factor = pseudo_rand() % 4 + 1; // range 1 to 4
    nice_delay_ms(MINIMUM_PERIOD * random_duration_factor);
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD);
}


#endif

