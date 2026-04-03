#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 10         // data pin for LED strip
#define SENSE_PIN 16       // sense pin to detect if strip is connected
#define MASK_LED_PIN 14         // data pin for mask LED strip
#define MASK_SENSE_PIN 15       // sense pin to detect if mask strip is connected
#define PATT_PIN 2        // button pin to switch animations
#define LED_COUNT 12      // how many LEDs on strip
#define MASK_LED_COUNT 24      // how many LEDs on strip
#define PRIMARY_HUE 24
#define WAVE_MAX 255
#define WAVE_MIN 8
#define SCROLL_UPDATE_TIME 9
#define DEBOUNCE_DELAY 10

// globals
// LED stuff
uint8_t wave_offset = 255;
CRGBArray<LED_COUNT> strip;
CRGBArray<MASK_LED_COUNT> mask_strip;

// state machine
unsigned long pressed_millis = 0;

// debounce
bool pressed = false;
bool acted_patt = false;

void patt_solid() {
    fill_solid(strip,LED_COUNT,CHSV(80,255,255));
}

void patt_scroll() {
    for (int i = 0; i < LED_COUNT; i++) {
            strip[i] = hsv2rgb_spectrum(CHSV(PRIMARY_HUE,255, map(cubicwave8(50 * i + wave_offset),0,255,WAVE_MIN,WAVE_MAX)));
    }
    for (int i = 0; i < MASK_LED_COUNT; i++) {
            mask_strip[i] = hsv2rgb_spectrum(CHSV(PRIMARY_HUE,255, map(cubicwave8(50 * i + wave_offset),0,255,WAVE_MIN,WAVE_MAX)));
    }
    EVERY_N_MILLIS(SCROLL_UPDATE_TIME) wave_offset--;
}

void poll_button() {
  // debounce tomfoolery
  bool any_button_pressed =
      digitalRead(PATT_PIN) == LOW;
  if (any_button_pressed && !pressed) {
    pressed = true;
    pressed_millis = millis();
  }

  // switch anim
  if (pressed) {
    // logic
    FastLED.setBrightness(0);
  }
}

void setup() {
    pinMode(SENSE_PIN, INPUT_PULLUP);
    pinMode(PATT_PIN, INPUT_PULLUP);
    CFastLED::addLeds<NEOPIXEL, LED_PIN>(strip, LED_COUNT);
    CFastLED::addLeds<NEOPIXEL, MASK_LED_PIN>(mask_strip, MASK_LED_COUNT);

    FastLED.setBrightness(255);
    FastLED.clear();
    FastLED.show();     // turn off all LEDs ASAP
}

void loop() {
  patt_scroll();
  FastLED.show();
  yield();
}