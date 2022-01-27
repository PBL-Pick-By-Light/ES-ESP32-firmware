/*
File: main.cpp
Author: David Lotz
Description: Main runnable for execution on esp32. Responsible for importing all modules and initializing settings.
Last Edited: 20.01.2022 by David Lotz

Coded in Visual Studio Code with the PlatformIO IDE extension (https://platformio.org/)
*/

//Comment this out to not have any serial output
//#define DEBUG_MODE

#include <Arduino.h>

#include "esp32_wlan.h"
#include "esp32_mqtt.h"
#include "LED.h"

// ---WLAN connection settings--- //
const char* ssid = "SWTP_TEST_WLAN";
const char* password = "SWTPswtp";

// ---MQTT Broker connection settings--- //
//Broker Server Address
const char* mqtt_server = "192.168.1.116";
//Port Number. 1883 is the default Port for MQTT.
uint16_t mqtt_port = 1883;

#ifndef UNIT_TEST
//Executed at startup, initalize all connections and settings.
void setup() {
    #ifdef DEBUG_MODE
        Serial.begin(9600);
    #endif
    //Initialize LED stripe
    LEDInit();

    //Display Startup animation
    AnimationSnake(6, 4, true);
    delay(333);
    //Light up whole shelf in red
    LEDGlow(0, 255, 255, 0, 0);
    delay(333);
    LEDOffAll();
    //Light up whole shelf in green
    LEDGlow(0, 255, 0, 255, 0);
    delay(333);
    LEDOffAll();
    //Light up whole shelf in blue
    LEDGlow(0, 255, 0, 0, 255);
    delay(333);
    LEDOffAll();

    //Initialize WLAN connection
    setupWLAN(ssid, password);
    //Initialize Broker connection
    setupMQTT(mqtt_server, mqtt_port);
}

//Executed and looped through after setup() ran
void loop() {
    //Listen to incoming MQTT messages
    listen();
}
#endif