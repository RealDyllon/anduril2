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
    strobe_mode_b_te st = strobe_type_b;

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
    // 2 clicks: switch to strobe_b_mode
    else if (event == EV_2clicks) {
        set_state(strobe_state_a, 0);
        set_level(0);
        return MISCHIEF_MANAGED;
    }
    // 3 clicks: rotate through strobe/flasher modes
    else if (event == EV_3clicks) {
        // 0 - candle
        // 1 to 4 - ascending strobe
        // 5 to 8 - thin thin thick
        strobe_type_b = (st + 1) % NUM_STROBES;
//        strobe_type_b = (st + 1) % 4 + 1; // 1 to 4
        save_config();
        return MISCHIEF_MANAGED;
    }
    // 4 clicks: rotate backward through strobe/flasher modes
    else if (event == EV_4clicks) {
        strobe_type_b = (st - 1 + NUM_STROBES) % NUM_STROBES;
        save_config();
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

        // lightning has no adjustments
        //else if (st == lightning_storm_e) {}

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
    uint8_t st = strobe_type_b;  // can't use switch() on an enum

    switch(st) {
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
        case thin_thin_thick_5_e:
            thin_thin_thick_5_iter();
            break;

        // todo: add more custom flashing modes here
    }
}
#endif  // ifdef USE_STROBE_STATE



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

inline void thin_thin_thick_5_iter() {
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
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms

    set_level(bike_flasher_brightness);
    nice_delay_ms(MINIMUM_PERIOD * 3); // times are off - should be 8000ms
    set_level(0); // TODO: should be off,
    nice_delay_ms(MINIMUM_PERIOD); // times are off - should be 2000ms
}

#endif
