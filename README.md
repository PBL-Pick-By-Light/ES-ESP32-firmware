# ESP32-firmware

Heart & soul of microcontrollers controlling shelfs, for awesome behavior like this:  

<img src="Showcase.gif" width="330" height="500"/>

## Description

Aim is to implement functionality needed to establish a connection with a WLAN and communicate with Back-/Frontend via MQTT.
Furthermore, includes software to control connected LEDs and support backup functionality. The current software is made to work with an [ESP32](https://www.espressif.com/en/products/socs/esp32) by [espressif](https://www.espressif.com/en).

## Getting Started

### Environment

For an easy setup, past developers chose to program the ESP32 in VSC using the PlatformIO Extension:
* [Visual Studio Code](https://code.visualstudio.com/)
* VSC Plugin: [PlatformIO IDE](https://platformio.org/platformio-ide)
* [USB-Driver for ESP32](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) (**only needed for Windows**) 

### Cloning

Run this command to clone this repo manually:
```bash
git clone https://git.thm.de/softwaretechnik-projekt-pick-by-light-system-wise21_22/pbl-embedded-system/esp32-firmware
```

### Flashing the firmware

* Load the project in Visual Studio Code
* Build with PlatformIO (to check if everything is correct)
* Connect the ESP32 via USB
* Upload with PlatformIO (You may need to hold the boot button of the ESP for it to get recognized)

### Testing

* Connect your ESP32 via USB
* Open up a console in VSC and run:
```bash
pio test
```

## Help

* If you are having problems with Python while building, delete the related folder.
* If the connecting bar is stuck while uploading, hold the boot button of the ESP until PlatformIO connects.

## License

[Apache License 2.0](https://www.tldrlegal.com/l/apache2)
