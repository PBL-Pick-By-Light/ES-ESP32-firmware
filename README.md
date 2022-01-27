# ESP32-firmware

Firmware of an esp who controls the leds of a shelf.

## Description

This project contains the controls of the leds with adafruit, establishes a connection with the internet and listens to different topics with mqtt.

## Getting Started

### Dependencies

* Visual Studio Code https://code.visualstudio.com/
* VSC Plugin: PlatformIO IDE https://platformio.org/platformio-ide
* Driver for ESP (only Windows USB Driver) https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

### Cloning

```bash
git clone https://git.thm.de/softwaretechnik-projekt-pick-by-light-system-wise21_22/pbl-embedded-system/esp32-firmware
```

### Uploading onto esp32

* Load project in Visual Studio Code
* Build with Platformio (to check if everything is correct)
* Connect the ESP to the computer with usb
* Upload with Platformio

### Testing

* Connect esp32 first

```bash
pio test
```

## Help

* If your having problems with python while building, delete the related folder.
* If the connecting bar is stuck while uploading, hold the reset button on the esp until platformio connects

## License

[Apache License 2.0](https://www.tldrlegal.com/l/apache2)
