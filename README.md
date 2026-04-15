## Rudicore-M5 ##

<img width="300" height="278" alt="rudicore-m5" align="left" src="/logo/rudicore-m5.png" />

**Rudicore-M5** is a custom firmware for the [M5Stack](https://m5stack.com/) platform that turns the device into a flexible **I²C ↔ UART bridge**.

It accepts simple, human-readable queries over UART (via USB or Bluetooth), translates them into I²C communication with connected M5Stack units or sensors, and returns the results over the same UART connection.

This makes it easy to interface with M5Stack sensors and actuators from any host system that can **communicate over UART** — for example, from a **Python script**, a **terminal**, or **embedded hardware** including phones or smartwatches.

---

## Tools

Rudicore-M5 works together with several related projects that make it easier to use:

- **[rudipy](https://github.com/rudimesh/rudipy/)** – a Python library for easy communication with Rudicore-M5.
- **[RudiConnect](https://github.com/rudimesh/RudiConnect)** – an Android app for quick testing and control on the go.
- **[RudiWear](https://github.com/rudimesh/RudiWear)** – a WearOS app that lets you use Rudicore-M5 from your smartwatch.

## Example

Device sends:
```
>ENVII.GetTemperature()
```

M5Stack communicates with the M5Stack ENVII unit over I²C, reads the data, and returns the result:
```
25.37
```

For a full list of supported M5Stack Units and their command strings see <a href="docs/command_reference.md">the command reference</a>.

## How to install:

- Web based flashing tool:
  - The easiest way to install Rudicore-M5 on your device is to use the web-based firmware flasher. Click [here](https://rudimesh.github.io/Rudicore-M5/webflasher.html) to flash the firmware directly from your browser.

- From a binary:
  - Go over to the releases page.
  - Expand "Assets" and download the latest binary.
  - Flash it with [esptool](https://github.com/espressif/esptool), e.g.:  
  ```
  esptool.py --chip esp32 --port COM1 write_flash 0x0 Rudicore-M5.bin
  ```
  Replace COM1 with the COM port your M5Stack is connected to. For Linux use the appropriate ttyUSB or ttyACM device name in /dev.

- From source:
  - Clone the repository.
  - Open the folder with [PlatformIO](https://platformio.org/).
  - Build and flash from there.

## License

Rudicore-M5 was developed by Hans Zondag and Thieu Asselbergs at Radboud University and is licensed under the GNU General Public License v3.0 (GPL-3.0).

This means you are free to use, study, share, and modify the software, but any derivative work or redistribution must also be licensed under the GPL-3.0.
