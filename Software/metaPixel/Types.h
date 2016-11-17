/**
PlasmaTypes / types.h
Legacy file, the contents will be moced to other files soon
*/
#ifndef __PLASMATYPES_H__
#define __PLASMATYPES_H__
#include "Effect.hpp"
#include <Queue.h>
#include "Parameter.hpp"

typedef enum {
	NoEffect = 0,
	HorizontalEffect = 1,
	VerticalEffect = 2,
	DiagonalEffect = 4,
	CircleEffect = 8,
}plasmaEffectMask_t;

typedef struct {
	const char* id;
	queuedFunction function;
	uint16_t delay;
	void* userData;
}effectProgram_t;


typedef struct {
	Effect* program;
	uint16_t delay;
	const char *parameter;
}effectProgramN_t;


typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t macro;
	uint8_t strobe;
	uint8_t mode;
	uint8_t master;
}DMXChannels_t;

#endif
