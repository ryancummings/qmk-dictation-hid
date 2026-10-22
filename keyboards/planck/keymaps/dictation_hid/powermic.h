// Copyright 2026 Ryan Cummings
// SPDX-License-Identifier: GPL-2.0-or-later
//
// USB identity and HID interface of a PowerMic II-compatible device, for a keymap's
// config.h. Include it last in config.h: it replaces the keyboard's own USB identity.
// Needs patches/0001-joystick-custom-hid-interface.patch applied to QMK, and in
// rules.mk: JOYSTICK_ENABLE = yes, JOYSTICK_DRIVER = digital, JOYSTICK_OWN_EP = yes.

#pragma once

// The real handset's USB identity (README, "Sources"). The real handset has no serial
// number; QMK on ChibiOS adds one from the chip's ID unless told not to.
#undef VENDOR_ID
#undef PRODUCT_ID
#undef DEVICE_VER
#undef MANUFACTURER
#undef PRODUCT
#define VENDOR_ID 0x0554
#define PRODUCT_ID 0x1001
#define DEVICE_VER 0x0204
#define MANUFACTURER "Dictaphone Corporation"
#define PRODUCT "PowerMicII-NS"
#undef SERIAL_NUMBER_USE_HARDWARE_ID
#define SERIAL_NUMBER_USE_HARDWARE_ID FALSE

// The handset's 64-byte report descriptor, byte for byte, on QMK's joystick interface.
// Its input report is 3 bytes: a constant byte, then HID buttons 1-14 and 2 padding
// bits. QMK's joystick report has the same layout with one 8-bit axis (always 0, the
// constant byte) and 14 buttons. The host sets an 8-bit light (output) report and a
// 39-byte vendor feature report with SET_REPORT; the handset has no OUT endpoint. Its
// IN endpoint is 4 bytes, polled every 10 ms.
#define JOYSTICK_AXIS_COUNT 1
#define JOYSTICK_AXIS_RESOLUTION 8
#define JOYSTICK_BUTTON_COUNT 14
#define JOYSTICK_EPSIZE 4
#define JOYSTICK_POLLING_INTERVAL 10
#define JOYSTICK_FEATURE_REPORT_SIZE 39
#define JOYSTICK_REPORT_DESCRIPTOR                                                     \
    0x05, 0x01, 0x09, 0x00, 0xA1, 0x01, 0x05, 0x08, 0x09, 0x4B, 0x15, 0x00, 0x25,     \
    0x01, 0x95, 0x08, 0x75, 0x01, 0x91, 0x02, 0x05, 0x09, 0x15, 0x00, 0x25, 0x01,     \
    0x75, 0x01, 0x95, 0x08, 0x81, 0x01, 0x19, 0x01, 0x29, 0x0E, 0x15, 0x00, 0x25,     \
    0x01, 0x95, 0x0E, 0x81, 0x02, 0x95, 0x02, 0x81, 0x01, 0x05, 0xFF, 0x09, 0x00,     \
    0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x27, 0xB1, 0x00, 0xC0

// Button keycodes. QMK joystick button n is HID button n + 1. The names follow
// Nuance's button names; what each button does is set in the dictation software.
#define PM_TRANS JS_0  // Transcribe       HID button 1   report 00 01 00
#define PM_TABB  JS_1  // Tab backward     HID button 2   report 00 02 00
#define PM_DICT  JS_2  // Dictate          HID button 3   report 00 04 00
#define PM_TABF  JS_3  // Tab forward      HID button 4   report 00 08 00
#define PM_REW   JS_4  // Rewind           HID button 5   report 00 10 00
#define PM_FFWD  JS_5  // Fast forward     HID button 6   report 00 20 00
#define PM_PLAY  JS_6  // Stop/Play        HID button 7   report 00 40 00
#define PM_CUSL  JS_7  // Custom left      HID button 8   report 00 80 00
#define PM_SEL   JS_8  // Enter/Select     HID button 9   report 00 00 01
#define PM_CUSR  JS_9  // Custom right     HID button 10  report 00 00 02

// Report types in SET_REPORT's wValue high byte, as joystick_receive_report gives them.
#define PM_REPORT_OUTPUT 2
#define PM_REPORT_FEATURE 3
// Bit 0 of the light report: on while the host is recording.
#define PM_LIGHT_RECORDING 0x01
