#ifndef __CUBE_H_
#define __CUBE_H_
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

enum DemoStatus
{
    Colourful_Egg,Weather_Humidity,Time
};
void SetVars();
void ProcessLine(struct Line2d *ret,struct Line3d vec);
void StartColourfulEgg();
void RenderImage(void);

#endif