#include <Adafruit_NeoPixel.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define PIN 3              // pin on which the NeoPixel will be connected
#define NUMPIXELS 90        // number of pixels in the NeoPixel
#define BUTTON_PIN   0     // pin on which the toggle button is connected
#define TOTAL_ROUTINES 9   // number of routines to show
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
    case 3:
      doWipe();
      break;
    case 4:
      doTheaterChase();
      break;
    case 5:
      rainbow(20);
      break;
    case 6:
      rainbowCycle(20);
      break;
    case 7:
      theaterChaseRainbow(50);
      break;
    default:
      cycleAll();
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
    delay(50);
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
    delay(50);
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
    delay(50);
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
    delay(50);
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

void doWipe()
{
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  if (isInterrupted == HIGH) return;
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  if (isInterrupted == HIGH) return;
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  if (isInterrupted == HIGH) return;
}

void colorWipe(uint32_t c, uint8_t wait)
{
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    if (isInterrupted == HIGH) return;
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void doTheaterChase()
{
  theaterChase(strip.Color(127, 127, 127), 50); // White
  if (isInterrupted == HIGH) return;
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  if (isInterrupted == HIGH) return;
  theaterChase(strip.Color(0, 0, 127), 50); // Blue
  if (isInterrupted == HIGH) return;
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait)
{
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    if (isInterrupted == HIGH) return;
    for (int q = 0; q < 3; q++) {
      if (isInterrupted == HIGH) return;
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        if (isInterrupted == HIGH) return;
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        if (isInterrupted == HIGH) return;
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

void rainbow(uint8_t wait)
{
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    if (isInterrupted == HIGH) return;
    for (i = 0; i < strip.numPixels(); i++) {
      if (isInterrupted == HIGH) return;
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait)
{
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    if (isInterrupted == HIGH) return;
    for (i = 0; i < strip.numPixels(); i++) {
      if (isInterrupted == HIGH) return;
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait)
{
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    if (isInterrupted == HIGH) return;
    for (int q = 0; q < 3; q++) {
      if (isInterrupted == HIGH) return;
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        if (isInterrupted == HIGH) return;
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        if (isInterrupted == HIGH) return;
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

void cycleAll()
{
  turnRed();
  turnGreen();
  turnBlue();
  doWipe();
  doTheaterChase();
  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
