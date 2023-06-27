#include <Adafruit_ST7789.h>
#include <Adafruit_ST77xx.h>
#include <ArduinoGraphics.h>
#include <Arduino_GFX_Library.h>
#include <stdio.h>
#include "stdlib.h"
#include "hardware/adc.h"
#include <string.h>
#define GFX_BL 13 // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
#include <Arduino_GFX_Library.h>
#define TFT_BL 13
//(DC, CS, SCK, MOSI, DIN, SPI)
Arduino_DataBus *bus = new Arduino_RPiPicoSPI(8, 9, 10, -1, 11, spi1);
//(bus, RST, rotation, ips, width, height)
Arduino_GFX *gfx = new Arduino_ST7789(bus, 12, 3, true, 240, 320);

#include "HX711.h"
#include <SPI.h>

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
float calibration_factor = -466;
HX711 scale;

const int calibPin = 5; // the number of the pushbutton pin
const int zeroPin = 4;
int calibState = 0;     // current state of the button
int lastCalibState = 0; // previous state of the button
int zeroState = 0;
int lastZeroState = 0;
int pressureAnalogPin = 26;
int lastPressure;
long lastReading = 0;
int reading;
int highest;
int lastHighest;
const int numPressure = 10;

int pressure[numPressure]; // the pressure from the analog input
int readIndex = 0;         // the index of the current Pressure
int totalPressure = 0;     // the running totalPressure
int averagePressure = 0;   // the averagePressure

void setup(void)
{
    gfx->begin();
    gfx->fillScreen(BLACK);
    gfx->setTextSize(5);
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    pinMode(calibPin, INPUT);
    pinMode(zeroPin, INPUT);
    pinMode(pressureAnalogPin, INPUT);
    scale.set_scale(calibration_factor); // Adjust to this calibration
#ifdef GFX_BL
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
#endif

    gfx->setCursor(10, 10);
    gfx->setTextColor(WHITE, BLACK);
    // initialize all the pressure to 0:
    for (int thisPressure = 0; thisPressure < numPressure; thisPressure++)
    {
        pressure[thisPressure] = 0;
    }
    delay(5000); // 5 seconds
}

void loop()
{
    calibState = digitalRead(calibPin);
    zeroState = digitalRead(zeroPin);
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
    // save the current state as the last state, for next time through the loop
    lastCalibState = calibState;
    lastZeroState = zeroState;
    if (reading > highest)
    {
        highest = reading;
    }
    if (highest != lastHighest)
    {
        drawHighest(highest);
    }
    if (reading != lastReading)
    {
        drawReading(reading);
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
    delay(1); // delay in between reads for stability
    drawPressure(averagePressure);
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

    gfx->setTextSize(3);
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->println("Remove all weight from \n scale then press \n calibrate");
    buttonWait(5);
    delay(50);
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->println("Zeroing...");
    delay(50);
    scale.set_scale();
    scale.tare(); // Reset the scale to 0
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->println("Place 200g weight \n on scale then \n press calibrate.");
    buttonWait(5);
    delay(50);
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->println("Calibrating....");
    long current = scale.get_units(10);
    calibration_factor = current / 200;
    scale.set_scale(calibration_factor);
    delay(50);
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->println("Done!");
    highest = 0;
    delay(50);
    gfx->setTextSize(5);
    delay(100);
}

void drawHighest(int highest)
{

    gfx->setCursor(10, 10);
    gfx->println("---------");
    gfx->setCursor(10, 10);
    gfx->println(highest);
}
void drawReading(int reading)
{
    gfx->setCursor(10, 80);
    gfx->println("---------");
    gfx->setCursor(10, 80);
    gfx->println(reading);
}
void drawPressure(int pressure)
{
    if (pressure < 250)
    {
        gfx->setTextColor(YELLOW, BLACK);
    }
    else if (pressure >= 250 && pressure < 500)
    {
        gfx->setTextColor(GREEN, BLACK);
    }
    else
    {
        gfx->setTextColor(RED, BLACK);
    }
    gfx->setCursor(10, 150);
    gfx->println("---------");
    gfx->setCursor(10, 150);
    gfx->println(pressure);
    gfx->setTextColor(WHITE, BLACK);
}
int zero()
{
    scale.set_scale();
    scale.tare();
    delay(50);
    return 0;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/