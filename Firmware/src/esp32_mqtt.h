/*
File: esp32_mqtt.h
Author: David Lotz
Description: Module for a ESP32 low-level controller. Implements the MQTT protocol for communication with a Broker.
Last Edited: 20.12.2021 by David Lotz
*/

#ifndef SRC_ESP32_MQTT_H
#define SRC_ESP32_MQTT_H

#include <stdint.h>
#include <WString.h>

typedef unsigned char byte;

//Used to set this client's settings regarding the aforementioned MQTT Broker
bool setupMQTT(const char* server_address, uint16_t port);

//This function is used above and creates new topics using the MAC-Address.
String createTopic(String attachment);

//Calls loop() function of MQTT to react to published messages
void listen();

#endif