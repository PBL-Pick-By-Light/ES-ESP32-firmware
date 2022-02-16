#include "esp32_wlan.h"
#include "esp32_mqtt.h"

#include <Arduino.h>
#include <unity.h>

void test_led_builtin_pin_number(void) {
    TEST_ASSERT_EQUAL(2, LED_BUILTIN);
}

void test_led_state_high(void) {
    digitalWrite(LED_BUILTIN, HIGH);
    TEST_ASSERT_EQUAL(HIGH, digitalRead(LED_BUILTIN));
}

void test_led_state_low(void) {
    digitalWrite(LED_BUILTIN, LOW);
    TEST_ASSERT_EQUAL(LOW, digitalRead(LED_BUILTIN));
}

void test_get_mac(void) {
    String test = getMAC();
    TEST_ASSERT_EQUAL_INT(17, test.length());
}

void test_create_topic(void) {
    String topic = createTopic("test");
    TEST_ASSERT_EQUAL(true, topic.endsWith("test") && topic.startsWith("pbl/"));
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    pinMode(LED_BUILTIN, OUTPUT);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_led_builtin_pin_number);
    RUN_TEST(test_get_mac);
    RUN_TEST(test_led_state_high);
    RUN_TEST(test_led_state_low);
    RUN_TEST(test_create_topic);

    UNITY_END(); // stop unit testing
}

void loop(){

}