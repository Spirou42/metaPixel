/**
* GraphicTests - a collection of simple test routines for the metaTFTDisplay enviroment
*/

#ifndef __GRAPHICTEST_H__
#define __GRAPHICTEST_H__

#include "metaTFTDisplay.h"
extern metaTFT tft;
unsigned long testFillScreen();
unsigned long testText();
unsigned long testLines(uint16_t color);
unsigned long testFastLines(uint16_t color1, uint16_t color2);
unsigned long testRects(uint16_t color);
unsigned long testFilledRects(uint16_t color1, uint16_t color2);
unsigned long testFilledCircles(uint8_t radius, uint16_t color);
unsigned long testCircles(uint8_t radius, uint16_t color);
unsigned long testTriangles();
unsigned long testFilledTriangles();
unsigned long testRoundRects();
unsigned long testFilledRoundRects();
void testThings();
#endif
