# Copyright 2026 Ryan Cummings
# SPDX-License-Identifier: GPL-2.0-or-later

# The PowerMic-compatible button interface: QMK's joystick on its own interface and
# endpoint, with the descriptor from powermic.h (needs the patch in patches/).
JOYSTICK_ENABLE = yes
JOYSTICK_DRIVER = digital
JOYSTICK_OWN_EP = yes

# PowerScribe 360 opened every HID collection with the PowerMic's vendor ID and took
# each one for a PowerMic. With a raw HID and a media-key collection beside the
# PowerMic one, it showed the device but ignored its buttons; with only the boot
# keyboard and the PowerMic collection, the buttons worked. So the board has no other
# HID collection: no raw HID, media keys, NKRO, mouse keys or debug console. Windows
# keeps the boot keyboard to itself, so that one is safe to keep.
RAW_ENABLE = no
EXTRAKEY_ENABLE = no
NKRO_ENABLE = no
MOUSEKEY_ENABLE = no
CONSOLE_ENABLE = no
COMMAND_ENABLE = no

# Planck rev6 extras this example does not use. QMK's DAC audio driver froze a Planck
# rev6 now and then in all-day use (and never with audio compiled out), so it stays off.
AUDIO_ENABLE = no
MUSIC_ENABLE = no
ENCODER_ENABLE = no
DIP_SWITCH_ENABLE = no

# The record light.
RGBLIGHT_ENABLE = yes
