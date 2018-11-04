#include <Adafruit_NeoPixel.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define PIN 3
#define NUMPIXELS 3
#define BUTTON_PIN   0
#define TOTAL_ROUTINES 3
#define DEBOUNCE_DELAY 50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int           stepNo;
volatile byte isInterrupted;
unsigned long lastDebounceTime;

void setup()
{
  GIMSK = 0b01000000;
  MCUCR = 0b00000010;
  sei();

  isInterrupted = LOW;
  stepNo        = 0;

  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin(); // This initializes the NeoPixel library.
  strip.show();
}



void loop() {
  isInterrupted = LOW;
  startShow(stepNo);
}

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

void turnOff()
{
  for (int i = 0; i < NUMPIXELS; i++) {
    if (isInterrupted == HIGH) return;
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    delay(100);
  }
}

void clean()
{
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

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

ISR(INT0_vect )
{
  isInterrupted = debounceInterrupt();
  if (isInterrupted == HIGH) {
    clean();
    stepNo++;
  }
}
