#Software
Here, you'll find a basic description on how the metaPixel Software can be build and how the Software works.

## Building
The metaPixel Software uses the Arduino build environment, Teensyduino to add Teensy 3.1 support and a couple of third party libraries.

### Tools
1. [Arduino](https://www.arduino.cc/en/Main/Software) environment
2. [Teensyduino](http://www.pjrc.com/teensy/teensyduino.html)

### Libraries
1. [my version](https://github.com/Spirou42/AVRQueue) of [AVRQueue](https://github.com/Zuph/AVRQueue)  
a simple task switching library. metaPixel uses this library to implement all concurrent tasks like Backbuffer blending, Effect rendering and command execution.
2. [my version](https://github.com/Spirou42/ILI9341_t3) of [ILI9341_3](https://github.com/PaulStoffregen/ILI9341_t3) 
3. [Streaming](http://arduiniana.org/libraries/streaming/)  
C++ Stream operators for the Arduino serial class. Simply because it is easier to read and type than Arduinos `println`
4. [FastLED](http://fastled.io)  actually you have to use [my Version of FastLED](https://github.com/Spirou42/FastLED)
LED Driver Library. FastLED is a very neat LED handling library with a lot of well designed features. It supports different type of 'intelligent' LEDs and LED controllers, provides a lot of fast fixed point arithmetic and support RGB as well as HSV color space.     

## Overview
`metaPixel.h` and `metaPixel.ino` are the main files of the metaPixel controller software. `metaPixel.h` contains all definitions, prototypes, defines and includes.

###Files
* **Commands**
Serial line receiver and parser, commands, command queue and interpreter

* **Effect**
Base class for all Effects.  

* **Effect...**
The Effects itself  

* **metaDisplay**
The main class for handling the LED display. This class responsible for all display related activities like colouring pixels, drawing lines a.s.o.. In addition the `metaDisplay` class ist

* **metaModule**
A coordinate mapper for a single module used by `metaDisplay` to map `(x,y)` coordinates to a frame buffer index.

* **Palettes**
A couple of Palette definitions extracted with FastLEDs [PaletteKnife](http://fastled.io/tools/paletteknife/) tool from [cpt-city](http://soliton.vm.bytemark.co.uk/pub/cpt-city/)

* **Parameter**
A couple of interrupt changeable, animatable `int16_t` values. This class provides the basic infrastructure for managing and mapping parameters.

* **Types**
The content of this file will move to different places in the future. Actually it contains bunch of structure and type definitions.

* **VT100Stream**
Some extensions for streaming VT100 control chars.

## Controlling the Display

metaPixel is a simple state machine controlled by a couple of `int16_t` values encapsulated into a `Parameter16_t` class providing simple animation capabilities.

Fifteen `Parameter16_t` values are actually implemented and used to control global and effect local functions.

<table>
<tr><th>Index</th><th>Parameter Name</th><th>Range</th><th>Description</th></tr>
<tr><th colspan='4' align='left'>System Parameter</th></tr>
<tr><td align="center">0</td><td align="center">P</td><td align="center">0 - 6</td><td>Effect Program</td></tr>
<tr><td align="center">1</td><td align="center">D</td><td align="center">1 - 5000</td><td>Effect Frame Delay</td></tr>
<tr><td align="center">2</td><td align="center">C</td><td align="center">0 - 11</td><td>Palette</td></tr>
<tr><td align="center">3</td><td align="center">B</td><td align="center">0 - 255</td><td>LED Brightness</td></tr>
<tr><td align="center">4</td><td align="center">Q</td><td align="center">0 - 5</td><td>Mirror Mode</td></tr>
<tr><td align="center">5</td><td align="center">Z</td><td align="center">1 - 14</td><td>Blend factor</td></tr>
<tr><td align="center">6</td><td align="center">A</td><td align="center">0 - 1</td><td>Display Resoultion</td></tr>
<tr><th colspan='4' align='left'>Effect Parameter</th></tr>
<tr><td align="center">7</td> <td align="center">U</td><td align="center">-</td><td>Effect Speed 1</td></tr>
<tr><td align="center">8</td> <td align="center">V</td><td align="center">-</td><td>Effect Speed 1</td></tr>
<tr><td align="center">9</td> <td align="center">R</td><td align="center">-</td><td>Effect Scale 1</td></tr>
<tr><td align="center">10</td><td align="center">I</td><td align="center">-</td><td>Effect Scale 2</td></tr>
<tr><td align="center">11</td><td align="center">O</td><td align="center">-</td><td>Effect Parameter 1</td></tr>
<tr><td align="center">12</td><td align="center">H</td><td align="center">-</td><td>Effect Parameter 1</td></tr>
<tr><td align="center">13</td><td align="center">M</td><td align="center">0 - 255</td><td>Effect Mask 1</td></tr>
<tr><td align="center">14</td><td align="center">N</td><td align="center">0 - 255</td><td>Effect Mask 2</td></tr>
</table>

### Serial Interface
The metaPixel display is controlled by a simple serial command line interface. Connect your computer to the USB-Serial with a VT100 capable terminal program and press `Return`

	Effect: Noise
	Parameter:
	nScale    [R] 35 (255)        nSpeed    [U]*5 (255)         hSpeed    [V] 253 (255)
	nMask     [M] 1 (7)

	Program   [P] 1 (6)           Delay     [D] 100 (5000)      Palette   [C] 8 (11)
	Bright    [B] 160 (255)       Mirror    [Q] 0 (5)           Fade      [Z] 3 (14)
	Res       [A] 1 (1)

	>

From top to bottom you'll see:
* the name of the currently running Effect
* the effect parameters
* the system parameters

Each parameter entry consist of 5 parts
1. Descriptive Name
2. Parameter Name in `[]`
3. current value
4. optional animation marker `*`
5. current max value in `()`

To change a value simply type the parameter name followed by the new value. For example sending `P5` or `p5` will change the current effect to **Fire**
You can combine as many value changes in a single line as you wish as long as the line length did not exceed 512 bytes.

#### Additional Commands
In addition setting a single value to a effect parameter there is a couple of 'control' commands implemented.

<table>
<tr><th>Function</th><th>Command</th><th>Format</th></tr>
<tr><td>Bounce</td><td>~</td><td>~[paraName][startValue] [endValue] [time]</td></tr>
<tr><td colspan=2></td><td >Bounces the value of [paraName] between [startValue] and [endValue] in [time] sec.</td></tr>
<tr><td>Animate</td><td>@</td><td>@[paraName][startValue] [endValue] [time]</td></tr>
<tr><td colspan=2></td><td>Animates the value of [paraName] from [startValue] to [endValue] in [time] sec.</td></tr>
<tr><td>Stop Animate</td><td>*</td><td>*[paraName]</td></tr>
<tr><td colspan=2></td><td>Stops any animation on parameter [paraName]</td></tr>
<tr><td>Wait</td><td>&</td><td>&[time]</td></tr>
<tr><td colspan=2></td><td>Stops command processing for [time] sec.</td></tr>
<tr><td>Wait for Animation</td><td>%</td><td>%[paraName]</td></tr>
<tr><td colspan=2></td><td>Stops command processing until animation is finished. If Bounce is enabled on [paraName] this command will wait until the next limit (up or down) is reached.</td></tr>
<tr><td>Dump</td><td>?</td><td>?</td></tr>
<tr><td colspan=2></td><td>dumps the current parameter set as a command line.</td></tr>
<tr><td>Macro</td><td>#</td><td>#[macroNumber]</td></tr>
<tr><td colspan=2></td><td>call the hard coded macro with the given number. Macros are simple hard coded strings containing a bunch of parameter changes and commands to control a effect sequence. It is allowed to build circular references.</td></tr>

</table>
