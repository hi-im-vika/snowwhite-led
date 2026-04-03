#include <Arduino.h>
#include <FastLED.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define LED_PIN 10        // data pin for LED strip
#define SENSE_PIN 16      // sense pin to detect if strip is connected
#define MASK_LED_PIN 14   // data pin for mask LED strip
#define MASK_SENSE_PIN 15 // sense pin to detect if mask strip is connected
#define PATT_PIN 2        // button pin to switch animations

#define LED_COUNT 12      // how many LEDs on strip
#define MASK_LED_COUNT 24 // how many LEDs on strip
#define PRIMARY_HUE 24
#define WAVE_MAX 255
#define WAVE_MIN 8

#define SCROLL_UPDATE_TIME 9
#define RAINBOW_UPDATE_TIME 10
#define MASK_STARTUP_UPDATE_TIME 20

#define DEBOUNCE_DELAY 10

// globals
// LED stuff
uint8_t wave_offset = 255;
CRGBArray<LED_COUNT> strip;
CRGBArray<MASK_LED_COUNT> mask_strip;
uint8_t rainbow_hue = 0;
uint8_t global_brightness = 255;
uint8_t mask_brightness = 0;
uint8_t visor_brightness = 0;

// state machine
unsigned long pressed_millis = 0;
bool do_visor_startup = true;
bool do_mask_startup = true;

// debounce
bool pressed = false;
bool acted_patt = false;

void next_pattern();
void patt_solid();
void patt_scroll();
void patt_rainbow();

typedef void (*pattern_list_t[])();
pattern_list_t patterns = {patt_solid, patt_scroll, patt_rainbow};
uint8_t current_pattern_idx = 0;

void poll_button() {
  // debounce tomfoolery
  bool any_button_pressed = digitalRead(PATT_PIN) == LOW;
  if (any_button_pressed && !pressed) {
    pressed = true;
    pressed_millis = millis();
  }

  // switch anim
  if (pressed) {
    // logic
    if (millis() - pressed_millis > DEBOUNCE_DELAY) {
      if (digitalRead(PATT_PIN) == LOW && !acted_patt) {
        next_pattern();
        acted_patt = true;
        // EEPROM.write(0, current_pattern_idx);
        // EEPROM.commit();
      } else if (digitalRead(PATT_PIN) == HIGH && acted_patt) {
        pressed = false;
        acted_patt = false;
      }
    }
  }
}

void setup() {
  pinMode(MASK_SENSE_PIN, INPUT_PULLUP);
  pinMode(PATT_PIN, INPUT_PULLUP);
  CFastLED::addLeds<NEOPIXEL, LED_PIN>(strip, LED_COUNT);
  CFastLED::addLeds<NEOPIXEL, MASK_LED_PIN>(mask_strip, MASK_LED_COUNT);

  FastLED.setBrightness(255);
  FastLED.clear();
  FastLED.show(); // turn off all LEDs ASAP
}

void loop() {
  FastLED.clear();
  patterns[current_pattern_idx]();
  // update visor
  if (do_visor_startup) {
    if (visor_brightness < global_brightness) {
      visor_brightness++;
      strip.fadeToBlackBy(global_brightness - visor_brightness);
    } else {
      do_visor_startup = false;
      visor_brightness = global_brightness;
      strip.fadeToBlackBy(global_brightness - visor_brightness);
    }
  }
  // if mask connected, do mask stuff
  if (digitalRead(MASK_SENSE_PIN) == LOW) {
    EVERY_N_MILLIS(MASK_STARTUP_UPDATE_TIME) next_mask_idx = true;
    if (do_mask_startup && next_mask_idx) {
      if (mask_idx < MASK_LED_COUNT) {
        mask_idx++;
      } else {
        do_mask_startup = false;
      }
      next_mask_idx = false;
    }
  } else {
    next_mask_idx = true;
    do_mask_startup = true;
    mask_brightness = 0;
    mask_idx = 0;
  }
  if (!do_visor_startup && (digitalRead(MASK_SENSE_PIN) == HIGH || (!do_mask_startup))) {
    poll_button();
    FastLED.setBrightness(global_brightness);
  }
  // update mask

  // show all
  FastLED.show();
  yield();
}

void next_pattern() {
  // check for underflow
  uint8_t result = global_brightness - 64;
  if (result < global_brightness) {
    global_brightness = global_brightness - 64;
  } else {
    global_brightness = 255;
    current_pattern_idx = (current_pattern_idx + 1) % ARRAY_SIZE(patterns);
  }
}

void patt_solid() {
  fill_solid(strip, LED_COUNT, CHSV(PRIMARY_HUE, 255, 255));
  fill_solid(mask_strip, MASK_LED_COUNT, CHSV(PRIMARY_HUE, 255, 255));
}

void patt_scroll() {
  for (int i = 0; i < LED_COUNT; i++) {
    strip[i] = hsv2rgb_spectrum(CHSV(
        PRIMARY_HUE, 255,
        map(cubicwave8(50 * i + wave_offset), 0, 255, WAVE_MIN, WAVE_MAX)));
  }
  for (int i = 0; i < MASK_LED_COUNT; i++) {
    mask_strip[i] = hsv2rgb_spectrum(CHSV(
        PRIMARY_HUE, 255,
        map(cubicwave8(50 * i + wave_offset), 0, 255, WAVE_MIN, WAVE_MAX)));
  }
  EVERY_N_MILLIS(SCROLL_UPDATE_TIME) wave_offset--;
}

void patt_rainbow() {
  EVERY_N_MILLIS(RAINBOW_UPDATE_TIME) rainbow_hue--;
  fl::fill_rainbow_circular(strip, LED_COUNT, rainbow_hue, false);
  fl::fill_rainbow_circular(mask_strip, MASK_LED_COUNT, rainbow_hue, false);
}