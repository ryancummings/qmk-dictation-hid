// Copyright 2026 Ryan Cummings
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Example: a Planck that is also a PowerMic II-compatible set of dictation buttons.
// The left block holds the ten PowerMic buttons, the right block a number pad. The
// buttons are HID buttons, not keys: the dictation software reads them and decides
// what each one does. The underglow shows the host's record light.

#include QMK_KEYBOARD_H
#include "usb_descriptor.h"

#ifndef QMK_JOYSTICK_CUSTOM
#    error "Apply patches/0001-joystick-custom-hid-interface.patch to QMK first (build.sh does this)"
#endif
#if defined(RAW_ENABLE) || defined(EXTRAKEY_ENABLE) || defined(NKRO_ENABLE) || defined(MOUSEKEY_ENABLE) || defined(CONSOLE_ENABLE)
#    error "Keep the PowerMic collection the only HID collection besides the boot keyboard (README, Gotchas)"
#endif

enum layers { _BASE, _FN };

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Base
 * ,-----------------------------------------------------------------------------------.
 * |Transc| Rew  | Play | FFwd |      |      |      |   7  |   8  |   9  |   -  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |TabBck| Dict |TabFwd|      |      |      |      |   4  |   5  |   6  |   +  | Del  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |CustL | Sel  |CustR |      |      |      |      |   1  |   2  |   3  |   .  | Enter|
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |  Fn  |      |      |      |      |    Space    |   0  |   ,  |      |      | Tab  |
 * `-----------------------------------------------------------------------------------'
 */
[_BASE] = LAYOUT_ortho_4x12(
    PM_TRANS, PM_REW,  PM_PLAY, PM_FFWD, KC_NO,   KC_NO,   KC_NO,   KC_7,    KC_8,    KC_9,    KC_MINS, KC_BSPC,
    PM_TABB,  PM_DICT, PM_TABF, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_4,    KC_5,    KC_6,    KC_PLUS, KC_DEL,
    PM_CUSL,  PM_SEL,  PM_CUSR, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_1,    KC_2,    KC_3,    KC_DOT,  KC_ENT,
    MO(_FN),  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_SPC,  KC_SPC,  KC_0,    KC_COMM, KC_NO,   KC_NO,   KC_TAB
),

/* Fn: Boot restarts the board into its bootloader for flashing.
 * ,-----------------------------------------------------------------------------------.
 * | Boot |      |      |      |      |      |      |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_FN] = LAYOUT_ortho_4x12(
    QK_BOOT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),
};
// clang-format on

static void set_record_light(bool on) {
#ifdef RGBLIGHT_ENABLE
    if (on) {
        rgblight_enable_noeeprom();
        rgblight_sethsv_noeeprom(HSV_RED);
    } else {
        rgblight_disable_noeeprom();
    }
#endif
}

void keyboard_post_init_user(void) {
    set_record_light(false);
}

// The host sends its reports with SET_REPORT, which QMK receives in the USB interrupt;
// read them here, in the main loop. PowerScribe 360 sets the light report to 01 while
// recording and 00 otherwise. It also sends one feature report starting 08, a probe
// for the handset's barcode scanner, which this board leaves unanswered like a handset
// without one. Your own software may use the feature report as a channel to the board.
void housekeeping_task_user(void) {
    uint8_t data[JOYSTICK_FEATURE_REPORT_SIZE], type;
    uint8_t length = joystick_receive_report(data, sizeof(data), &type);
    if (length && type == PM_REPORT_OUTPUT) {
        set_record_light(data[0] & PM_LIGHT_RECORDING);
    }
}
