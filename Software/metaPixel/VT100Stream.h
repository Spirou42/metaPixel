/**
* VT100Stream.h
*
* simple streaming extension for sending VT100-Escape sequences
*/

#ifndef __VT100_EXTENTION_H__
#define __VT100_EXTENTION_H__

#include "Arduino.h"
#include "Streaming.h"
//#define ESC (char)0x1b
/// Simple sequences
enum _VT100Simples {
  esc,            ///< Escape
  clearScreen,    ///< ClearScreen
  clearHome,      ///< clear Home
  cursorHome,     ///< cursor Home
  clearLineRight, ///< clear line right of cursor
  bold,           ///< bold on
  light,          ///< light Mode
  normal,         ///< normal Mode
  underline,      ///< underline
  reverse,        ///<reverse
  blink,          ///<blinking

};

/// posioning helper
class ScreenPos{
public:
  uint8_t line;
  uint8_t column;
  ScreenPos():line(0),column(0){};
  ScreenPos(uint8_t l, uint8_t c):line(l),column(c){};
};

inline Print &operator <<(Print& obj, ScreenPos k){
  obj<<(char)0x1b<<'['<<k.line<<';'<<k.column<<'H';
  return obj;
}

inline Print &operator <<(Print& obj,_VT100Simples control)
{
  switch(control){
  case esc:             obj<<(char)0x1b; break;
  case clearScreen:     obj<<(char)0x1b<<"[2J"; break;
  case clearHome:       obj<<(char)0x1b<<"[2J"<<(char)0x1b<<"[H";break;
  case cursorHome:      obj<<(char)0x1b<<"[H";break;
  case clearLineRight:  obj<<(char)0x1b<<"[K"; break;
  case bold:            obj<<(char)0x1b<<"[1m";break;
  case light:           obj<<(char)0x1b<<"[2m";break;
  case normal:          obj<<(char)0x1b<<"[0m";break;
  case underline:       obj<<(char)0x1b<<"[4m";break;
  case reverse:         obj<<(char)0x1b<<"[7m";break;
  case blink:           obj<<(char)0x1b<<"[5m";break;
  default:
    break;
  }
  return obj;
}
#endif
