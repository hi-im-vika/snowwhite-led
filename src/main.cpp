#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 6         // data pin for LED strip
#define SENSE_PIN 7       // sense pin to detect if strip is connected
#define PATT_PIN 2        // button pin to switch animations
#define LED_COUNT 32      // how many LEDs on strip
#define PRIMARY_HUE 80
#define WAVE_MAX 255
#define WAVE_MIN 32

CRGBArray<LED_COUNT> strip;

void patt_solid() {
    fill_solid(strip,LED_COUNT,CHSV(80,255,255));
}

void setup() {
    pinMode(SENSE_PIN, INPUT_PULLUP);
    pinMode(PATT_PIN, INPUT_PULLUP);
    CFastLED::addLeds<NEOPIXEL, LED_PIN>(strip, LED_COUNT);

    FastLED.setBrightness(255);
    FastLED.clear();
    FastLED.show();     // turn off all LEDs ASAP
}

void loop() {
  patt_solid();
  FastLED.show();
  yield();
}