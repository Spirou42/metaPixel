/**
*Commands.h
* simple command line processor
*/

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#define SERIAL_BUFFER_LENGTH 80
//** reads lines from Serial
extern int serialReader(unsigned long now, void* userData);
extern int getParameterFor(char p);
//* processes lines
extern void commandProcessor(char* line_buffer);
//* line buffer
extern char serial_buffer[SERIAL_BUFFER_LENGTH];


#endif
