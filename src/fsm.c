#include "fsm.h"

static enum {
	Mode_None,
	Mode_Manual, 
	Mode_Auto
} mode = Mode_None;

////////////////////////////////////////////////////////////////////////////////
// manual operation mode
////////////////////////////////////////////////////////////////////////////////
typedef enum{ 		// TODO: define states for manual mode here
	ruhe, schliessen, oeffnen // = Geschlossen
} ManuState_t;


typedef union {// TODO: define an input type for manual mode here

	struct {		 //brauchen wir da lb? komplett raus oder?
		unsigned char offen: 1; 		// either approximation or collision (or both) light barriers triggered		
		unsigned char oeff: 1;	// door is closed		
		unsigned char geschlossen: 1;		// door is open	
		unsigned char schliessen: 1; 		// either approximation or collision (or both) light barriers triggered			
		unsigned char _reserved: 4;	// fill remaining bits to prevent compiler "optimization" writing garbage into them
	} single;
	unsigned char all;
} ManuInputs_t;


static const struct { // TODO: insert FSM table for manual mode here
	ManuState_t nextState;	
	Actuators_t output;
} ManuTable[3][16] = {
	{	//Ruhe
		{ruhe,{{0,0,0}}/*Fehler*/},{ruhe,{{0,0,0}}/*Fehler*/}, {ruhe, {{0,0,0}}}, {ruhe, {{0,0,0}}},
		{ruhe,{{0,0,0}}/*Fehler*/},{ruhe,{{0,0,0}}/*Fehler*/}, {oeffnen, {{0,0,1}}}, {oeffnen, {{0,0,1}}},
		{ruhe, {{0,0,0}}}, {schliessen, {{1,1,0}}}, {ruhe,{{0,0,0}}/*Fehler*/},{ruhe,{{0,0,0}}/*Fehler*/},
		{ruhe, {{0,0,0}}}, {ruhe, {{0,0,0}}}, {ruhe,{{0,0,0}}/*Fehler*/},{ruhe,{{0,0,0}}/*Fehler*/}
	}	,
	 //schliessen
	{
		{schliessen, {{1,1,0}}}, {schliessen, {{1,1,0}}}, {ruhe, {{0,0,0}}}, {ruhe, {{0,0,0}}},
		{oeffnen, {{0,0,1}}}, {oeffnen, {{0,0,1}}}, {oeffnen, {{0,0,1}}}, {oeffnen, {{0,0,1}}},
		{schliessen, {{1,1,0}}}, {schliessen, {{1,1,0}}}, {schliessen, {{0,0,0}}/*Fehler*/},{schliessen, {{0,0,0}}/*Fehler*/},
		{ruhe, {{0,0,0}}}, {ruhe, {{0,0,0}}}, {schliessen, {{0,0,0}}/*Fehler*/},{schliessen, {{0,0,0}}/*Fehler*/}
	}	,
	//oeffnen
	{
		{oeffnen, {{0,0,1}}}, {oeffnen,{{0,0,0}}/*Fehler*/},{oeffnen, {{0,0,1}}}, {oeffnen, {{0,0,1}}},
		{oeffnen, {{0,0,1}}}, {oeffnen, {{0,0,1}}}, {oeffnen, {{0,0,1}}}, {oeffnen, {{0,0,1}}},
		{ruhe, {{0,0,0}}}, {oeffnen,{{0,0,0}}/*Fehler*/},{oeffnen,{{0,0,0}}/*Fehler*/},{oeffnen,{{0,0,0}}/*Fehler*/},
		{ruhe, {{0,0,0}}}, {ruhe, {{0,0,0}}}, {oeffnen,{{0,0,0}}/*Fehler*/},{oeffnen,{{0,0,0}}/*Fehler*/}
	}
};


static Actuators_t doManualCycle(Sensors_t sensors, int doInit) {
	Actuators_t actuators;
	ManuInputs_t inputs;
	static ManuState_t state;
	static int closeDelayCounter;
		
	if(doInit) {
		closeDelayCounter = 0;
		state = ruhe;
	} 

	// TODO: fill input structure
	inputs.all = 0;
	if(sensors.single.limit_open) inputs.single.offen = 1;
	if(sensors.single.limit_closed) inputs.single.geschlossen = 1;
	if(sensors.single.button_open)inputs.single.oeff=1;
	if(sensors.single.button_close)inputs.single.schliessen=1;
	
	

	// TODO: table based transition and output function	
	actuators = ManuTable[state][inputs.all].output;
	state = ManuTable[state][inputs.all].nextState;	

		
	return actuators; 	
} 




////////////////////////////////////////////////////////////////////////////////
// automatic operation mode
////////////////////////////////////////////////////////////////////////////////

// states
typedef enum{
	Bew_ohneLS, Lichtschranke, // TODO: add states here
	Ruhe,
	Auto_Initial // = Geschlossen
} AutoState_t;

// These are the FSM inputs (do not mix up with system input type Sensors_t!)
typedef union {
	struct {		
		unsigned char lb: 1; 		// either approximation or collision (or both) light barriers triggered		
		unsigned char closed: 1;	// door is closed		
		unsigned char open: 1;		// door is open		
		unsigned char _reserved: 5;	// fill remaining bits to prevent compiler "optimization" writing garbage into them
	} single;
	unsigned char all;
} AutoInputs_t;

static const struct { 
	AutoState_t nextState;	
	Actuators_t output;
} autoTable[4]/*Anzahl Zustände*/ [8]/*Anzahl Eingangsvektoren*/ = {
	//Bew_ohneLS
	{{Bew_ohneLS,{{1,1,0}}},		{Bew_ohneLS,{{0,0,1}}},			{Ruhe,{{0,0,0}}}, 			{Bew_ohneLS,{{0,0,0}}},/*Fehler*/		{Ruhe,{{0,0,0}}},		{Lichtschranke,{{0,0,0,}}},	{Bew_ohneLS,{{0,0,0}}},/*Fehler*/	{Bew_ohneLS,{{0,0,0}}}/*Fehler*/},
	//Lichtschranke
	{{Bew_ohneLS,{{0,0,1}}},		{Lichtschranke,{{0,0,1}}},		{Lichtschranke,{{0,0,1}}}, 	{Lichtschranke,{{0,0,0}}},/*Fehler*/	{Ruhe,{{0,0,0}}},		{Lichtschranke,{{0,0,0}}},	{Lichtschranke,{{0,0,0}}},/*Fehler*/{Lichtschranke,{{0,0,0}}}/*Fehler*/},
	//Ruhe
	{{Ruhe,{{0,0,0}}},/*Fehler*/ 	{Ruhe,{{0,0,0}}},/*Fehler*/		{Ruhe,{{0,0,0}}},			{Lichtschranke,{{0,0,1}}},				{Bew_ohneLS,{{1,1,0}}},	{Lichtschranke,{{0,0,0}}},	{Ruhe,{{0,0,0}}},/*Fehler*/			{Ruhe,{{0,0,0}}}/*Fehler*/},
	//Auto_Initial
	{{Bew_ohneLS,{{0,0,1}}},		{Lichtschranke,{{0,0,1}}},		{Ruhe,{{0,0,1/*hier*/}}},			{Lichtschranke,{{0,0,1}}},				{Ruhe,{{0,0,1/*hier*/}}},		{Lichtschranke,{{0,0,1/*hier*/}}},	{Auto_Initial,{{0,0,0}}},/*Fehler*/	{Auto_Initial,{{0,0,0}}}/*Fehler*/}
};

#define DOOR_CLOSE_DELAY	(5000 / FSM_INTERVAL_MS) // wait 5 seconds before closing the door

static Actuators_t doAutoCycle(Sensors_t sensors, int doInit) {
	Actuators_t actuators;
	AutoInputs_t inputs;
	static AutoState_t state;
	static int closeDelayCounter;
		
	if(doInit) {
		closeDelayCounter = 0;
		state = Auto_Initial;
	} 
	
	// fill input structure
	inputs.all = 0;
	if(sensors.single.limit_open) inputs.single.open = 1;
	if(sensors.single.limit_closed) inputs.single.closed = 1;
	if(sensors.single.lb_approximation || sensors.single.lb_collision) inputs.single.lb = 1;
	
	// table based transition and output function	
	actuators = autoTable[state][inputs.all].output;
	state = autoTable[state][inputs.all].nextState;	
	
	// door close delay
	if(actuators.single.motor_close) {
		if(closeDelayCounter > 0) {
			closeDelayCounter--;
			actuators.single.motor_close = 0;
			actuators.single.warn_lamp = 0;
		} 
	} else closeDelayCounter = DOOR_CLOSE_DELAY;
		
	return actuators;
} 

////////////////////////////////////////////////////////////////////////////////
// Error Detection
////////////////////////////////////////////////////////////////////////////////

static unsigned char errorMask;

#define SENSOR_ERROR_DELAY (2000 / FSM_INTERVAL_MS) 

static unsigned char detectErrors(Sensors_t sensors, Actuators_t actuators) {
	unsigned char mask = 0;
	
	if(sensors.single.limit_open && sensors.single.limit_closed) mask |= 0x1;  
	if((sensors.single.limit_closed || sensors.single.limit_open) && (actuators.single.motor_close||actuators.single.motor_open)) mask |= 0x2; //Endlage, aber eine Motor ist an

	// TODO: handle more errors here
	
	return mask;
}

////////////////////////////////////////////////////////////////////////////////
// Top Level State Machine
////////////////////////////////////////////////////////////////////////////////

void FSM_Init() {
	mode = Mode_None;
	errorMask = 0;
}

Actuators_t FSM_Cycle(Sensors_t sensors) {	
	Actuators_t actuators;
	if(sensors.single.mode_auto){
		// automatic operating mode
		actuators = doAutoCycle(sensors, (mode != Mode_Auto));
		mode = Mode_Auto;
	} else {
		// manual operating mode
		actuators = doManualCycle(sensors, (mode != Mode_Manual));
		mode = Mode_Manual;
	}

	// error detection	
	errorMask = detectErrors(sensors, actuators);
	if(errorMask) {
		actuators.single.leds = errorMask;
		actuators.single.motor_open = actuators.single.motor_close = 0;
		actuators.single.warn_lamp = 1;
		mode = Mode_None;
	}

	return actuators;
} 

// for simulator only
int FSM_GetCycleTime() {
	return FSM_INTERVAL_MS;
}
