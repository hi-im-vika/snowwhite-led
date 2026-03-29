#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 14        // data pin for LED strip
#define SENSE_PIN 15      // sense pin to detect if strip is connected
#define PATT_PIN 2        // button pin to switch animations

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}