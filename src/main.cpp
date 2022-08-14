#include <Arduino.h>
#include <NeoPixelBus.h>
#include "HomeSpan.h"
#include "extras/Pixel.h"

const uint8_t fixturesCount = 6;
const uint8_t pixelsPerFixture = 12;
const uint8_t totalPixelCount = fixturesCount * pixelsPerFixture;

// For testing
// int fixture1Indices[pixelsPerFixture] = {0};
// int fixture2Indices[pixelsPerFixture] = {1};
// int fixture3Indices[pixelsPerFixture] = {2};
// int fixture4Indices[pixelsPerFixture] = {3};
// int fixture5Indices[pixelsPerFixture] = {4};
// int fixture6Indices[pixelsPerFixture] = {5};

int fixture1Indices[pixelsPerFixture] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
int fixture2Indices[pixelsPerFixture] = {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
int fixture3Indices[pixelsPerFixture] = {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
int fixture4Indices[pixelsPerFixture] = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
int fixture5Indices[pixelsPerFixture] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59};
int fixture6Indices[pixelsPerFixture] = {60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71};

RgbwColor pixelColors[totalPixelCount];

const uint8_t pixelPin = 25;
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(totalPixelCount, pixelPin);

struct NeoPixel_RGBW : Service::LightBulb
{
  Characteristic::On power{0, true};
  Characteristic::Hue H{0, true};
  Characteristic::Saturation S{0, true};
  Characteristic::Brightness V{100, true};
  int *pixelIndices;

  NeoPixel_RGBW(int pixelIndices[]) : Service::LightBulb()
  {
    // sets the range of the Brightness to be from a min of 1%, to a max of 100%, in steps of 1%
    V.setRange(1, 100, 1);
    this->pixelIndices = pixelIndices;
    // manually call update() to set pixel with restored initial values
    update();
  }

  boolean update() override
  {
    int p = power.getNewVal();

    float val = V.getNewVal<float>(); // range = [0,100]
    float hue = H.getNewVal<float>(); // range = [0, 360]
    float sat = S.getNewVal<float>(); // range = [0, 100]

    for (uint8_t i = 0; i < pixelsPerFixture; i++)
    {
      HsbColor c = HsbColor(hue / 360, sat / 100, (val * p) / 100);
      pixelColors[pixelIndices[i]] = RgbwColor(c);
    }

    return true;
  }
};

void initFixture(const char *name, int pixelIndices[])
{
  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();
  new Characteristic::Name(name);
  new NeoPixel_RGBW(pixelIndices);
}

void setup()
{
  strip.Begin();
  strip.Show();

  Serial.begin(115200);

  for (int i = 0; i < totalPixelCount; i++)
  {
    // pixelColors[i] = pixel->HSV(0, 0, 0, 0);
    pixelColors[i] = RgbwColor(0, 0, 0, 0);
  }

  homeSpan.begin(Category::Bridges, "RVLights");

  // Bridge accessory with no Services other than Accessory Information
  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();

  initFixture("Sink Light", fixture1Indices);
  initFixture("Stove Light", fixture2Indices);
  initFixture("Bed Front Passenger", fixture3Indices);
  initFixture("Bed Rear Passenger", fixture4Indices);
  initFixture("Bed Rear Driver", fixture5Indices);
  initFixture("Bed Front Driver", fixture6Indices);
}

void loop()
{
  homeSpan.poll();

  // apply the color to the strip
  for (uint8_t pixel = 0; pixel < totalPixelCount; pixel++)
  {
    strip.SetPixelColor(pixel, pixelColors[pixel]);
  }
  strip.Show();
}
