#Software
Here, you'll find a basic description on how the metaPixel Software can be build and how the Software works. 

## Building
The metaPixel Software uses the Arduino build environment, Teensyduino to add Teensy 3.1 support and a couple of third party libraries.

### Tools
1. [Arduino](https://www.arduino.cc/en/Main/Software) environment
2. [Teensyduino](http://www.pjrc.com/teensy/teensyduino.html)

### Libraries
1. [AVRQueue](https://github.com/Zuph/AVRQueue)  
a simple task switching library. 
2. [Streaming](http://arduiniana.org/libraries/streaming/)  
C++ Stream operators for the Arduino serial class
3. [FastLED](http://fastled.io)  actually you have to use my [Version of FastLED](https://github.com/Spirou42/FastLED) 
LED Driver Library with nice and helpful features. 

## Overview
`metaPixel.h` and `metaPixel.ino` are the main files of the metaPixel controller software. `metaPixel.h` contains all definitions, prototypes, defines and includes.

###Files
* **Commands**  
Serial line receiver and parser, commands and command queue 

* **Effect**  
Base class for all Effects.  

* **Effect...**  
The Effects itself  

* **Encoder**  
Legacy rotary encoder handler for **`Menu`**

* **Menu**  
Legacy 7Segment menu UI.

* **metaDisplay**  
The main class for metaPixel Display handling control, handles all display related activities like colouring pixels, drawing lines a.s.o. 

* **metaModule**  
A coordinate mapper for a single module used by **metaDisplay** to map `x,y` -> `idx`  

* **Palettes**  
A couple of Palette definitions extracted with FastLEDs [PaletteKnife](http://fastled.io/tools/paletteknife/) tool from [cpt-city](http://soliton.vm.bytemark.co.uk/pub/cpt-city/) 

* **Parameter**  
A couple of interrupt changeable animatable *int16_t* values.

* **Types**  
The content of this file will move to different places in the future

* **VT100Stream**  
Some extensions for streaming VT100 control chars.

### Components / Infrastructure
The foundation of the metaPixel control software is composed of a couple of `Parameter16_t` objects holding all the global parameters controlling the softwares behaviour and the `taskQueue` as a main function dispatcher. 



