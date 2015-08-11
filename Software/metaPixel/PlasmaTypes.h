
#ifndef __PLASMATYPES_H__
#define __PLASMATYPES_H__

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
	effectProgram_t program;
	float seconds;
}TimelineEntry_t;

typedef struct {
	boolean shouldMirrow;
	boolean shouldAnimateScale;
	int16_t scale;
	int16_t wBase,hBase;
	uint8_t frameSteps;
}PlasmaData_t;
#endif

