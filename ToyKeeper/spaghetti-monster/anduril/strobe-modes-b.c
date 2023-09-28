//
// Created by Dyllon on 28/9/23.
//

#ifndef STROBE_MODES_B_C
#define STROBE_MODES_B_C

#include "strobe-modes-b.h"

#ifdef USE_STROBE_STATE
uint8_t strobe_state_b(Event event, uint16_t arg) {
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
    else if (event == EV_3clicks) {
        // 0 - candle
        // 1 to 4 - ascending strobe
        // 5 to 8 - thin thin thick
        strobe_type = (st + 1) % NUM_STROBES;
//        strobe_type = (st + 1) % 4 + 1; // 1 to 4
        save_config();
        return MISCHIEF_MANAGED;
    }
    // 3 clicks
    // todo: switch to strobe_mode
//    else if (event == EV_2clicks) {
//        // 0 - candle
//        // 1 to 4 - ascending strobe
//        // 5 to 8 - thin thin thick
//        strobe_type = (st + 1) % 4 + 4; // 5 to 8
//        save_config();
//        return MISCHIEF_MANAGED;
//    }
    // 4 clicks: rotate backward through strobe/flasher modes
//    else if (event == EV_4clicks) {
//        strobe_type = (st - 1 + NUM_STROBES) % NUM_STROBES;
//        save_config();
//        return MISCHIEF_MANAGED;
//    }
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
inline void strobe_state_b_iter() {
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
        // 50,50
        // 100, 50
        // 150, 50
        // 200, 50
//        case bike_flasher_e:
//            bike_flasher_iter();
//            break;
//        case custom_flasher_1_e:
//            customer_flasher_1_iter();
//            break;
//        case custom_flasher_2_e:
//            customer_flasher_2_iter();
//            break;
//        case custom_flasher_3_e:
//            customer_flasher_3_iter();
//            break;
        // 50,50,150,50
        // 50,50,50,50,150,50
        // 50,50,50,50,50,50,150,50
        // 50,50,50,50,50,50,50,50,150,50
        case thin_thin_thick_1_e:
            thin_thin_thick_1_iter();
            break;
        case thin_thin_thick_2_e:
            thin_thin_thick_2_iter();
            break;
        case thin_thin_thick_3_e:
            thin_thin_thick_3_iter();
            break;
        case thin_thin_thick_4_e:
            thin_thin_thick_4_iter();
            break;
        // todo
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

//#ifdef USE_BIKE_FLASHER_MODE
//inline void bike_flasher_iter() {
//    // one iteration of main loop()
//    uint8_t burst = bike_flasher_brightness << 1;
//    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
//    set_level(bike_flasher_brightness);
//    nice_delay_ms(MINIMUM_PERIOD * 1); // times are off - should be 8000ms
//    set_level(0); // TODO: should be off,
//    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms
//}
//#endif
//
//// todo: define other strobe modes' iters here
//inline void customer_flasher_1_iter() {
//    // one iteration of main loop()
//    uint8_t burst = bike_flasher_brightness << 1;
//    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
//    set_level(bike_flasher_brightness);
//    nice_delay_ms(MINIMUM_PERIOD * 2); // times are off - should be 8000ms
//    set_level(0); // TODO: should be off,
//    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms
//}
//
//inline void customer_flasher_2_iter() {
//    // one iteration of main loop()
//    uint8_t burst = bike_flasher_brightness << 1;
//    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
//    set_level(bike_flasher_brightness);
//    nice_delay_ms(MINIMUM_PERIOD * 3); // times are off - should be 2500ms
//    set_level(0); // TODO: should be off,
//    nice_delay_ms(MINIMUM_PERIOD);// times are off - should be 2500ms
//}
//
//inline void customer_flasher_3_iter() {
//    // one iteration of main loop()
//    uint8_t burst = bike_flasher_brightness << 1;
//    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
//    set_level(bike_flasher_brightness);
//    nice_delay_ms(MINIMUM_PERIOD * 4);
//    set_level(0); // TODO: should be off,
//    nice_delay_ms(MINIMUM_PERIOD);
//}

inline void thin_thin_thick_1_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD * 3); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms
}

inline void thin_thin_thick_2_iter() {
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD * 3); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms
}

inline void thin_thin_thick_3_iter() {
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD * 3); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms
}

inline void thin_thin_thick_4_iter() {
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD * 3); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms
}

#ifdef USE_CANDLE_MODE
#include "candle-mode.c"
#endif


#ifdef USE_BORING_STROBE_STATE
#include "ff-strobe-modes.c"
#endif


#endif
