/*
File: esp32_mqtt.cpp
Author: David Lotz
Description: Module for a ESP32 low-level controller. Implements the MQTT protocol for communication with a Broker.
Last Edited: 26.01.2022 by David Lotz

Coded in Visual Studio Code with the PlatformIO IDE extension (https://platformio.org/)
*/

//Comment this out to not have any serial output
//#define DEBUG_MODE

#include "esp32_mqtt.h"
#include "esp32_wlan.h"
#include "LED.h"

#include <EEPROM.h>
#include <PubSubClient.h>
#include <WiFi.h>
//Number of Bytes used in the EEPROM Memory Space (4096 is the max value)
//Current configuration with 1024 Bytes is for a maximum of 64 Positions and 128 LEDs
#define MAX_Positions 64
#define MAX_LEDs      128  
#define EEPROM_SIZE   (MAX_Positions * (MAX_LEDs/8))
//Macro used for converting String to const char*
#define MK_STR(x) (x.c_str())
//Timer Setup (used for periodically sending a register message as "keep alive")
hw_timer_t* timer = NULL;
volatile bool sendKeepAlive = false;

#ifdef DEBUG_MODE
    //This function prints the bitmask of a given position
    void printEEPROM(char pos) {
        Serial.printf("\nEEPROM configuration:\n");
        for(int i = 1+(MAX_LEDs/8*pos); i < ((MAX_LEDs/8)+1)+(MAX_LEDs/8*pos); i++) { //Each position has MAX_LEDs/8 Bytes, multiplying by the current position gives us the Byte-Position in EEPROM.
            Serial.printf("%d(%d). ", i-1, i/(MAX_LEDs/8 - 1));
            for(int x = 0; x < 8; x++) {
                if(EEPROM.read(i-1) & (0b10000000 >> x)) {
                    Serial.printf("1");
                } else {
                    Serial.printf("0");
                }
            }
            Serial.printf("\t");
            if(i%4 == 0) {
                printf("\n");
            }
        }
    }
#endif

// ---MQTT-Topics--- //

//This clients MAC-Address
String mac = getMAC();

//Declare createTopic()
String createTopic(String attachment);

//Topics to publish messages to
String t_register = "pbl/register";
String t_config_ack = createTopic("/config/ack");
String t_config_offline = createTopic("/config/offline");
String t_light_ack = createTopic("/light/ack");
//Topics to listen for incoming messages
String t_config_reset = createTopic("/config/reset");
String t_config_updatePosition = createTopic("/config/update_Position");
String t_config_createPosition = createTopic("/config/create_Position");
String t_config_deletePosition = createTopic("/config/delete_Position");
String t_light_allOff = createTopic("/light/allOff");
String t_light_allOn = createTopic("/light/allOn");
String t_light_set = createTopic("/light/set");
String t_light_unset = createTopic("/light/unset");

//This function is used above and creates new topics using the MAC-Address.
String createTopic(String attachment) {
    return "pbl/"+mac+attachment;
}

//Create new PubSubClient client
WiFiClient esp_client;
PubSubClient client(esp_client);

//Used to connect to the MQTT Broker. setupMQTT() must be called once beforehand!.
//client_name is the name of this client, under which it is gonna be known under the client.
bool reconnect(const char* client_name) {
    #ifdef DEBUG_MODE
        Serial.printf("\nConnecting to MQTT-Broker with name \"%s\"...", client_name);
    #endif
    //Create empty String for publishing empty Last-Will Payload
    char empty_string[] = {};
    //Wait for the connection
    while(!client.connected()) {
        //Try to connect to the Broker and set Last Will
        client.connect(client_name, MK_STR(t_config_offline), MQTTQOS0, false, empty_string);
    }
    #ifdef DEBUG_MODE
        Serial.printf("\nConnected!");
    #endif
    return true;
}

//Subscribe to a specified topic
bool subscribeTo(const char* topic) {
    //Subscribe to topic
    return client.subscribe(topic);
}

//Publish a message (payload) on a specified topic.
bool publishMsg(const char* topic, const char* payload) {
    #ifdef DEBUG_MODE
        Serial.printf("\nPublishing a message... Time: %ld", millis());
    #endif
    return client.publish(topic, payload);
}

//Calls loop() function of MQTT to react to published messages. Sends keep-alive messages.
void listen() {
    if(sendKeepAlive) {
        publishMsg(MK_STR(t_register), mac.c_str());
        sendKeepAlive = false;
    }
    client.loop();
}

//Function used to send an acknowledgement corresponding to a "config" message
void sendConfigAck(char ack_id) {
        char* ack_byte = (char*) malloc(sizeof(char)*2);
        ack_byte[0] = ack_id;
        ack_byte[1] = '\0';
        publishMsg(MK_STR(t_config_ack), ack_byte);
        free(ack_byte);
}

//Function used to send an acknowledgement corresponding to a "light" message
void sendLightAck(char ack_id) {
        char* ack_byte = (char*) malloc(sizeof(char)*2);
        ack_byte[0] = ack_id;
        ack_byte[1] = '\0';
        #ifdef DEBUG_MODE
            Serial.printf("\nSend LightACK");
        #endif
        publishMsg(MK_STR(t_light_ack), ack_byte);
        free(ack_byte);
}

//This is called whenever this client receives a new message on a given subscribed topic.
void callback(char* topic, byte* message, unsigned int length) {

    //config/reset
    //Incoming Message resets all permanently stored data.
    if(!strcmp(topic, MK_STR(t_config_reset))) {
        #ifdef DEBUG_MODE
            Serial.printf("\nReset started.\nOverwriting EEPROM with zeroes...");
        #endif
        //Overwrite EEPROM with zeroes
        for(int i = 0; i < EEPROM_SIZE; i++) {
            //Only overwrite if Byte is not zero
            if (EEPROM.read(i) != 0) {
                #ifdef DEBUG_MODE
                    Serial.printf("\nOverwriting not-zero EEPROM address %d, corresponding to position %d byte %d from the left.", i, i/(MAX_LEDs/8), i%(MAX_LEDs/8));
                #endif
                EEPROM.write(i, 0);
            }
        }
        EEPROM.commit();
        #ifdef DEBUG_MODE
            Serial.printf("\nReset successfully completed!");
        #endif
        //Send ACK
        sendConfigAck(message[0]);
    }

    //config/update_Position OR config/create_Position
    //Incoming message saves an LED configuration for a specified position
    else if((!strcmp(topic, MK_STR(t_config_updatePosition))) || (!strcmp(topic, MK_STR(t_config_createPosition)))) {

        //Create Bit-Mask for all transmitted LEDs. A mask for a position is (MAX_LEDs/8) Bytes long.
        //Only overwrite position if message is valid (Has more than 2 Bytes)
        while(length > 2) {
            //Iterator for going through received message (back to front)
            length -= 1;
            //Current Byte in Position
            int nth_byte = message[length] / 8;
            //Current Bit in Byte
            int pos_in_byte = message[length] % 8;
            //Calculate position dependent on position mentioned in message
            int pos = ((MAX_LEDs/8) * (int)message[1] + ((MAX_LEDs/8)-1-nth_byte));
            #ifdef DEBUG_MODE
                Serial.printf("\nOverwriting EEPROM address %d, corresponding to position %d byte %d from the left.\n", pos, pos/(MAX_LEDs/8), pos%(MAX_LEDs/8));
            #endif
            //Set corresponding Bit if LED is part of this position
            EEPROM.write(pos, (EEPROM.read(pos) | (uint8_t)(0b00000001 << pos_in_byte))); 
        }
        EEPROM.commit();

        #ifdef DEBUG_MODE
            printEEPROM(message[1]);
        #endif
        
        //Send ACK
        sendConfigAck(message[0]);
    }

    //config/delete_Position
    //Incoming Message resets LED configuration for a specified position
    else if(!strcmp(topic, MK_STR(t_config_deletePosition))) {
        int pos = (int)message[1];
        int bytes_per_pos = (MAX_LEDs/8);
        //Iterate through every Byte of the corresponding position
        for(int i = pos*bytes_per_pos; i < (pos*bytes_per_pos+bytes_per_pos); i++) {
            //Only overwrite Byte if not already zero
            if(EEPROM.read(i) != 0) {
                EEPROM.write(i, 0);
            }
        }
        EEPROM.commit();

        //Send ACK
        sendConfigAck(message[0]);
    }

    //light/set
    //Incoming message lights up all specified LEDs
    else if(!strcmp(topic, MK_STR(t_light_set))) {
        --length;
        //Extract colors
        int B = message[length--];
        int G = message[length--];
        int R = message[length--];
        //Turn on all LEDs specififed in the message
        while(length > 0) {
            LEDGlow(message[length], message[length], R, G, B);
            length--;
        }
    
        //Send acknowledment
        sendLightAck(message[0]);
    }

    //light/unset
    //Incoming message turns off all specified LEDs
    else if(!strcmp(topic, MK_STR(t_light_unset))) {
        --length;
        //Turn off all LEDs specififed in the message
        while(length > 0) {
            LEDOff(message[length], message[length]);
            length--;
        }
    
        //Send acknowledment
        sendLightAck(message[0]);
    }

    //light/allOn
    //Incoming message turns on all LEDs
    else if(!strcmp(topic, MK_STR(t_light_allOn))) {
        //Turn on all LEDs
        LEDGlow(0, MAX_LEDs-1, message[1], message[2], message[3]);
        #ifdef DEBUG_MODE
            Serial.printf("\nTurned on all LEDs");
        #endif
        //Send ACK
        sendLightAck(message[0]);
    }

    //light/allOff
    //Incoming message turns off all LEDs
    else if(!strcmp(topic, MK_STR(t_light_allOff))) {
        //Turn off all LEDs
        LEDOffAll();
        #ifdef DEBUG_MODE
            Serial.printf("\nTurned off all LEDs");
        #endif
        //Send ACK
        sendLightAck(message[0]);
    }
}

//ISR for periodically sending register message
void IRAM_ATTR onTimer() {
    sendKeepAlive = true;
}

//Used to set this client's settings regarding the aforementioned MQTT Broker
bool setupMQTT(const char* server_address, uint16_t port) {
    //Configure MQTT server connection
    client.setServer(server_address, port);

    //Initialize 2048 Bytes of Memory for saving LED positions
    EEPROM.begin(EEPROM_SIZE);

    //Set callback() function
    client.setCallback(callback);

    //Connect to Broker
    reconnect(mac.c_str());

    //Register at Broker with MAC-Address
    publishMsg(MK_STR(t_register), mac.c_str());

    //Subscribe to all topics
    subscribeTo(MK_STR(t_config_reset));
    subscribeTo(MK_STR(t_config_updatePosition));
    subscribeTo(MK_STR(t_config_createPosition));
    subscribeTo(MK_STR(t_config_deletePosition));
    subscribeTo(MK_STR(t_light_set));
    subscribeTo(MK_STR(t_light_unset));
    subscribeTo(MK_STR(t_light_allOn));
    subscribeTo(MK_STR(t_light_allOff));

    //Setup Timer. Timer will tick every micro second.
    timer = timerBegin(0, 80, true);
    //Add interrupt to timer
    timerAttachInterrupt(timer, &onTimer, true);
    //Interrupt every 5 seconds
    timerAlarmWrite(timer, 5000000, true);
    //Start Timer
    timerAlarmEnable(timer);

    return true;
}
