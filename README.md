# rally-handbreak

A simple USB handbrake controller for sim racing, built for the Raspberry Pi Pico using the TinyUSB stack.

This project presents a minimal composite USB device:

- **HID Joystick**: Reports a single analog axis (handbrake position) to your PC/game.
- **CDC Serial**: Allows debug output and live ADC value monitoring over USB serial.

## Features

- Reads an analog potentiometer (handbrake lever) using the Pico's ADC.
- Sends the value as a joystick axis via USB HID.
- Includes a deadzone and scaling for the analog input.
- Debug output over USB serial for easy calibration and troubleshooting.
- Onboard LED blinks to indicate device status.

I am using a modified version of [this project](https://www.thingiverse.com/thing:5213117) for the actual handbreak. It works great!

## Hardware

- Raspberry Pi Pico (RP2040) also works with the Seeed Xiao 2040
- Potentiometer (connected to an ADC pin, e.g., GP26/ADC0)

## Usage

1. **Build and flash** the firmware to your Pico.
2. Connect the Pico to your PC via USB.
3. The device will appear as a joystick and a serial port.
4. Use a serial terminal (e.g., `screen`, `minicom`, or VS Code's serial monitor) to view debug output and calibrate your handbrake.

## Building

This project uses the [Pico SDK](https://github.com/raspberrypi/pico-sdk) and [TinyUSB](https://github.com/hathach/tinyusb).

Make sure you have the Raspberry Pi Pico Visual Studio Code extension installed. TinyUSB is included in the Pico SDK so no need to install separately.

```sh
git clone <this repo>
cd rally-handbreak
mkdir build && cd build
cmake -G Ninja ..
```

Use the Pico SDK 'Compile' and 'Run' buttons to build and flash.

## Customization

- Adjust `DEADZONE_MIN` and `ADC_MAX` in `main.c` to match your potentiometer's range.
- Edit the HID report descriptor in `usb_descriptors.c` for more axes or buttons.

## Credits

- Based on the TinyUSB HID composite example by @hathach.
- Adapted for sim racing handbrake use by [your name].

---

MIT License
