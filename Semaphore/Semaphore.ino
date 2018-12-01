/*
  Semaphore toy

  The following pins are usable for PinInterrupt or PinChangeInterrupt*:
  Arduino Uno/Nano/Mini: 2, 3, All pins* are usable

IR libraly reference manual
https://github.com/NicoHood/IRLremote/blob/master/Readme.md
*/

#include "IRLremote.h"
#include "SemaphoreTypes.h"
#include <avr/sleep.h>
#include <avr/wdt.h>

/* if DEBUG is used some IR Rx data could not be captured*/
//#define DEBUG //Comment this for stop debuging
#ifdef DEBUG
	#define PRINT(x) Serial.print(x)
	#define PRINTLN(x) Serial.println(x)

	/****** debug pins******/
	#define pinDeb1 10
	#define D1_ON  digitalWrite(pinDeb1,HIGH)
	#define D1_OFF digitalWrite(pinDeb1,LOW)

	#define pinDeb2 11
	#define D2_ON  digitalWrite(pinDeb2,HIGH)
	#define D2_OFF digitalWrite(pinDeb2,LOW)
	/*----* debug pins end -----*/

#else
	#define PRINT(x)
	#define PRINTLN(x)
	#define D1_ON
	#define D1_OFF
	#define D2_ON
	#define D2_OFF
#endif

/*LEDs pins*/
#define pinLedRed         7
#define pinLedOrange      8
#define pinLedGren	      9

/*Switches */
#define pinSwitchMode     5
#define pinBtnTouch 	  4

/*Power On components*/
#define pinBtnTouch_Power 6		//Capacity sensor
#define pinIR_Power 	  3		//IR sensor

/*IR sensor */
#define pinIR 2
CHashIR IRLremote;

#define analogPin 0

/* VARIABLES */
const unsigned long POWER_ON_TIMEOUT = 900000; //15minutes (15*60*1000)
volatile bool oldBtn = FALSE;
SemIRStates_T SemIRState = NotValid;
volatile unsigned long PowerOnTimeOut = 0ul;
volatile SemHumanControlSteps_T SemControl = Manual;
RxData_T RxData = {.dataIndex = 0, .RxBuffer = {0,1,2}};

/*commands received from joystick*/
const HashIR_command_t IrRxCommnad[2][4] = {  //[command],[transmitter mode A/B]
	//  { mode A UP,  mode A DOWN,    mode B UP,   mode B DOWN}
		{0xFF7A3B7A,   0xFF7A3B7A,   0x33B86A7A,   0x33B86A7A},   /*Go2Stop (all TIP positions)*/
		{0x90B1027A,    0x20E9B7A,   0xC4EF317A,   0x364CCA7A}};  /*Go2Run (all long hold positions)*/


/*Set semaphore color*/
void SetSemColor(SemColor_T color){

	/* Color change leads to trigger reset PowerTimeOut
	 * Under voltage lead to sleep mode in 15minutes*/
	if(SemControl != BatteryLow){
		PowerOnTimeOut = millis();
	}

	switch(color){
	case Red:
	    digitalWrite(pinLedRed	 , LOW);
	    digitalWrite(pinLedOrange, HIGH);
	    digitalWrite(pinLedGren  , HIGH);
		break;
	case Red_plus_Orange:
		digitalWrite(pinLedRed   , LOW);
		digitalWrite(pinLedOrange, LOW);
		digitalWrite(pinLedGren  , HIGH);
		break;
	case Green:
		digitalWrite(pinLedRed   , HIGH);
		digitalWrite(pinLedOrange, HIGH);
		digitalWrite(pinLedGren  , LOW);
		break;
	case Orange:
		digitalWrite(pinLedRed   , HIGH);
		digitalWrite(pinLedOrange, LOW);
		digitalWrite(pinLedGren  , HIGH);
		break;
	default:
		digitalWrite(pinLedRed   , HIGH);
		digitalWrite(pinLedOrange, HIGH);
		digitalWrite(pinLedGren  , HIGH);
		break;
	}
}

void SemManualStepIncrease(void){
	static unsigned char step = 0;

	if(step < ((unsigned char)No_light - 1)){
		step++;
	}else{
		step = 0;
	}
	SetSemColor((SemColor_T)step);
	PRINT(F("Man_Step: "));
	PRINTLN(step);
}

#define arrayAvrSize  15   //used for average value
/* after power on function slowly integrated to actual voltage value
 * could not be activated often - affect IR reading*/
// time consumption of this function 140us
unsigned int AverageVoltage(void){
	static int index = 0;
	static int array[arrayAvrSize] = { 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023 };
	static unsigned long oldTime = 0ul;
	unsigned long actualTime = millis();
	int suma = 0u;
	static unsigned int retVal = 1023u;

	if(actualTime > (oldTime + 500u)){
		array[index] = analogRead(analogPin);

		if(index < arrayAvrSize)
		{
			index++;
		}else{
			index = 0;
		}
		oldTime = actualTime;
	
		for(int i = 0; i < arrayAvrSize; i++){
			suma = suma + array[i];
		}
		retVal = (suma/arrayAvrSize);
	}
//	PRINTLN(retVal);
	return retVal;
}

#define BATTERY_LOW_LEVEL 850   // measured (tested) constant. Approx. 3,1V on battery
void ReadControlSelection(void){
	static int oldRead = 0;
	int actualRead = digitalRead(pinSwitchMode);

#ifdef DEBUG
//	delay(100);
#endif

	/*no hysteresis used, it's just a toy :-) */
	if(AverageVoltage() > BATTERY_LOW_LEVEL) 	
	{
		if ((actualRead != oldRead) || (SemControl == BatteryLow)){
			PRINT(F("Switch "));
			PRINTLN(actualRead);
			if(actualRead == 0u){
				SemControl = IR2Manual;
			}else{
				SemControl = Manual2IR;
			}
			oldRead = actualRead;
		}
	}else
	{
		PRINTLN(F("Under voltage"));
		SemControl = BatteryLow;
	}
}
/**
 * Decode received data.
 * Precondition: three same data was received
 */
SemIRStates_T RxDecode(HashIR_command_t rxData){

	for(unsigned int option=0; option < 4; option++){

		if(rxData == IrRxCommnad[Go2Stop][option]){
			return Go2Stop;
			PRINTLN(F("Decode Go2Stop"));

		}else if(rxData == IrRxCommnad[Go2Run][option]){
			return Go2Run;
			PRINTLN(F("Decode Go2Run"));
		}
	}
	return NotValid;
}

/**
 * Change semaphore states when IR reception activated
 */
void SemIRStateMachine(SemIRStates_T * state){
	static unsigned long oldTime = 0;
	unsigned long actualTime = millis();
	static SemIRStates_T oldState = NotValid;

	switch(*state){
	case Go2Stop:
		if(oldState != Stop){
			oldTime = actualTime;
			SetSemColor(Orange);
			*state = Stop;
		}
		break;

	case Stop:
		if (actualTime > (oldTime + 500u)){
			SetSemColor(Red);
			oldState = Stop;
		}
		break;

	case Go2Run:
		if(oldState != Run){
			oldTime = actualTime;
			SetSemColor(Red_plus_Orange);
			*state = Run;
		}
		break;

	case Run:
		if (actualTime > (oldTime + 500u)){
			SetSemColor(Green);
			oldState = Run;
		}
		break;
	default:
		//SetSemColor(No_light);
		break;
	}
}
/**
 * Toy is not used for defined time. Better to go into deep sleep mode.
 * COULD BE LEAVE ONLY BY MECHANICAL SWITCH CYCLE
 * Current consumption in Power-save mode is 250uA //TODO some improvements?
 */
void PowerOnExpired(void){

	if(millis() > (PowerOnTimeOut + POWER_ON_TIMEOUT )){

		PRINTLN(F("Power OFF"));

		pinMode(pinLedRed, INPUT);
		pinMode(pinLedOrange, INPUT);
		pinMode(pinLedGren, INPUT);
		pinMode(pinSwitchMode,INPUT);
		pinMode(pinBtnTouch,INPUT);
		/*Power on components*/
		pinMode(pinBtnTouch_Power, INPUT);
		pinMode(pinIR_Power, INPUT);
#ifdef DEBUG
		pinMode(pinDeb1, INPUT);
		pinMode(pinDeb2, INPUT);
#endif
		wdt_disable();  					   // will save 38uA
 		detachInterrupt(0);      			   // disables interrupt 0 on pin 2 so the
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
	    sleep_mode();            			   // here the device is actually put to sleep!!
	}
}

void setup(){

	#ifdef DEBUG
	while (!Serial);
	Serial.begin(115200);
	PRINTLN(F("Startup"));

	pinMode(pinDeb1, OUTPUT);
	pinMode(pinDeb2, OUTPUT);
	D1_OFF;
	D2_OFF;
#endif

	pinMode(pinLedRed, OUTPUT);
	pinMode(pinLedOrange, OUTPUT);
	pinMode(pinLedGren, OUTPUT);
	pinMode(pinSwitchMode,INPUT_PULLUP);
	pinMode(pinBtnTouch,INPUT);

	/*Power on components*/
	pinMode(pinBtnTouch_Power, OUTPUT);
	digitalWrite(pinBtnTouch_Power, LOW);
	pinMode(pinIR_Power, OUTPUT);
	digitalWrite(pinIR_Power, LOW);

	SetSemColor(Orange);		//default color
	
	analogReference(INTERNAL);  //analog reference set to 1.1V

	PowerOnTimeOut = millis(); //trigger - reset PowerTimeOut
	ReadControlSelection();	
	PRINTLN(POWER_ON_TIMEOUT);
}



void loop (){
	bool actBtn  = FALSE;
	
	/*check how long was not semaphore used*/
	PowerOnExpired();
	
	switch(SemControl){

		/*Manual control*/
		case Manual:
			ReadControlSelection();			
			actBtn = digitalRead(pinBtnTouch);

			/*button has two state; touch, release. Only touch state is accessible */
			if(actBtn != oldBtn){
				if(actBtn == TRUE){
					SemManualStepIncrease();
					PRINT(F("T "));
				}else{
				}
				oldBtn = actBtn;
			}else
			{
			}
		break;
		
		/*Transition state; enable IR*/
		case Manual2IR:
			IRLremote.begin(pinIR); //TODO if error orange light
			SemControl = IR;
			digitalWrite(pinIR_Power, LOW);		//Power on
			delay(10);
		break;
		
		/* IR control */
		case IR:
			/*read IR data if available*/
			if (IRLremote.available()){
				D1_ON;
				HashIR_data_t rx_data = IRLremote.read();
				RxData.RxBuffer[RxData.dataIndex] = rx_data.command;

				PRINT(RxData.dataIndex);
				PRINT(F(" Rx "));
#ifdef DEBUG
				Serial.println(rx_data.command, HEX);
#endif
				/* cyclic filling buffer*/
				if(RxData.dataIndex < BUFFER_SIZE - 1){
					RxData.dataIndex++;
				}else{
					//buffer full
					RxData.dataIndex = 0 ;
				}
			}else{
				D1_OFF;
			}

			/*Timeout is triggered when IRLremote.available() == 1 , 20*1000 is physically 3,6ms after available data received
			 * this condition is entered always when nothing received*/
			if(IRLremote.timeout() > (20000)){
//				D2_ON;
				/*check if all 3 buffers are the same*/
				if((    RxData.RxBuffer[0] == RxData.RxBuffer[1])  \
					&& (RxData.RxBuffer[0] == RxData.RxBuffer[2])  ){

					/*Decode received data*/
					SemIRState = RxDecode(RxData.RxBuffer[0]);  //doesn't matter which index is hand over
					PRINT(F("Decode "));
					PRINTLN(SemIRState);

					/* "clear buffer" - write some absolutely unexpected data*/
					RxData.RxBuffer[0] = 0;
					RxData.RxBuffer[1] = 1;
					RxData.RxBuffer[2] = 2;
				}

				/*was received something now is some free time for other things...*/
				ReadControlSelection();
//				D2_OFF;
			}

			/*Call semaphore state machine*/
			SemIRStateMachine(&SemIRState);
		break;

		/*Transition state; disable IR*/
		case IR2Manual:
			IRLremote.end(pinIR);
			SemControl = Manual;
			digitalWrite(pinIR_Power, HIGH);		//Power OFF IR sensor (save some current consumption)
			delay(10);
		break;
		
		/* Battery low - semaphore error*/
		case BatteryLow:
			SetSemColor(Orange);
			delay(1000);
			SetSemColor(No_light);
			delay(1000);
			break;

		default:			
		break;					
	}
}
