/**
*Commands.h
* simple command line processor
*/

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "Arduino.h"
#include "Parameter.h"
#include "Streaming.h"
#include "VT100Stream.h"
extern const char* macroNames[];
#define SERIAL_BUFFER_LENGTH 512
//** reads lines from Serial

#define ANIMATE_COMMAND   '@'
#define BOUNCE_COMMAND    '~'
#define STOP_COMMAND      '*'
#define WAITTIME_COMMAND  '&'
#define WAITANIM_COMMAND  '%'
#define DUMP_COMMAND      '?'
#define MACRO_COMMAND     '#'

typedef enum _commandType {
  commandParameter,
  commandAnimation,
  commandDump,
  commandWait,
  commandMacro,
} commandType_t;

typedef struct _parameterSet
{
  Parameter16_t *parameter;
  int16_t value;
} CommandParameterSet_t;

typedef struct _parameterAnimation
{
  Parameter16_t *parameter;
  int16_t fromValue;
  int16_t toValue;
  unsigned long timeToGo;
  bool bounce;
}CommandParameterAnimation_t;

typedef struct _CommandWait
{
  unsigned long time;
  bool waitForAnimationStop;
  Parameter16_t *parameter;
}CommandWait_t;

typedef union _commandData
{
  CommandParameterSet_t parameterSetData;
  CommandParameterAnimation_t parameterAnimationData;
  CommandWait_t commandWaitData;
  uint16_t macroIndex;
}CommandData_t;

class metaPixelCommand
{
public:
  metaPixelCommand* nextCommand;        // pointer to
  commandType_t   type;
  CommandData_t data;
  metaPixelCommand(commandType_t t):type(t),data(){};
  bool processCommand();
  friend Print& operator<<(Print& out,metaPixelCommand* command){
    out << clearLineRight<< " [";
    switch(command->type){
      case commandParameter:
        out << "Set "<<command->data.parameterSetData.parameter->code<<" to "<<command->data.parameterSetData.value; break;
      case commandAnimation:
        {
          CommandParameterAnimation_t dd = command->data.parameterAnimationData;
          const char *f,*m,*l;

          if(dd.bounce){
            f = "Bounce ";
            m = " between ";
            l = " and ";
          }else{
            f = "Animate ";
            m = " from ";
            l = " to ";
          }
          out << f<<dd.parameter->code<<m<<dd.fromValue<<l<<dd.toValue;
        }
        break;
      case commandDump:
        {
          out << "Dump";
        }
        break;
      case commandWait:
        if(command->data.commandWaitData.parameter){
          out << "Wait for parameter"<<*(command->data.commandWaitData.parameter);
        }else{
          out << "Wait for "<<command->data.commandWaitData.time/1000;
        }
        break;
      case commandMacro:
        out << "Macro "<< macroNames[command->data.macroIndex];
        break;
    }
    out <<" ]";
    return out;
  }
};

class CommandQueue
{
public:
  metaPixelCommand* queueStart;
  metaPixelCommand* queueEnd;
  size_t            queueLength;
  elapsedMillis     waitTimer;
  bool              waiting;
  unsigned long     waitTill;
  Parameter16_t     *waitParameter;
  CommandQueue():queueStart(NULL),queueEnd(NULL),queueLength(0){};
  void addCommand(metaPixelCommand* cmd, bool atBeginning = false);
  void addCommands(CommandQueue *otherQueue, bool atBeginning = false);
  metaPixelCommand* popCommand();
  void clearQueue();
  void processQueue();
  friend Print& operator<<(Print& obj, CommandQueue& comQ)
  {
    if(comQ.waiting){
      if(!comQ.waitParameter){
        obj<<clearLineRight<<"Waiting "<<(comQ.waitTill - comQ.waitTimer)/1000<<endl;
      }else{
        obj<<clearLineRight<<"Waiting for animation on ["<<(comQ.waitParameter->code)<<"] to end"<<endl;
      }
    }
    metaPixelCommand *l = comQ.queueStart;
    int8_t k = 0;
    while(l &&(++k < 30)){
      Serial <<clearLineRight<< l << endl<<clearLineRight;
      l = l->nextCommand;
    }
    if(l){
      Serial <<clearLineRight<< " ..." << endl<<clearLineRight;
    }
    Serial <<clearLineRight<< endl<<clearLineRight;
    return obj;
  };
};


extern int serialReader(unsigned long now, void* userData);
extern int getParameterIdxFor(char p);
extern Parameter16_t * getParameterFor(char p);
//* processes lines
extern void commandProcessor(const char* line_buffer, bool executeImediately = false);
//* line buffer
extern char serial_buffer[SERIAL_BUFFER_LENGTH];

#endif
