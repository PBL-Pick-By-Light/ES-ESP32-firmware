/*
File: esp32_wlan.cpp
Author: David Lotz
Description: Module for a ESP32 low-level controller. Implements a Wireless Network setup.
Last Edited: 20.01.2022 by David Lotz

Coded in Visual Studio Code with the PlatformIO IDE extension (https://platformio.org/)
*/

//Comment this out to not have any serial output
//#define DEBUG_MODE

#include "esp32_wlan.h"

#include <WiFi.h>

//Connect to a WiFi network
void setupWLAN(const char* ssid, const char* password) {
    #ifdef DEBUG_MODE
        Serial.printf("\nConnecting to WiFi-Network \"%s\" with password \"%s\"...", ssid, password);
    #endif
    //Start the WiFi connection
    WiFi.begin(ssid, password);

    //Wait for the connection to occur. 
    while(WiFi.status() != WL_CONNECTED);
    #ifdef DEBUG_MODE
        Serial.printf("\nConnected!");
    #endif
}

//Returns this clients MAC-Adress
String getMAC() {
    //Get MAC
    return WiFi.macAddress();
}
