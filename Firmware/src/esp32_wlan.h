/*
File: esp32_wlan.h
Author: David Lotz
Description: Module for a ESP32 low-level controller. Implements a Wireless Network setup.
Last Edited: 13.01.2022 by David Lotz
*/

#ifndef SRC_ESP32_WLAN_H
#define SRC_ESP32_WLAN_H

#include <WString.h>

//Try to connect to a WiFi network.
void setupWLAN(const char* ssid, const char* password);

//Get this clients MAC-Adress
String getMAC();

#endif