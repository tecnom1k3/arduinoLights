#include <Adafruit_NeoPixel.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define PIN 3              // pin on which the NeoPixel will be connected
#define NUMPIXELS 3        // number of pixels in the NeoPixel
#define BUTTON_PIN   0     // pin on which the toggle button is connected
#define TOTAL_ROUTINES 3   // number of routines to show
#define DEBOUNCE_DELAY 50  // milliseconds to debounce the toggle button
#define LED 4              // pin on which the power on led is connected

Adafruit_NeoPixel strip;             // NeoPixel strip
int               stepNo;            // state, changes after each press of the toggle button
volatile byte     isInterrupted;     // if the toggle button has been pressed to switch routine
unsigned long     lastDebounceTime;  // milliseconds since start of last debounce

/**
   main setup routine
*/
void setup()
{
  GIMSK = 0b01000000;  // activate external interrupt
  MCUCR = 0b00000010;  // interrupt on LOW change
  sei();

  /* initializes variables */
  isInterrupted = LOW;
  stepNo        = 0;
  strip         = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

  /* turn on the power on led */
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  strip.begin(); // This initializes the NeoPixel library.
  strip.show();
}

/**
   main loop rountine
*/
void loop() {
  isInterrupted = LOW;
  startShow(stepNo);
}

/**
   based on the current state, start a show
*/
void startShow(int i) {
  switch (i % TOTAL_ROUTINES) {
    case 0:
      turnRed();
      break;
    case 1:
      turnGreen();
      break;
    case 2:
      turnBlue();
      break;
  }
}

/**
   turn NeoPixel strip lights red
*/
void turnRed()
{
  for (int i = 0; i < NUMPIXELS; i++) {
    if (isInterrupted == HIGH) return;
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    delay(100);
  }
  turnOff();
}

/**
   turn NeoPixel strip lights green
*/
void turnGreen()
{
  for (int i = 0; i < NUMPIXELS; i++) {
    if (isInterrupted == HIGH) return;
    strip.setPixelColor(i, strip.Color(0, 255, 0));
    strip.show();
    delay(100);
  }
  turnOff();
}

/**
   turn NeoPixel strip lights blue
*/
void turnBlue()
{
  for (int i = 0; i < NUMPIXELS; i++) {
    if (isInterrupted == HIGH) return;
    strip.setPixelColor(i, strip.Color(0, 0, 255));
    strip.show();
    delay(100);
  }
  turnOff();
}

/**
   turn off NeoPixel strip lights
*/
void turnOff()
{
  for (int i = 0; i < NUMPIXELS; i++) {
    if (isInterrupted == HIGH) return;
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    delay(100);
  }
}

/**
   clean NeoPixel strip
*/
void clean()
{
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

/**
   Debounce interrupt
*/
byte debounceInterrupt()
{
  int reading = HIGH;
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  byte result = LOW;
  lastDebounceTime = millis();

  if ((lastDebounceTime - last_interrupt_time) > DEBOUNCE_DELAY) {
    result = HIGH;
  }

  last_interrupt_time = lastDebounceTime;
  return result;
}

/**
   Interrupt service routine
*/
ISR(INT0_vect)
{
  isInterrupted = debounceInterrupt();
  if (isInterrupted == HIGH) {
    clean();
    stepNo++;
  }
}
