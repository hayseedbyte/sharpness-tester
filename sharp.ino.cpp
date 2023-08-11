#include <Arduino.h>
#line 1 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
#include <Adafruit_ST7789.h>
#include <Adafruit_ST77xx.h>
#include <ArduinoGraphics.h>
#include <Arduino_GFX_Library.h>
// #include "extern/../pico-scale/include/hx711_scale_adaptor.h";
// #include "extern/../pico-scale/include/scale.h";
#include <stdio.h>
#include "stdlib.h"
#include "hardware/adc.h"
#include <string.h>
#define GFX_BL 13 // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
#define TFT_BL 13
//(DC, CS, SCK, MOSI, DIN, SPI);
Arduino_DataBus *bus = new Arduino_RPiPicoSPI(8, 9, 10, -1, 11, spi1);
//(bus, RST, rotation, ips, width, height)
Arduino_GFX *gfx = new Arduino_ST7789(bus, 12, 3, true, 240, 320);

#include "HX711.h"
#include <SPI.h>

// pins
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
const int calibPin = 18; // the number of the pushbutton pin
const int zeroPin = 19;
const int storePin = 20;
const int pin4 = 21;
const int pin5 = 22;
int pressureAnalogPin = 26;

float calibration_factor = -466; //  adjusted by the calibrate() function
const int calibration_weight = 200;
HX711 scale;

// button states
int calibState = 0;     // current state of the button
int lastCalibState = 0; // previous state of the button
int zeroState = 0;
int lastZeroState = 0;
int storeState = 0;
int lastStoreState = 0;
int storeIndex = 0;

// weight and pressure readings
int lastPressure;
long lastReading = 0;
int reading;
int highest;
int lastHighest;

// smooth out pressure reading
const int numPressure = 10;
int pressure[numPressure]; // the pressure from the analog input
int readIndex = 0;         // the index of the current Pressure
int totalPressure = 0;     // the running totalPressure
int averagePressure = 0;   // the averagePressure

int results[3];
int avg;
int offset = 1;

#line 62 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
void setup(void);
#line 90 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
void loop();
#line 186 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
void buttonWait(int buttonPin);
#line 198 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
void calibrate();
#line 234 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
void drawAvg(int avg);
#line 241 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
void drawHighest(int highest);
#line 249 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
void drawReading(int reading);
#line 256 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
void drawPressure(int pressure);
#line 295 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
int zero();
#line 62 "F:\\clint\\portfolio\\sharpness-tester\\sharp\\sharp.ino"
void setup(void)
{
    // initialize graphics
    gfx->begin();
    gfx->fillScreen(BLACK);
    gfx->setTextSize(5);
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // initialize scale
    // set pin modes
    pinMode(calibPin, INPUT);
    pinMode(zeroPin, INPUT);
    pinMode(pressureAnalogPin, INPUT);
    pinMode(storePin, INPUT);

#ifdef GFX_BL
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
#endif

    scale.set_scale(calibration_factor); // Adjust to this calibration

    // initialize all the pressure to 0:
    for (int thisPressure = 0; thisPressure < numPressure; thisPressure++)
    {
        pressure[thisPressure] = 0;
    }
    delay(5000); // 5 seconds
}

void loop()
{
    // get inputs
    calibState = digitalRead(calibPin);
    zeroState = digitalRead(zeroPin);
    storeState = digitalRead(storePin);

    long reading = scale.get_units(5);

    // compare the calibState to its previous state
    if (calibState != lastCalibState)
    {
        if (calibState == HIGH)
        {

            calibrate();
        }
        delay(50);
    }
    if (zeroState != lastZeroState)
    {
        if (zeroState == HIGH)
        {
            zero();
            highest = 0;
        }
        delay(50);
    }
    if (storeState != lastStoreState)
    {
        if (storeState == HIGH)
        {
            if (storeIndex < 3)
            {
                results[storeIndex] = highest;
                storeIndex++;
            }
            else
            {
                storeIndex = 0;
                results[storeIndex] = highest;
                storeIndex++;
            }
        }
    }
    // save the current state as the last state, for next time through the loop
    lastStoreState = storeState;
    lastCalibState = calibState;
    lastZeroState = zeroState;
    if (reading > highest)
    {
        highest = reading;
    }
    if (highest > lastHighest + offset || highest < lastHighest - offset) // only write if number changes
    {
        drawHighest(highest);
    }
    if (reading > lastReading + offset || reading < lastReading - offset)
    {
        drawReading(reading);
    }
    if (results[3])
    {
        int i = 0;
        int n = 0;
        for (i = 0; i < 3; i++)
        {
            n = n + results[i];
        }
        avg = n / 3;
        drawAvg(avg);
    }
    // subtract the last Pressure:
    totalPressure = totalPressure - pressure[readIndex];
    // read from the sensor:
    pressure[readIndex] = analogRead(pressureAnalogPin);
    // add the Pressure to the totalPressure:
    totalPressure = totalPressure + pressure[readIndex];
    // advance to the next position in the array:
    readIndex = readIndex + 1;

    // if we're at the end of the array...
    if (readIndex >= numPressure)
    {
        // ...wrap around to the beginning:
        readIndex = 0;
    }
    // calculate the averagePressure:
    averagePressure = totalPressure / numPressure;
    delay(10); // delay in between reads for stability
    drawPressure(averagePressure);
    lastReading = reading;
    lastHighest = highest;
    lastStoreState = storeState;
}

void buttonWait(int buttonPin)
{
    int state = 0;
    while (1)
    {
        state = digitalRead(buttonPin);
        if (state == HIGH)
        {
            return;
        }
    }
}
void calibrate()
{

    gfx->setTextSize(3);    // Make text smaller
    gfx->fillScreen(BLACK); // blank screen
    gfx->setCursor(10, 10);
    gfx->println("Remove all weight from \n scale then press \n calibrate");
    buttonWait(5); // wait on user to press button
    delay(50);
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->println("Zeroing...");
    delay(50);
    scale.set_scale(); // reset scale to 0
    scale.tare();      // Reset the scale to 0
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->println("Place 200g weight \n on scale then \n press calibrate.");
    buttonWait(5);
    delay(50);
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->println("Calibrating....");
    long current = scale.get_units(10);                // average of ten readings
    calibration_factor = current / calibration_weight; // calculate new calibration factor
    scale.set_scale(calibration_factor);
    delay(50);
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->println("Done!");
    highest = 0; // reset highest
    drawHighest(highest);
    delay(50);
    gfx->setTextSize(5); // set text back to larger size
    delay(100);
}
void drawAvg(int avg)
{
    gfx->setCursor(150, 10);
    gfx->println("-------- "); // fill with dashes to clear line
    gfx->setCursor(150, 10);
    gfx->println(avg);
}
void drawHighest(int highest)
{

    gfx->setCursor(10, 10);
    gfx->println("-------- "); // fill with dashes to clear line
    gfx->setCursor(10, 10);
    gfx->println(highest);
}
void drawReading(int reading)
{
    gfx->setCursor(10, 80);
    gfx->println("--------");
    gfx->setCursor(10, 80);
    gfx->println(reading);
}
void drawPressure(int pressure)
{
    // gfx->setCursor(10, 180);
    // gfx->println("---------"); // fill with dashes to clear line
    // gfx->setCursor(10, 180);
    // gfx->println(pressure);
    gfx->setCursor(10, 180);
    gfx->setTextColor(WHITE, BLACK);
    gfx->println("---------"); // fill with dashes to clear line
    if (pressure < 30)
    {
        gfx->setCursor(10, 180);
        gfx->setTextColor(RED, BLACK); // yellow text with black background to overwrite
        gfx->println("oo-------");
    }
    else if (pressure >= 30 && pressure < 55)
    {
        gfx->setCursor(10, 180);
        gfx->setTextColor(YELLOW, BLACK); // yellow text with black background to overwrite
        gfx->println("oooo-----");
    }
    else if (pressure >= 55 && pressure < 64)
    {
        gfx->setCursor(10, 180);
        gfx->setTextColor(YELLOW, BLACK);
        gfx->println("oooooo---");
    }
    else if (pressure >= 64)
    {
        gfx->setCursor(10, 180);
        gfx->setTextColor(GREEN, BLACK);
        gfx->println("ooooooooo");
    }
    gfx->setCursor(10, 140);
    gfx->println("---------"); // fill with dashes to clear line
    gfx->setCursor(10, 140);
    gfx->println(pressure);
    gfx->setTextColor(WHITE, BLACK); // set text color back to default
}
int zero()
{
    scale.set_scale(calibration_factor);
    scale.tare();
    delay(50);
    return 0;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

