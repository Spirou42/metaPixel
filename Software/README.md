#Software
## Building
The metaPixel Software uses the Arduino build environment, Teensyduino to add Teensy 3.1 support and a couple of third party libraries.

### Tools
1. [Arduino](https://www.arduino.cc/en/Main/Software) environment
2. [Teensyduino](http://www.pjrc.com/teensy/teensyduino.html)

### Libraries
1. [AVRQueue](https://github.com/Zuph/AVRQueue) - a simple task switching library. 
2. [Streaming](http://arduiniana.org/libraries/streaming/) - C++ Stream operators for the Arduino serial  class
3. [FastLED](http://fastled.io) - LED Driver Library 

### Files
*metaPixel.ino* is the main source file and *metaPixel.h* contains all global definitions, defines and includes.



## Structural Overview
The work horse of the metaPixel control software is the task switcher of the Queue library [**taskQueue**]. All processes such as Effects, command line interface, UI SubSystem (current implementation is outdated) and the DMX interface are implemented either interrupt based or 