# qmk-dictation-hid

**Dictation-handset buttons on your own keyboard, for radiologists who dictate in
PowerScribe 360.**

This is for radiologists who use Nuance **PowerScribe 360** and want the buttons of a
**PowerMic** or **Philips SpeechMike** (Dictate, Next field, Previous field, Enter,
Backspace and the rest) on a keyboard or macro pad that runs QMK firmware, instead of on
the handset in their hand.

The board tells the computer it is a **Nuance PowerMic II**. PowerScribe 360 supports the
PowerMic II itself, so it reads the board's buttons with **no driver, no add-on and no
macro software** on the workstation. You choose what each button does in PowerScribe's
own PowerMic settings, as you would for a real PowerMic. You keep dictating into your
usual microphone.

## Who this is for

- You dictate reports in **PowerScribe 360** on Windows.
- You want to press Dictate, Tab forward, Tab backward, Enter/Select and the other
  PowerMic buttons on keys, for example with the hand that is not on the mouse or
  trackball.
- You use a **PowerMic** or a **SpeechMike** now. The board takes over the buttons; your
  microphone keeps doing the audio. Without Philips' add-on a SpeechMike still works as a
  plain USB microphone. In testing, PowerScribe recorded from a wireless lapel microphone
  while the board supplied the buttons.
- You can build and flash QMK firmware, or are willing to learn. The example is for a
  Planck rev6; other ARM (ChibiOS) keyboards need small changes.

**What about the SpeechMike?** The board copies a PowerMic II only. PowerScribe 360 reads
SpeechMike buttons only through Philips' add-on, so a SpeechMike copy would still need that
add-on installed (gotcha 12). If you use a SpeechMike, keep it as your microphone and put
the PowerMic buttons on the board.

**Other software:** PowerScribe One, Dragon and other dictation software were not tested.
Any program that reads a PowerMic II may work, but there is no evidence here that it does.

## What is in this repository

The repository has three parts:

- a QMK patch that lets a keymap give QMK's joystick interface its own HID report
  descriptor and receive the reports the host sends back;
- `powermic.h`, the PowerMic II's USB identity, report descriptor and button keycodes, for
  any keymap's `config.h`;
- an example keymap for the Planck rev6 that puts the ten PowerMic buttons on the left
  and a number pad on the right.

> [!WARNING]
> **Check with your IT department before you plug this into a clinical or managed
> workstation.** The board uses a vendor ID that belongs to another company. Endpoint
> security, USB device control and asset tools can flag a device with an unknown or
> spoofed vendor ID, and many hospitals do not allow unapproved USB devices at all.

> [!IMPORTANT]
> **This is not a medical device.** It is a hobby project, provided as is, without
> warranty of any kind (see [LICENSE](LICENSE)). It is not tested, certified or cleared
> for clinical use. You are responsible for checking what every button does in your
> software before you use it on real work (see gotcha 11).

> [!NOTE]
> This project is not affiliated with, endorsed by or supported by Nuance Communications,
> Microsoft, Philips or Speech Processing Solutions. PowerMic, PowerScribe and Dragon are
> trademarks of their owners; SpeechMike is a trademark of its owner. The names are used
> only to say what the firmware is compatible with.

## What was tested

The author tested the example on a Planck rev6 (Drop edition, STM32F303) with
PowerScribe 360 on Windows in October 2026. PowerScribe 360 listed the board as a
PowerMicII-NS, acted on all five buttons that were assigned (Dictate, Tab forward, Tab
backward, Enter/Select and Custom left), recorded from a separate microphone, and lit the
record light. Other keyboards, other dictation software, PowerScribe One, Dragon and macOS
were not tested.

## How it works

QMK already has a joystick feature: a HID interface that reports buttons and axes. The
patch lets a keymap replace the joystick's report descriptor with the PowerMic II's own
64-byte descriptor, keep the joystick on its own USB interface and endpoint, and receive
the output and feature reports the host sends with `SET_REPORT`. `powermic.h` then sets the
USB identity and sizes the joystick report so it is byte for byte the PowerMic's 3-byte
button report. The keymap puts `JS_0` to `JS_9` (named `PM_DICT`, `PM_TABF` and so on) on
keys.

The board enumerates as two HID interfaces:

| Interface | What | Endpoint |
|---|---|---|
| 0 | QMK's boot keyboard (the number pad) | IN, 8 bytes, 1 ms |
| 1 | the PowerMic II button interface | IN, 4 bytes, 10 ms |

A real PowerMic II has three audio interfaces first and its HID interface is interface 3.
This board has no audio.

## Repository layout

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

## Build and flash

You need a working QMK setup (`qmk setup`, then `qmk doctor`). The patch was made against
QMK `master` at commit `7a1bbf37` (2026-10-02). Later QMK versions may need it updated.

1. Clone this repository anywhere, then go into it:

   ```sh
   git clone https://github.com/ryancummings/qmk-dictation-hid.git
   cd qmk-dictation-hid
   ```

2. Build. `build.sh` applies the patch to the QMK checkout that `qmk env` reports (it skips
   this if the patch is already applied), then runs `qmk compile` from this folder, which
   QMK treats as a userspace:

   ```sh
   ./build.sh                 # builds planck/rev6_drop:dictation_hid
   ```

   To apply the patch yourself instead: `git -C "$(qmk env | sed -n 's/^QMK_HOME="\(.*\)"$/\1/p')" apply patches/0001-joystick-custom-hid-interface.patch`.
   To remove it later: the same command with `apply -R`.

3. Flash. Run `./build.sh flash`, then put the board into its bootloader: hold **Fn**
   (bottom-left) and press the top-left key (`QK_BOOT`), or press the reset button on the
   back of the board. A Planck rev6 bootloader shows up as the STM32 DFU device `0483:df11`.

4. Check the result. On Linux, `lsusb -d 0554:1001` should list the board. On Windows, the
   board appears in Device Manager under Human Interface Devices with the standard
   Windows HID drivers.

5. In the dictation software, choose the PowerMic as the dictation device. Choose another
   microphone as the recording input, because this board has no audio. Then set what
   each button does (see gotcha 11).

### Using another keyboard

The patch works on ChibiOS (ARM) boards only. On another ChibiOS keyboard, copy the
`dictation_hid` folder into that keyboard's `keymaps` folder (or your userspace), change
`keymap.c` to that keyboard's layout macro, and keep `rules.mk` as it is. Turn off anything
in the keyboard's own `keyboard.json` that adds a HID collection (see gotcha 1). AVR (LUFA)
and V-USB boards are not supported: the patch's `SET_REPORT` handling is in ChibiOS's USB
code.

## Buttons and reports

The input report is 3 bytes: a constant `00`, then HID buttons 1 to 8 (bit 0 is button 1),
then HID buttons 9 to 14. The board sends a report each time a button changes, like the
real handset. QMK joystick button *n* is HID button *n* + 1.

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

Two buttons held together set both bits, for example Dictate and Enter/Select give
`00 04 01`. All buttons released is `00 00 00`.

The host sends two reports to the board, both with `SET_REPORT` on the control endpoint
(the handset has no OUT endpoint):

| Report | Size | What PowerScribe 360 sent in testing |
|---|---|---|
| Output (type 2): the record light | 1 byte | `01` while recording, `00` otherwise |
| Feature (type 3): vendor data | 39 bytes | one report starting `08`, a probe for the handset's barcode scanner |

The example keymap lights the underglow red while bit 0 of the light report is set. It
leaves the scanner probe unanswered, like a PowerMic without a scanner.

### Reading the buttons from your own software

Your software does not need to know about QMK. Open the HID collection with usage page
`0x01` (Generic Desktop) and usage `0x00` on vendor ID `0x0554`, product ID `0x1001`. On
Windows each input report you read is 4 bytes: `00` (the device uses no report IDs) and
then the 3 bytes above. For example, "Dictate pressed" is `00 00 04 00` and "released" is
`00 00 00 00`; your software maps that to whatever action it wants.

To send your own data to the board, you can use the 39-byte feature report
(`HidD_SetFeature` on Windows). Start your messages with bytes that the dictation
software does not use (it sent `08`), and handle them in `housekeeping_task_user` with
`joystick_receive_report()`. To answer a feature read, call `joystick_set_feature_reply()`.

## Gotchas

These are the problems found while getting this to work. Each one says how it was found.

1. **Every HID collection with the PowerMic vendor ID is taken for a PowerMic.**
   PowerScribe 360 opens every HID collection whose vendor ID is `0x0554` and treats each
   one as a PowerMic. With QMK's raw HID collection and its media-key collection beside
   the PowerMic collection, PowerScribe listed the device but ignored every button. With
   only the boot keyboard and the PowerMic collection, the buttons worked (tested). So
   `rules.mk` turns off raw HID (`RAW_ENABLE`), media keys (`EXTRAKEY_ENABLE`), NKRO, mouse
   keys and the debug console, and `keymap.c` refuses to build with any of them on. Mouse
   keys and the console were not tested on their own; they add collections, so they are
   off too. The boot keyboard is safe because Windows keeps keyboard collections to itself,
   so other programs cannot open them. Many keyboards turn some of these features on in
   their own `keyboard.json`; the keymap's `rules.mk` turns them off again.

2. **The button interface must have no report ID.** PowerScribe's button handling accepts
   only 4-byte reads: the report-ID byte that Windows adds (`00` when a device has no
   report IDs) and the 3 bytes of the report. QMK normally puts the joystick on its shared
   endpoint with a report ID when other features share one. The patch adds
   `JOYSTICK_OWN_EP = yes` to keep it on its own interface and endpoint, and it stops the
   build with an error if a custom descriptor would end up on the shared endpoint.

3. **QMK's joystick report must match the descriptor byte for byte.** QMK sends its own
   joystick report structure; the patch only changes the descriptor that describes it. The
   PowerMic's first byte is constant padding, which QMK has no option for, so `powermic.h`
   declares one 8-bit axis (`JOYSTICK_AXIS_COUNT 1`, `JOYSTICK_AXIS_RESOLUTION 8`) that is
   always 0, then 14 buttons, which QMK packs into 2 bytes. Change any of these and the
   report no longer matches the descriptor. The endpoint is 4 bytes, polled every 10 ms,
   like the real handset (from a real PowerMic II's `lsusb -v`).

4. **The host sends its reports on the control endpoint, which stock QMK ignores for the
   joystick.** The PowerMic has no OUT endpoint, so the host sets the light and the
   feature report with `SET_REPORT`. Stock QMK accepts `SET_REPORT` only on the keyboard
   interfaces. The patch accepts it on the joystick interface too. The USB callback runs in
   an interrupt, so it only stores the report; read it from the main loop with
   `joystick_receive_report()`, as `keymap.c` does.

5. **The feature report is a barcode-scanner check; leave it unanswered.** PowerScribe sent
   one 39-byte feature report starting `08` when it opened the device (logged on the board
   in testing). Its PowerMic support uses the feature report only for the PowerMic II's
   barcode scanner. A board with no scanner can ignore it. A feature read (`GET_REPORT`)
   returns the last feature report set, or zeros; the host was not seen reading it.

6. **The record light is bit 0 of the output report.** PowerScribe 360 set the light report
   to `01` while recording and `00` otherwise (logged on the board in testing). This also
   matches the PowerMic III light values in GoogleChromeLabs' `dictation_support`.

7. **The handset's trigger is not a button here.** On a real handset the trigger shows up in
   the first byte, which the descriptor declares constant. In Nuance's model the trigger
   belongs to the barcode scanner, a separate device, so the board does not send it.

8. **There is no audio.** A real PowerMic is also a USB microphone. This board is not.
   PowerScribe 360 accepted a PowerMic with no audio interfaces and recorded from a
   microphone chosen separately as the recording input (tested). Choose your microphone in
   the dictation software's audio settings.

9. **No serial number.** The real PowerMic II reports no serial number. QMK on ChibiOS
   makes one from the chip's unique ID unless `SERIAL_NUMBER_USE_HARDWARE_ID` is `FALSE`,
   which `powermic.h` sets. Whether PowerScribe cares was not tested; the setting simply
   copies the real device. The vendor and product strings are the real handset's too
   ("Dictaphone Corporation", "PowerMicII-NS"); only this full set was tested.

10. **Software that matches only VID and PID also matches a real PowerMic.** If you write
    your own tool or a Linux udev rule for the board, remember that a genuine PowerMic II
    has the same IDs. One way to tell them apart: the board's PowerMic collection is on
    USB interface 1, a real PowerMic II's on interface 3. A udev rule that lets a
    logged-in user open the board on Linux (it matches a real PowerMic too):

    ```
    SUBSYSTEM=="hidraw", ATTRS{idVendor}=="0554", ATTRS{idProduct}=="1001", TAG+="uaccess"
    ```

11. **The dictation software decides what each button does, per user and per system.
    Check every system before you rely on it.** The firmware only says "Custom left is
    pressed". In PowerScribe 360 the action is set under Tools → Preferences → PowerMic,
    and the settings are kept per user on each PowerScribe system. In testing, one system
    had Custom left set to *Sign Report*, so a key meant as Backspace signed a report. On
    every system you use, open the PowerMic button settings before you use the board, and
    make sure no button you put on a key signs, deletes or sends anything you did not mean.

12. **Copying a Philips SpeechMike does not help with PowerScribe 360.** A QMK board can copy
    a SpeechMike's vendor HID interface too, but PowerScribe 360 does not read SpeechMike
    buttons itself. It reads them only through Philips' add-on (`PS360SpeechMikeSupport.exe`),
    which installs a Windows multimedia (MCI) driver that reads the SpeechMike and passes
    each button to PowerScribe. Without the add-on PowerScribe detects the SpeechMike but
    ignores every button (tested with a real SpeechMike Premium Touch as well). The
    PowerMic II is supported by PowerScribe 360 itself, which is why this project copies it.

13. **HID buttons are not keys.** The PowerMic buttons are button states, not keystrokes, so
    there is no keyboard auto-repeat while a button is held and they type nothing in other
    programs. The board sends one report each time a button changes. QMK's normal debounce
    applies; no debounce problems were seen.

14. **Planck rev6: QMK's audio driver froze the board.** In all-day use, QMK's DAC audio
    driver froze a Planck rev6 now and then (it dropped off USB until replugged), at any
    volume, and never with audio compiled out. `rules.mk` turns audio off. This is about
    the board, not about the PowerMic interface.

15. **Flashing needs a key or the reset button.** With raw HID and the console off, nothing
    on the computer can tell the board to restart into its bootloader. Use `QK_BOOT` (Fn
    plus the top-left key in the example) or the reset button.

### Not tested

There is no evidence here about: Windows keeping old descriptors after you reflash with
the same VID and PID (not seen in testing), using the board next to a genuine PowerMic,
PowerMic III emulation, PowerScribe One, Dragon, or other operating systems.

## Sources

- PowerMic II report descriptor and per-button captures: from a USBPcap capture of a
  genuine PowerMic II, documented in
  [whyaaronbailey/adafruit_rp2040_usbh](https://github.com/whyaaronbailey/adafruit_rp2040_usbh)
  (`keymaps/tartarus2_qmk/powermic.h`). The descriptor in `powermic.h` is a byte copy.
- USB identity (IDs, `bcdDevice` 2.04, strings, no serial, HID on interface 3, 4-byte
  endpoint at 10 ms): a real PowerMic II's `lsusb -v` in
  [linuxhw/LsUSB](https://github.com/linuxhw/LsUSB).
- Button names and order: Nuance's `PMII_MICBUTTONS` table (Nuance Microphone SDK API
  Reference).
- PowerMic III light values: [GoogleChromeLabs/dictation_support](https://github.com/GoogleChromeLabs/dictation_support)
  (`powermic_3_device.ts`).
- PowerScribe 360 button functions: Nuance, *PowerScribe 360 Reporting: Using the PowerMic
  II* (L-3322-006).
- Everything marked "tested" or "logged": the author's own testing with PowerScribe 360 on
  Windows, October 2026.

## License

GPL-2.0-or-later, the same as QMK. See [LICENSE](LICENSE). The patch changes QMK files,
which are GPL-2.0-or-later.
