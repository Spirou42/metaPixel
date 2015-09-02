/**
*Commands.h
* simple command line processor
*/

#ifndef __COMMANDS_H__
#define __COMMANDS_H__
#include "Parameter.h"
#define SERIAL_BUFFER_LENGTH 80
//** reads lines from Serial
extern int serialReader(unsigned long now, void* userData);
extern int getParameterIdxFor(char p);
extern Parameter16_t * getParameterFor(char p);
//* processes lines
extern void commandProcessor(char* line_buffer,bool executeImediately = false);
//* line buffer
extern char serial_buffer[SERIAL_BUFFER_LENGTH];



typedef enum _commandType {
  commandParameter,
  commandAnimation,
  commandWait,
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
      case commandWait:
        out << "Wait for "<<command->data.commandWaitData.time/1000;
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
  CommandQueue():queueStart(NULL),queueEnd(NULL),queueLength(0){};
  void addCommand(metaPixelCommand* cmd);
  metaPixelCommand* popCommand();
  void processQueue();
  friend Print& operator<<(Print& obj, CommandQueue& comQ)
  {
    if(comQ.waiting){
        obj<<clearLineRight<<"Waiting "<<(comQ.waitTill - comQ.waitTimer)/1000<<endl;
    }
    metaPixelCommand *l = comQ.queueStart;
    while(l){
      Serial <<clearLineRight<< l << endl;
      l = l->nextCommand;
    }
    return obj;
  };
};
#endif
