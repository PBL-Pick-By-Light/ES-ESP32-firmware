/*
File: LED_h
Author: Moritz Krug
Description: Module for ESP32 firmware. Implements LED controls.
Last Edited: 20.01.2022 by David Lotz

This library is intended for a Adafruit NeoPixel LED stripe and
works with the Adafruit NeoPixel library to compliment it. This
library works staticly with one connected LED stripe and you dont
need to create any objects whatsoever.

Arduino core library is necessary.
*/

#ifndef SRC_LED_H
#define SRC_LED_H

/*!
    @brief  Initialize the esp and led stripe
*/
void LEDInit();

/*!
    @brief          Let the leds glow from start led to end led
                    with the rgb color
    @param  start   Number of the first led to be turned on,
                    0 to amount of leds
    @param  end     Number of the last led to be turned on,
                    start to amount of leds
    @param  red     Green brightness, 0 to 255
    @param  green   Red brightness, 0 to 255
    @param  blue    Blue brightness, 0 to 255
 */
void LEDGlow(int start, int end, int red, int green, int blue);

/*!
    @brief          Turn the leds off from start to end led
    @param  start   Number of the first led to be turned off,
                    0 to amount of leds
    @param  end     Number of the last led to be turned off,
                    start to amount of leds
*/
void LEDOff(int start, int end); //The LEDoff method turns off all pixels between the starting and ending pixels (including the start and end pixel)

/*!
    @brief  Turn all leds off from 0 to amount of pixels
*/
void LEDOffAll(); //This method turns off all pixels.

/*!
    @brief          Turn one led blue to find out the position
    @param  start   Number of the led to be turned on,
                    0 to amount of leds
*/
void LEDGlowStart(int start);

/*!
    @brief  Turns on leds after one starting led was set to indicate the shelf size
*/
void LEDGlowNext();

/*!
    @brief              Displays a "snake-like" animation with all available LEDs
    @param  rows        Number of available rows
    @param  columns     Number of available columns
    @param  rainbowMode True if LEDs shall cycle through every color
*/
void AnimationSnake(int rows, int columns, bool rainbowMode);

/*!
    @brief Light up all LEDs with red, green and blue
*/
void LightUpAll();
#endif
