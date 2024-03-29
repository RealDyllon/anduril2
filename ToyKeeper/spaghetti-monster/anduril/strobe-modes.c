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

#include "strobe-modes.h"

#ifdef USE_STROBE_STATE
uint8_t strobe_state(Event event, uint16_t arg) {
    static int8_t ramp_direction = 1;

    // 'st' reduces ROM size slightly
    strobe_mode_te st = strobe_type;

    #ifdef USE_MOMENTARY_MODE
    momentary_mode = 1;  // 0 = ramping, 1 = strobes
    #endif

    #ifdef USE_CANDLE_MODE
    // pass all events to candle mode, when it's active
    // (the code is in its own pseudo-state to keep things cleaner)
    if (st == candle_mode_e) {
        candle_mode_state(event, arg);
    }
    #endif

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
        strobe_type = ((st + 1) % 4) + 0; // 0,1,2,3
        save_config();
        return MISCHIEF_MANAGED;
    }
    // 3 clicks - thin thick flashers
    else if (event == EV_3clicks) {
        // 50,50,150,50
        // 50,50,50,50,150,50
        // 50,50,50,50,50,50,150,50
        strobe_type = ((st + 1) % 3) + 4; // 4,5,6
        save_config();
        return MISCHIEF_MANAGED;
    }
    // 4 clicks: asceding on time
    else if (event == EV_4clicks) {
        // 50,50
        // 100, 50
        // 150, 50
        // 200, 50
        // 250, 50
        strobe_type = ((st + 1) % 5) + 7; // 7,8,9,10,11
        save_config();
        return MISCHIEF_MANAGED;
    }
    // hold: change speed (go faster)
    //       or change brightness (brighter)
    else if (event == EV_click1_hold) {
        if (0) {}  // placeholder

        // party / tactical strobe faster
        #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        #ifdef USE_TACTICAL_STROBE_MODE
        else if (st <= tactical_strobe_e) {
        #else
        else if (st == party_strobe_e) {
        #endif
            if ((arg & 1) == 0) {
                uint8_t d = strobe_delays[st];
                d -= ramp_direction;
                if (d < 8) d = 8;
                else if (d > 254) d = 254;
                strobe_delays[st] = d;
            }
        }
        #endif

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

        // party / tactical strobe slower
        #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        #ifdef USE_TACTICAL_STROBE_MODE
        else if (st <= tactical_strobe_e) {
        #else
        else if (st == party_strobe_e) {
        #endif
            if ((arg & 1) == 0) {
                if (strobe_delays[st] < 255) strobe_delays[st] ++;
            }
        }
        #endif

        // lightning has no adjustments
        //else if (st == lightning_storm_e) {}

        // TODO:
        // biking mode dimmer
        #ifdef USE_BIKE_FLASHER_MODE
        else if (st == bike_flasher_e) {
            if (bike_flasher_brightness > 2)
                bike_flasher_brightness --;
            set_level(bike_flasher_brightness);
        }
        #endif

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
    #if defined(USE_LIGHTNING_MODE) || defined(USE_CANDLE_MODE)
    // clock tick: bump the random seed
    else if (event == EV_tick) {
        // un-reverse after 1 second
        if (arg == AUTO_REVERSE_TIME) ramp_direction = 1;

        pseudo_rand_seed += arg;
        return MISCHIEF_MANAGED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}

// runs repeatedly in FSM loop() whenever UI is in strobe_state or momentary strobe
inline void strobe_state_iter() {
    uint8_t st = strobe_type;  // can't use switch() on an enum

    switch(st) {
        #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        #ifdef USE_PARTY_STROBE_MODE
        case party_strobe_e:
        #endif
        #ifdef USE_TACTICAL_STROBE_MODE
        case tactical_strobe_e:
        #endif
            party_tactical_strobe_mode_iter(st);
            break;
        #endif

        #ifdef USE_LIGHTNING_MODE
        case lightning_storm_e:
            lightning_storm_iter();
            break;
        #endif

        #ifdef USE_BIKE_FLASHER_MODE
        // custom flashers
        case bike_flasher_e:
            bike_flasher_iter();
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
        // thin thick flashers
        case thin_thick_flasher_1_e:
            thin_thick_flasher_1_iter();
            break;
        case thin_thick_flasher_2_e:
            thin_thick_flasher_2_iter();
            break;
        case thin_thick_flasher_3_e:
            thin_thick_flasher_3_iter();
            break;
        // ascending flashers
        case ascending_flasher_1_e:
            ascending_flasher_1_iter();
            break;
        case ascending_flasher_2_e:
            ascending_flasher_2_iter();
            break;
        case ascending_flasher_3_e:
            ascending_flasher_3_iter();
            break;
        case ascending_flasher_4_e:
            ascending_flasher_4_iter();
            break;
        case ascending_flasher_5_e:
            ascending_flasher_5_iter();
            break;
        #endif

        // todo: add more custom flashing modes here
    }
}
#endif  // ifdef USE_STROBE_STATE

#if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
inline void party_tactical_strobe_mode_iter(uint8_t st) {
    // one iteration of main loop()
    uint8_t del = strobe_delays[st];
    // TODO: make tac strobe brightness configurable?
    set_level(STROBE_BRIGHTNESS);
    if (0) {}  // placeholde0
    #ifdef USE_PARTY_STROBE_MODE
    else if (st == party_strobe_e) {  // party strobe
        #ifdef PARTY_STROBE_ONTIME
        nice_delay_ms(PARTY_STROBE_ONTIME);
        #else
        if (del < 42) delay_zero();
        else nice_delay_ms(1);
        #endif
    }
    #endif
    #ifdef USE_TACTICAL_STROBE_MODE
    else {  //tactical strobe
        nice_delay_ms(del >> 1);
    }
    #endif
    set_level(STROBE_OFF_LEVEL);
    nice_delay_ms(del);  // no return check necessary on final delay
}
#endif

#ifdef USE_LIGHTNING_MODE
inline void lightning_storm_iter() {
    // one iteration of main loop()
    int16_t brightness;
    uint16_t rand_time;

    // turn the emitter on at a random level,
    // for a random amount of time between 1ms and 32ms
    //rand_time = 1 << (pseudo_rand() % 7);
    rand_time = pseudo_rand() & 63;
    brightness = 1 << (pseudo_rand() % 7);  // 1, 2, 4, 8, 16, 32, 64
    brightness += 1 << (pseudo_rand() % 5);  // 2 to 80 now
    brightness += pseudo_rand() % brightness;  // 2 to 159 now (w/ low bias)
    if (brightness > MAX_LEVEL) brightness = MAX_LEVEL;
    set_level(brightness);
    nice_delay_ms(rand_time);

    // decrease the brightness somewhat more gradually, like lightning
    uint8_t stepdown = brightness >> 3;
    if (stepdown < 1) stepdown = 1;
    while(brightness > 1) {
        nice_delay_ms(rand_time);
        brightness -= stepdown;
        if (brightness < 0) brightness = 0;
        set_level(brightness);
        /*
           if ((brightness < MAX_LEVEL/2) && (! (pseudo_rand() & 15))) {
           brightness <<= 1;
           set_level(brightness);
           }
           */
        if (! (pseudo_rand() & 3)) {
            nice_delay_ms(rand_time);
            set_level(brightness>>1);
        }
    }

    // turn the emitter off,
    // for a random amount of time between 1ms and 8192ms
    // (with a low bias)
    rand_time = 1 << (pseudo_rand() % 13);
    rand_time += pseudo_rand() % rand_time;
    set_level(0);
    nice_delay_ms(rand_time);  // no return check necessary on final delay
}
#endif

#ifdef USE_BIKE_FLASHER_MODE
inline void bike_flasher_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
//    for(uint8_t i=0; i<5; i++) {
//        set_level(burst); //  off (burst)
//        nice_delay_ms();
//        set_level(bike_flasher_brightness);
//        nice_delay_ms(160);
//    }
    set_level(bike_flasher_brightness);
    nice_delay_ms(800);
    set_level(0); // TODO: should be off,
    nice_delay_ms(200);
}
#endif

// todo: define other strobe modes' iters here
inline void customer_flasher_1_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(1200); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(800); // times are off - should be 2000ms
}

inline void customer_flasher_2_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(250); // times are off - should be 2500ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(250);// times are off - should be 2500ms
}

inline void customer_flasher_3_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(100);
    set_level(0); // TODO: should be off,
    nice_delay_ms(100);
}

inline void thin_thick_flasher_1_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;

    set_level(bike_flasher_brightness); // on
    nice_delay_ms(50);
    set_level(0); // off
    nice_delay_ms(50);

    set_level(bike_flasher_brightness); // on
    nice_delay_ms(150);
    set_level(0); // off
    nice_delay_ms(50);
}

inline void thin_thick_flasher_2_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;

    for (uint8_t i=0; i<2; i++) { // 2 times
        set_level(bike_flasher_brightness); // on
        nice_delay_ms(50);
        set_level(0); // off
        nice_delay_ms(50);
    }

    set_level(bike_flasher_brightness); // on
    nice_delay_ms(150);
    set_level(0); // off
    nice_delay_ms(50);
}

inline void thin_thick_flasher_3_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;

    for (uint8_t i=0; i<3; i++) { // 3 times
        set_level(bike_flasher_brightness); // on
        nice_delay_ms(50);
        set_level(0); // off
        nice_delay_ms(50);
    }

    set_level(bike_flasher_brightness); // on
    nice_delay_ms(150);
    set_level(0); // off
    nice_delay_ms(50);
}

inline void ascending_flasher_1_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(50);
    set_level(0);
    nice_delay_ms(50);
}

inline void ascending_flasher_2_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(100);
    set_level(0);
    nice_delay_ms(50);
}

inline void ascending_flasher_3_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(150);
    set_level(0);
    nice_delay_ms(50);
}

inline void ascending_flasher_4_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(200);
    set_level(0);
    nice_delay_ms(50);
}

inline void ascending_flasher_5_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(250);
    set_level(0);
    nice_delay_ms(50);
}


#ifdef USE_CANDLE_MODE
#include "candle-mode.c"
#endif


#ifdef USE_BORING_STROBE_STATE
#include "ff-strobe-modes.c"
#endif


#endif

