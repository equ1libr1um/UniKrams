#line 1 "../src/fsm.c"

#line 1 "../src/fsm.h"




#line 1 "../src/sysdefs.h"



 
typedef union{
	struct{
		 
		unsigned char mode_auto: 1;
		                                          
		unsigned char button_close: 1;
		 
		unsigned char button_open: 1;
		 
		unsigned char _reserved1: 1;
		 
		unsigned char limit_open: 1;
		 
		unsigned char limit_closed: 1;
		   
		unsigned char lb_collision: 1;
		 
		unsigned char lb_approximation: 1;
	}single;
	 
	unsigned char all;
} Sensors_t;                         

 
typedef union{
	struct{
		  
		unsigned char warn_lamp: 1;
		 
		unsigned char motor_close: 1;
		 
		unsigned char motor_open: 1;
		 
		unsigned char leds: 5; 
	} single;
	 
	unsigned char all;
} Actuators_t;

  



#line 5 "../src/fsm.h"







void  FSM_Init();

Actuators_t  FSM_Cycle(Sensors_t sensors); 

int  FSM_GetCycleTime();


#line 2 "../src/fsm.c"

static enum {
	Mode_None,
	Mode_Manual, 
	Mode_Auto
} mode = Mode_None;

 
 
 

 

 

 

static Actuators_t doManualCycle(Sensors_t sensors, int doInit) {
	Actuators_t actuators;
		
	if(doInit) {
		 
	} 	

	 
	
	 

		
	return actuators; 	
} 

 
 
 

 
typedef enum{
	 
	
	Auto_Initial  
} AutoState_t;

 
typedef union {
	struct {		
		unsigned char lb: 1; 		 
		unsigned char closed: 1;	 
		unsigned char open: 1;		 
		unsigned char _reserved: 5;	 
	} single;
	unsigned char all;
} AutoInputs_t;

static const struct { 
	AutoState_t nextState;	
	Actuators_t output;
} autoTable[2][8] = {
	 	 	 	 	 








	
	 
};



static Actuators_t doAutoCycle(Sensors_t sensors, int doInit) {
	Actuators_t actuators;
	AutoInputs_t inputs;
	static AutoState_t state;
	static int closeDelayCounter;
		
	if(doInit) {
		closeDelayCounter = 0;
		state = Auto_Initial;
	} 
	
	 
	inputs.all = 0;
	if(sensors.single.limit_open) inputs.single.open = 1;
	if(sensors.single.limit_closed) inputs.single.closed = 1;
	if(sensors.single.lb_approximation || sensors.single.lb_collision) inputs.single.lb = 1;
	
	 
	actuators = autoTable[state][inputs.all].output;
	state = autoTable[state][inputs.all].nextState;	
	
	 
	if(actuators.single.motor_close) {
		if(closeDelayCounter > 0) {
			closeDelayCounter--;
			actuators.single.motor_close = 0;
			actuators.single.warn_lamp = 0;
		} 
	} else closeDelayCounter = (5000 / 20 )   ;
		
	return actuators;
} 

 
 
 

static unsigned char errorMask;



static unsigned char detectErrors(Sensors_t sensors, Actuators_t actuators) {
	unsigned char mask = 0;
	
	if(sensors.single.limit_open && sensors.single.limit_closed) mask |= 0x1;  

	 
	
	return mask;
}

 
 
 

void FSM_Init() {
	mode = Mode_None;
	errorMask = 0;
}

Actuators_t FSM_Cycle(Sensors_t sensors) {	
	Actuators_t actuators;
	if(sensors.single.mode_auto){
		 
		actuators = doAutoCycle(sensors, (mode != Mode_Auto));
		mode = Mode_Auto;
	} else {
		 
		actuators = doManualCycle(sensors, (mode != Mode_Manual));
		mode = Mode_Manual;
	}

	 
	errorMask = detectErrors(sensors, actuators);
	if(errorMask) {
		actuators.single.leds = errorMask;
		actuators.single.motor_open = actuators.single.motor_close = 0;
		actuators.single.warn_lamp = 1;
		mode = Mode_None;
	}

	return actuators;
} 

 
int FSM_GetCycleTime() {
	return 20  ;
}
