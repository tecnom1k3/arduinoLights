#include <Adafruit_NeoPixel.h>

#define PIN 12
#define NUM_LEDS 12
#define BUTTON_PIN 2
#define DEBOUNCE_DELAY 50
#define TOTAL_ROUTINES 3

Adafruit_NeoPixel strip;
int               buttonState;             // the current reading from the input pin
int               lastButtonState;
unsigned long     lastDebounceTime;  // the last time the output pin was toggled
int               pixelLed;
bool              debounce();
byte              debounceInterrupt();
void              turnRed();
volatile byte     isInterrupted;
int               stepNo;

void setup()
{
  pinMode(BUTTON_PIN, INPUT);

  strip            = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
  lastButtonState  = LOW;
  lastDebounceTime = 0;
  pixelLed         = 0;
  stepNo           = 0;
  isInterrupted    = LOW;

  strip.begin();
  strip.setPixelColor(pixelLed, strip.Color(0, 0, 0));
  strip.show();

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), evaluateInterrupt, RISING);
}

void loop()
{
  isInterrupted = LOW;
  switch (stepNo % TOTAL_ROUTINES) {
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
  for (int i = 0; i < NUM_LEDS; i++) {
    if (isInterrupted == HIGH) return;
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    delay(100);
  }
  turnOff();
}

void turnGreen()
{
  for (int i = 0; i < NUM_LEDS; i++) {
    if (isInterrupted == HIGH) return;
    strip.setPixelColor(i, strip.Color(0, 255, 0));
    strip.show();
    delay(100);
  }
  turnOff();
}

void turnBlue()
{
  for (int i = 0; i < NUM_LEDS; i++) {
    if (isInterrupted == HIGH) return;
    strip.setPixelColor(i, strip.Color(0, 0, 255));
    strip.show();
    delay(100);
  }
  turnOff();
}

void turnOff()
{
  for (int i = 0; i < NUM_LEDS; i++) {
    if (isInterrupted == HIGH) return;
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    delay(100);
  }
}

void clean()
{
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

bool debounce()
{
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        return true;
      }
    }
  }

  lastButtonState = reading;

  return false;
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
  lastButtonState = LOW;
  return result;
}

void evaluateInterrupt() {
  isInterrupted = debounceInterrupt();
  if (isInterrupted == HIGH) {
    clean();
    stepNo++;
  }
}
