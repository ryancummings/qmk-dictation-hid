# qmk-dictation-hid

PowerMic buttons on your own keyboard, for radiologists who dictate in PowerScribe 360.

This project is for radiologists who use Nuance PowerScribe 360. It puts the buttons of a
PowerMic or a Philips SpeechMike on a keyboard or macro pad that runs QMK firmware.
Examples of these buttons are Dictate, Next field, Previous field, Enter and Backspace.

The board tells the computer that it is a Nuance PowerMic II. PowerScribe 360 supports
the PowerMic II without additional software. Thus PowerScribe reads the buttons of the
board, and the workstation needs no driver, add-on or macro software. You set the function
of each button in the PowerMic configuration of PowerScribe, as for a real PowerMic. You
continue to dictate into your usual microphone.

## Who this project is for

This project is for you if these statements are true:

- You dictate reports in PowerScribe 360 on Windows.
- You want the PowerMic buttons on keys, for example under the hand that is not on the
  mouse or trackball.
- You can build and flash QMK firmware, or you want to learn how.

If you use a PowerMic or a SpeechMike now, the board does the buttons and your microphone
does the audio. Without the Philips add-on, a SpeechMike still works as a plain USB
microphone. In testing, PowerScribe recorded from a wireless lapel microphone while the
board supplied the buttons.

The example keymap is for a Planck rev6. Other ARM keyboards that use ChibiOS need small
changes (see [Use a different keyboard](#use-a-different-keyboard)).

## SpeechMike users

The board copies a PowerMic II only. PowerScribe 360 reads SpeechMike buttons only through
an add-on from Philips, so a copy of a SpeechMike also needs that add-on (see
[problem 12](#12-copy-the-powermic-not-the-speechmike)). If you use a SpeechMike, keep it
as your microphone and put the PowerMic buttons on the board.

## Other dictation software

The author did not test PowerScribe One, Dragon or other dictation software. Other
programs that read a PowerMic II possibly work, but no test shows this. If you test one,
open an issue with the result.

## Before you use the board

> [!WARNING]
> Speak to your IT department before you connect the board to a clinical or managed
> workstation. The board uses a USB vendor ID that belongs to a different company.
> Endpoint security, USB device control and asset tools can flag a device with an unknown
> or copied vendor ID. Many hospitals do not permit USB devices that IT did not approve.

> [!IMPORTANT]
> Do not use the board for clinical work until you make sure that each button does what
> you expect (see [problem 11](#11-make-sure-that-each-button-does-what-you-expect)). The
> board is not a medical device. It is a hobby project, provided as is, without warranty of
> any kind (see [LICENSE](LICENSE)). Nobody tested, certified or cleared it for clinical use.

> [!NOTE]
> Nuance Communications, Microsoft, Philips and Speech Processing Solutions are not
> affiliated with this project. They do not endorse or support it. PowerMic, PowerScribe,
> Dragon and SpeechMike are trademarks of their owners. This document uses the names only
> to identify the products that the firmware is compatible with.

## Test results

The author tested the example in October 2026 on a Planck rev6 (Drop edition, STM32F303)
with PowerScribe 360 on Windows. The results were:

- PowerScribe 360 showed the board as a PowerMicII-NS.
- PowerScribe acted on all five buttons that had a function: Dictate, Tab forward, Tab
  backward, Enter/Select and Custom left.
- PowerScribe recorded from a different microphone.
- PowerScribe turned on the record light of the board.

The author did not test other keyboards, other dictation software, PowerScribe One, Dragon
or macOS.

## Contents of the repository

The repository has three parts:

- A QMK patch. With the patch, a keymap can give the QMK joystick interface its own HID
  report descriptor, and receive the reports that the computer sends back. A report
  descriptor tells the computer the format of each report.
- `powermic.h`. It contains the USB identity, the report descriptor and the button
  keycodes of the PowerMic II. Any keymap can include it from its `config.h`.
- An example keymap for the Planck rev6. It puts the ten PowerMic buttons on the left and
  a number pad on the right.

```
patches/0001-joystick-custom-hid-interface.patch   QMK patch (ChibiOS boards)
keyboards/planck/keymaps/dictation_hid/
    powermic.h    USB identity, report descriptor, button keycodes
    config.h      includes powermic.h
    rules.mk      joystick on its own endpoint; every other HID collection off
    keymap.c      example layout and record light
qmk.json          QMK userspace file, so `qmk compile` finds the keymap here
build.sh          applies the patch, then builds or flashes
```

## Quick start

You need a working QMK installation. Run `qmk setup`, then `qmk doctor`. The author made
the patch against QMK `master` at commit `7a1bbf37` (2026-10-02). If `git apply` fails on a
later QMK version, you must update the patch.

1. Clone the repository and go into its folder:

   ```sh
   git clone https://github.com/ryancummings/qmk-dictation-hid.git
   cd qmk-dictation-hid
   ```

2. Build the firmware:

   ```sh
   ./build.sh                 # builds planck/rev6_drop:dictation_hid
   ```

   The script applies the patch to the QMK folder that `qmk env` shows. If the patch is
   already applied, the script does not apply it again. Then the script runs
   `qmk compile` from this folder. QMK uses this folder as a userspace (a folder of
   keymaps outside the QMK folder).

3. Start the flash with `./build.sh flash`.

4. Put the board into its bootloader. Hold Fn (bottom left) and push the top-left key
   (`QK_BOOT`), or push the reset button on the back of the board. The bootloader of a
   Planck rev6 shows as the STM32 DFU device `0483:df11`.

5. Make sure that the computer finds the board. On Linux, `lsusb -d 0554:1001` shows the
   board. On Windows, Device Manager shows it under Human Interface Devices, with the
   standard Windows HID drivers.

6. In PowerScribe, select the PowerMic as the dictation device.

7. Select a different microphone as the recording input. The board has no audio.

8. Set the function of each button (see
   [problem 11](#11-make-sure-that-each-button-does-what-you-expect)).

To apply the patch yourself, run this command from the repository folder:

```sh
git -C "$(qmk env | sed -n 's/^QMK_HOME="\(.*\)"$/\1/p')" apply patches/0001-joystick-custom-hid-interface.patch
```

To remove the patch, run the same command with `apply -R`.

## Use a different keyboard

The patch works only on ARM boards that use ChibiOS. The patch handles `SET_REPORT` in the
USB code of ChibiOS. Thus AVR (LUFA) and V-USB boards are not supported.

To use a different ChibiOS keyboard:

1. Copy the `dictation_hid` folder into the `keymaps` folder of that keyboard, or into
   your userspace.
2. Change `keymap.c` to use the layout macro of that keyboard.
3. Keep `rules.mk` as it is.
4. If the `keyboard.json` of the keyboard turns on a feature that adds a HID collection,
   turn the feature off in `rules.mk`. A HID collection is a group of controls that a
   program can open (see [problem 1](#1-use-only-the-boot-keyboard-beside-the-powermic-collection)).

## How it works

QMK has a joystick feature, which is a HID interface that reports buttons and axes. The
patch lets a keymap make three changes to the joystick:

- Replace the joystick report descriptor with the 64-byte descriptor of the PowerMic II.
- Keep the joystick on its own USB interface and endpoint. An endpoint is a data channel
  of a USB interface.
- Receive the output and feature reports that the computer sends with `SET_REPORT`.

`powermic.h` sets the USB identity. It also sets the size of the joystick report. The
report is then the same as the 3-byte button report of the PowerMic, byte for byte. The
keymap puts `JS_0` to `JS_9` on keys, with the names `PM_DICT`, `PM_TABF` and so on.

The board shows on USB as two HID interfaces:

| Interface | Function | Endpoint |
|---|---|---|
| 0 | QMK boot keyboard (the number pad) | IN, 8 bytes, 1 ms |
| 1 | PowerMic II button interface | IN, 4 bytes, 10 ms |

A real PowerMic II has three audio interfaces first, and its HID interface is interface 3.
This board has no audio.

## Buttons and reports

The input report has 3 bytes. The first byte is always `00`. The second byte holds HID
buttons 1 to 8, with button 1 in bit 0. The third byte holds HID buttons 9 to 14. The board
sends a report each time a button changes, as the real handset does. QMK joystick button
*n* is HID button *n* + 1.

| Button | Keycode | QMK | HID button | Report while held |
|---|---|---|---|---|
| Transcribe | `PM_TRANS` | `JS_0` | 1 | `00 01 00` |
| Tab backward | `PM_TABB` | `JS_1` | 2 | `00 02 00` |
| Dictate | `PM_DICT` | `JS_2` | 3 | `00 04 00` |
| Tab forward | `PM_TABF` | `JS_3` | 4 | `00 08 00` |
| Rewind | `PM_REW` | `JS_4` | 5 | `00 10 00` |
| Fast forward | `PM_FFWD` | `JS_5` | 6 | `00 20 00` |
| Stop/Play | `PM_PLAY` | `JS_6` | 7 | `00 40 00` |
| Custom left | `PM_CUSL` | `JS_7` | 8 | `00 80 00` |
| Enter/Select | `PM_SEL` | `JS_8` | 9 | `00 00 01` |
| Custom right | `PM_CUSR` | `JS_9` | 10 | `00 00 02` |

If you hold two buttons, the report sets both bits. For example, Dictate and Enter/Select
together give `00 04 01`. When you release all buttons, the report is `00 00 00`.

The computer sends two reports to the board. It sends both with `SET_REPORT` on the
control endpoint, because the handset has no OUT endpoint.

| Report | Size | What PowerScribe 360 sent in testing |
|---|---|---|
| Output (type 2): the record light | 1 byte | `01` while it recorded, `00` at other times |
| Feature (type 3): vendor data | 39 bytes | One report that starts with `08`, a probe for the barcode scanner of the handset |

The example keymap turns the underglow red while bit 0 of the light report is set. It does
not answer the scanner probe, the same as a PowerMic without a scanner.

### Read the buttons from your own software

Your software does not need to know about QMK. Open the HID collection with usage page
`0x01` (Generic Desktop) and usage `0x00`, on vendor ID `0x0554` and product ID `0x1001`.
On Windows, each input report that you read has 4 bytes. The first byte is `00`, because
the device uses no report IDs. The next 3 bytes are the report above. For example,
"Dictate pressed" is `00 00 04 00`, and "released" is `00 00 00 00`.

To send your own data to the board, use the 39-byte feature report (`HidD_SetFeature` on
Windows). Start your messages with bytes that the dictation software does not use.
PowerScribe sent `08`. On the board, read the messages in `housekeeping_task_user` with
`joystick_receive_report()`. To answer a feature read, call `joystick_set_feature_reply()`.

## Problems found in testing

This section lists the problems that the author found while making the board work. Each
item says how the author found the problem.

### 1. Use only the boot keyboard beside the PowerMic collection

PowerScribe 360 opens every HID collection with vendor ID `0x0554`, and uses each one as a
PowerMic. In one test, the board also had the QMK raw HID collection and the media-key
collection. PowerScribe showed the device but ignored every button. In a second test, the
board had only the boot keyboard and the PowerMic collection, and the buttons worked.

Thus `rules.mk` turns off raw HID (`RAW_ENABLE`), media keys (`EXTRAKEY_ENABLE`), NKRO,
mouse keys and the debug console. `keymap.c` stops the build if one of them is on. The
author did not test mouse keys or the console alone. They add collections, so they are off
too.

The boot keyboard is safe, because Windows keeps keyboard collections for itself. Other
programs cannot open them. Many keyboards turn on some of these features in their
`keyboard.json`. The `rules.mk` of the keymap turns them off again.

### 2. Do not give the button interface a report ID

The button code of PowerScribe accepts only reads of 4 bytes. Each read is the report-ID
byte that Windows adds (`00` when a device has no report IDs), then the 3 bytes of the
report. When other features share an endpoint, QMK usually puts the joystick on that shared
endpoint, with a report ID. The patch adds `JOYSTICK_OWN_EP = yes`, which keeps the
joystick on its own interface and endpoint. If a custom descriptor is on the shared
endpoint, the patch stops the build with an error.

### 3. Make the joystick report match the descriptor, byte for byte

QMK sends its own joystick report structure, and the patch changes only the descriptor.
The first byte of the PowerMic report is constant padding, and QMK has no option for
padding. Thus `powermic.h` declares one 8-bit axis that is always 0
(`JOYSTICK_AXIS_COUNT 1`, `JOYSTICK_AXIS_RESOLUTION 8`). Then it declares 14 buttons, which
QMK packs into 2 bytes. If you change one of these values, the report does not match the
descriptor.

The endpoint has 4 bytes, and the computer polls it every 10 ms. These values are the same
as on a real PowerMic II, from its `lsusb -v` output.

### 4. Receive the reports on the control endpoint

The PowerMic has no OUT endpoint. Thus the computer sets the light and the feature report
with `SET_REPORT`. Stock QMK accepts `SET_REPORT` only on the keyboard interfaces, and the
patch adds the joystick interface. The USB callback runs in an interrupt, so it only stores
the report. Read the report from the main loop with `joystick_receive_report()`, as
`keymap.c` does.

### 5. Do not answer the barcode-scanner probe

When PowerScribe opened the device, it sent one 39-byte feature report that starts with
`08`. The board logged this report during testing. PowerScribe uses the feature report only
for the barcode scanner of the PowerMic II, so a board without a scanner can ignore it. A
feature read (`GET_REPORT`) returns the last feature report that the computer set, or zeros.
The author did not see the computer read it.

### 6. Read the record light from bit 0 of the output report

PowerScribe 360 set the light report to `01` while it recorded, and to `00` at other times.
The board logged these values during testing. The values also agree with the PowerMic III
light values in `dictation_support` from GoogleChromeLabs.

### 7. Do not send the trigger

On a real handset, the trigger shows in the first byte, and the descriptor declares that
byte constant. In the Nuance device model, the trigger is part of the barcode scanner, which
is a separate device. Thus the board does not send the trigger.

### 8. Use a separate microphone

A real PowerMic is also a USB microphone, but this board is not. In testing, PowerScribe 360
accepted a PowerMic with no audio interfaces. It recorded from a microphone that was
selected separately as the recording input. Select your microphone in the audio
configuration of the dictation software.

### 9. Do not report a serial number

The real PowerMic II reports no serial number. QMK on ChibiOS makes a serial number from the
unique ID of the chip, unless `SERIAL_NUMBER_USE_HARDWARE_ID` is `FALSE`. `powermic.h` sets
it to `FALSE`, which copies the real device. The author did not test whether PowerScribe uses
the serial number.

The vendor and product strings also come from the real handset ("Dictaphone Corporation",
"PowerMicII-NS"). The author tested only this full set of values.

### 10. Tell the board apart from a real PowerMic

A genuine PowerMic II has the same vendor ID and product ID as the board. Thus a tool or a
Linux udev rule that matches only these IDs also matches a real PowerMic. To tell them
apart, use the USB interface number. The PowerMic collection of the board is on interface
1, and the HID interface of a real PowerMic II is interface 3.

This udev rule lets a logged-in user open the board on Linux. It also matches a real
PowerMic.

```
SUBSYSTEM=="hidraw", ATTRS{idVendor}=="0554", ATTRS{idProduct}=="1001", TAG+="uaccess"
```

### 11. Make sure that each button does what you expect

The firmware only reports that a button, for example Custom left, is pressed. The dictation
software decides what the button does. In PowerScribe 360, you set the function under
Tools → Preferences → PowerMic. PowerScribe keeps this configuration for each user on each
PowerScribe system.

In testing, one system had Custom left set to Sign Report. Thus a key that was meant as
Backspace signed a report. Before you use the board on a system, open the PowerMic button
configuration on that system. Make sure that no button on the board signs, deletes or sends
anything that you do not intend.

### 12. Copy the PowerMic, not the SpeechMike

A QMK board can also copy the vendor HID interface of a SpeechMike. But PowerScribe 360 does
not read SpeechMike buttons itself. It reads them only through the Philips add-on,
`PS360SpeechMikeSupport.exe`. The add-on installs a Windows multimedia (MCI) driver that
reads the SpeechMike and sends each button to PowerScribe. Without the add-on, PowerScribe
finds the SpeechMike but ignores every button. A real SpeechMike Premium Touch gave the same
result.

PowerScribe 360 supports the PowerMic II itself. That is why this project copies the
PowerMic II.

### 13. Expect button states, not keystrokes

The PowerMic buttons send button states, not keystrokes. Thus there is no keyboard
auto-repeat while you hold a button, and the buttons type nothing in other programs. The
board sends one report each time a button changes. The normal QMK debounce (a filter for
switch contact bounce) applies. The author saw no debounce problems.

### 14. Keep QMK audio off on the Planck rev6

In all-day use, the QMK DAC audio driver sometimes froze a Planck rev6. The board then went
off USB until someone unplugged it and plugged it in again. This occurred at all volumes,
and never with audio removed from the build. Thus `rules.mk` turns audio off. This problem
is about the board, not about the PowerMic interface.

### 15. Use a key or the reset button to flash

With raw HID and the console off, no program on the computer can restart the board into
its bootloader. Use `QK_BOOT` instead. In the example, `QK_BOOT` is Fn plus the top-left
key. You can also push the reset button.

### Items without test results

The author has no test results for these items:

- Old descriptors that Windows keeps after a new flash with the same vendor ID and product
  ID. The author did not see this problem.
- Use of the board next to a genuine PowerMic.
- Emulation of a PowerMic III.
- PowerScribe One, Dragon and macOS.

## Sources

- PowerMic II report descriptor and captures of each button: a USBPcap capture of a
  genuine PowerMic II, documented in
  [whyaaronbailey/adafruit_rp2040_usbh](https://github.com/whyaaronbailey/adafruit_rp2040_usbh)
  (`keymaps/tartarus2_qmk/powermic.h`). The descriptor in `powermic.h` is a byte copy.
- USB identity (IDs, `bcdDevice` 2.04, strings, no serial number, HID on interface 3,
  4-byte endpoint at 10 ms): the `lsusb -v` output of a real PowerMic II in
  [linuxhw/LsUSB](https://github.com/linuxhw/LsUSB).
- Button names and order: the `PMII_MICBUTTONS` table in the Nuance Microphone SDK API
  Reference.
- PowerMic III light values:
  [GoogleChromeLabs/dictation_support](https://github.com/GoogleChromeLabs/dictation_support)
  (`powermic_3_device.ts`).
- PowerScribe 360 button functions: Nuance, *PowerScribe 360 Reporting: Using the PowerMic
  II* (L-3322-006).
- All results that say "in testing" or "logged": the tests of the author with PowerScribe
  360 on Windows, October 2026.

## License

The project uses GPL-2.0-or-later, the same license as QMK (see [LICENSE](LICENSE)). The
patch changes QMK files, which also use GPL-2.0-or-later.
