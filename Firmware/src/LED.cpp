/*
File: LED.h
Author: Moritz Krug
Description: Module for ESP32 firmware. Implements LED controls.
Last Edited: 20.01.2022 by David Lotz

This library is intended for a Adafruit NeoPixel LED stripe and works with the Adafruit NeoPixel library to
compliment it. This library works staticly with one connected LED stripe and you dont need to create any objects whatsoever.
Arduino core library is necessary.
*/

//Comment this out to not have any serial output
//#define DEBUG_MODE

#include "LED.h"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN        2  //Pin on the esp to connect the led stripe with
#define NUMPIXELS 144 //Amount of Pixel on stripe

Adafruit_NeoPixel Stripe1(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //initialization of a NeoPixel stripe Object
                                                                 //Amount of pixel, pin to connect, 
                                                                 //kind of Neopixel and frequenz picked of the datasheet

int currentLED;

void LEDInit() {
    pinMode(2, OUTPUT); //Pin 2 (D2) to output mode
    Stripe1.begin();
}                                

void LEDGlow(int start, int end, int red, int green, int blue) {
    for(int i=start; i <= end; i++) {
        Stripe1.setPixelColor(i, Stripe1.Color(red, green, blue)); //Turn on a led with rgb
    } 
    Stripe1.show(); //Update the led stripe
}

void LEDOff(int start, int end){
    for(int i=start; i <= end; i++) {
        Stripe1.setPixelColor(i, Stripe1.Color(0, 0, 0)); //Fill a led with zero to turn it black or off
    } 
    Stripe1.show(); //Update the led stripe
}

void LEDOffAll() {
    Stripe1.clear(); //Fill whole led stripe with zero
    Stripe1.show(); //Update the led stripe
}

void LEDGlowStart(int start) {
    Stripe1.clear(); //TODO(dpal): should remove this, clearing all leds beforehand should be called separate, any behaviour expecting this atm?
    currentLED = start; //Remember the starter led
    Stripe1.setPixelColor(currentLED, Stripe1.Color(0,0,255)); //Turn the starter led blue
    Stripe1.show(); //Update the led stripe
}

void LEDGlowNext() {
    currentLED = currentLED + 1; //Increase the led
    Stripe1.setPixelColor(currentLED, Stripe1.Color(0,0,255)); //Turn the next led blue
    Stripe1.show(); //Update the led stripe
}

void AnimationSnake(int rows, int columns, bool rainbowMode) {
    //Variables for changing colors in Rainbow-Mode
    int R = 0;
    if(!rainbowMode)
        R = 255;
    int G = 0;
    int B = 0;
    int intensity = 0;
    //Amount of LEDs available
    float led_count = rows * columns;
    //How many LEDs have currently been switched on
    float progress = 0;
    //Itterate through all rows
    for(int current_row = 0; current_row < rows+2; current_row++) {
        //If row is uneven, move from on side to another
        if(current_row % 2) {
            //Itterate through all columns
            for(int current_column = 0; current_column < columns; current_column++) {
                //In rainbowMode: Pick Color based on current progress
                if(rainbowMode) {
                    //First third: Red -> Green
                    if(progress < (led_count/3)) {
                        //Calculate intensity/brightness based on progress
                        intensity = (int)(255*(progress/(led_count/3)));
                        R = 255-intensity;
                        G = intensity;
                        B = 0;
                    //Second third: Green -> Blue
                    } else 
                    if(progress < 2*(led_count/3)) {
                        //Calculate intensity/brightness based on progress                        
                        intensity = (int)(255*((progress-(led_count/3))/(led_count/3)));
                        G = 255-intensity;
                        B = intensity;
                        R = 0;
                    //Last third: Blue -> Red
                    } else 
                    if(progress < led_count) {
                        //Calculate intensity/brightness based on progress
                        intensity = (int)(255*((progress-2*(led_count/3))/(led_count/3)));
                        B = 255-intensity;
                        R = intensity;
                        G = 0;
                    }
                }
                //Calculate LED to light up next
                currentLED = ((current_row*columns)+current_column);
                //Shut off LEDs in the row before
                if(current_row > 0) {
                    LEDOff(currentLED - columns*2, currentLED - columns*2);
                }
                if(current_row < rows) {
                LEDGlow(currentLED, currentLED, R, G, B);
                #ifdef DEBUG_MODE
                    Serial.printf("\nLighting LED No. %d Row: %d Column: %d Color: %d, %d, %d", currentLED, current_row, current_column, R, G, B);
                #endif
                }
                //Increment progress-counter for altering colors in rainbowMode
                if (progress < led_count)
                    progress += 1;
                #ifdef DEBUG_MODE
                    Serial.printf("\n Progress: %f / %f", progress, led_count);
                #endif
                //Delay by 100ms
                delay(50);
            }
        //If row is even, move from the opposite site to the other
        } else {
            //Itterate through all columns
            for(int current_column = (columns-1); current_column >= 0; current_column--) {
                //In rainbowMode: Pick Color based on current progress
                if(rainbowMode) {
                    //First third: Red -> Green
                    if(progress < (led_count/3)) {
                        //Calculate intensity/brightness based on progress
                        intensity = (int)(255*(progress/(led_count/3)));
                        R = 255-intensity;
                        G = intensity;
                        B = 0;
                    //Second third: Green -> Blue
                    } else 
                    if(progress < 2*(led_count/3)) {
                        //Calculate intensity/brightness based on progress
                        intensity = (int)(255*((progress-(led_count/3))/(led_count/3)));
                        G = 255-intensity;
                        B = intensity;
                        R = 0;
                    //Last third: Blue -> Red
                    } else 
                    if(progress < led_count) {
                        //Calculate intensity/brightness based on progress
                        intensity = (int)(255*((progress-2*(led_count/3))/(led_count/3)));
                        B = 255-intensity;
                        R = intensity;
                        G = 0;
                    }
                }
                currentLED = ((current_row*columns)+current_column);
                //Shut off LEDs that have been further away than 6 LEDs from the current LED
                if(current_row > 0) {
                    LEDOff(currentLED - columns*2, currentLED - columns*2);
                }
                if(current_row < rows) {
                LEDGlow(currentLED, currentLED, R, G, B);
                #ifdef DEBUG_MODE
                    Serial.printf("\nLighting LED No. %d Row: %d Column: %d Color: %d, %d, %d", currentLED, current_row, current_column, R, G, B);
                #endif
                }
                //Increment progress-counter for altering colors in rainbowMode
                if (progress < led_count)
                    progress += 1;
                #ifdef DEBUG_MODE
                    Serial.printf("\n Progress: %f/ %f", progress, led_count);
                #endif
                //Delay by 100ms
                delay(50);
            }
        }
    }
}

void LightUpAll() {
    //Light up whole shelf in red
    LEDGlow(0, NUMPIXELS, 255, 0, 0);
    delay(333);
    LEDOffAll();
    //Light up whole shelf in green
    LEDGlow(0, NUMPIXELS, 0, 255, 0);
    delay(333);
    LEDOffAll();
    //Light up whole shelf in blue
    LEDGlow(0, NUMPIXELS, 0, 0, 255);
    delay(333);
    LEDOffAll();
}