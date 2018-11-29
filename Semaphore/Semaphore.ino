/*
  The following pins are usable for PinInterrupt or PinChangeInterrupt*:
  Arduino Uno/Nano/Mini: 2, 3, All pins* are usable

IR libraly reference manual
https://github.com/NicoHood/IRLremote/blob/master/Readme.md
*/

#include "IRLremote.h"
#include "SemaphoreTypes.h"

/* if DEBUG is used some IR Rx data could not be captured*/
//#define DEBUG //Comment this for stop debuging
#ifdef DEBUG
	#define PRINT(x) Serial.print(x)
	#define PRINTLN(x) Serial.println(x)
#else
	#define PRINT(x)
	#define PRINTLN(x)
#endif

/*LEDs pins*/
#define pinLedRed 7
#define pinLedOrange 8
#define pinLedGren	9
#define pinLedCheck LED_BUILTIN

/*Switches */
#define pinSwitchMode  5
#define pinBtnTouch 4

#define Led_OFF digitalWrite(pinLedCheck, LOW)
#define Led_ON  digitalWrite(pinLedCheck, HIGH)

/*IR sensor */
#define pinIR 2
CHashIR IRLremote;

#define analogPin 0

volatile Deb_TDebData sBtnTouch = {0,0,0}; //structure for button deboucing
volatile SemHumanControlSteps_T SemControl = Manual;
RxData_T RxData = {.dataIndex = 0, .newMsg = FALSE, .RxBuffer = {}};

/*commands received from train joystick*/
const HashIR_command_t IrRxCommnad[4][2][2] = {  //[command],[transmitter mode A/B], [two first commnads ]
		{{0xC4EF317A,0x33B86A7A},{0x90B1027A,0xFF7A3B7A}} ,       /*Forward move TIP  -> Go2Stop*/
		{{0xC4EF317A,0xC4EF317A},{0x90B1027A,0x90B1027A}} ,       /*Forward move LONG -> Go2Run */
		{{0x364CCA7A,0x33B86A7A},{0x20E9B7A, 0xFF7A3B7A}} ,       /*Reverse move TIP  -> Go2Stop*/
		{{0x364CCA7A,0x364CCA7A},{0x20E9B7A, 0x20E9B7A}}};        /*Reverse move LONG -> Go2Run */

/*Set semaphore color*/
void SetSemColor(SemColor_T color){

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
/*Debounce*/
//todo not used for Capacity button
void Debounce(int ActualState, Deb_TDebData * const DebounceData, word SampleCount)
{
  /* Has been input value changed? */
  if (ActualState != DebounceData->NotDebValue)
  {
    /* change on input: remember actual input value */
    DebounceData->NotDebValue  = ActualState;

    /* change on input: clear debouncing counter */
    DebounceData->Counter = 0x00;
  }
  /* Did debouncing time elapse? */
  if (DebounceData->Counter < SampleCount)
  {
    /* debouncing time not yet out, count this time */
    DebounceData->Counter++;
  }
  else
  {
    /* debouncing time is out, report new debounced value */
    DebounceData->DebValue = ActualState;
  }
}

#define arrayAvrSize  15   //used for average value
/* after power on function slowly integrated to actual voltage value
 * could not be activated offen - affect IR reading*/
unsigned int AverageVoltage(void){
	static int index = 0;
	static int array[arrayAvrSize] = { 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023 };
	static unsigned long oldTime = 0ul;
	unsigned long actualTime = millis();
	float suma = 0u;
	unsigned int retVal;

	if(actualTime > (oldTime + 500u)){
		array[index] = analogRead(analogPin);

		if(index < arrayAvrSize)
		{
			index++;
		}else{
			index = 0;
		}
		oldTime = actualTime;
	}

	for(int i = 0; i < arrayAvrSize; i++){
		suma = suma + array[i];
	}

	retVal = (int)(suma/arrayAvrSize);
//	PRINTLN(retVal);
	return retVal;
}

void ReadControlSelection(void){
	static int oldRead = 0;
	int actualRead = digitalRead(pinSwitchMode);

#ifdef DEBUG
//	delay(100);
#endif

	/*no hysteresis used, it's just a toy :-) */
	if(AverageVoltage() > 850u) 	// measured (tested) constant. Approx. 3,1V on battery
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

SemIRStates_T RxDecode(RxData_T * rx){
	SemIRStates_T retval = Idle;

	for(unsigned int symbol=0; symbol < 4; symbol++){
		/*look for first valid command*/
		if (rx->RxBuffer[Frame_0] == IrRxCommnad[symbol][ModeA][0]      \
				||                                                      \
			rx->RxBuffer[Frame_0] == IrRxCommnad[symbol][ModeB][0]	)
		{
			PRINTLN(F("Dec1 OK"));
			/*First*/
			if(rx->RxBuffer[Frame_1] == IrRxCommnad[symbol][ModeA][1]  \
			   ||	                                                   \
			   rx->RxBuffer[Frame_1] == IrRxCommnad[symbol][ModeB][1])
			{
				PRINTLN(F("Dec2 OK"));
				if((symbol == 0)||(symbol == 2)){
					retval = Go2Stop;
					PRINTLN(F("Decode Go2Stop"));
				}else{
					retval = Go2Run;
					PRINTLN(F("Decode Go2Run"));
				}
				//TODO break;
				}
		}
	}
	return retval;
}

void SemIRStateMachine(SemIRStates_T * state){
	static unsigned long oldTime = 0;
	unsigned long actualTime = millis();
	static SemIRStates_T oldState = Idle;

	switch(*state){
	case Go2Stop:
		if(oldState != Stop){
			oldTime = actualTime;
			SetSemColor(Orange);
			*state = Stop;
		}
		break;

	case Stop:
		if (actualTime > (oldTime + 1000u)){
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
		if (actualTime > (oldTime + 1000u)){
			SetSemColor(Green);
			oldState = Run;
		}
		break;
	default:
		//SetSemColor(No_light);
		break;
	}
}

void setup(){
#ifdef DEBUG
	while (!Serial);
	Serial.begin(115200);
	PRINTLN(F("Startup"));
#endif
	pinMode(pinLedRed, OUTPUT);
	pinMode(pinLedOrange, OUTPUT);
	pinMode(pinLedGren, OUTPUT);
	pinMode(pinLedCheck, OUTPUT);

	pinMode(pinSwitchMode,INPUT_PULLUP);
	pinMode(pinBtnTouch,INPUT);

	digitalWrite(pinLedCheck, LOW);
	SetSemColor(No_light);

	analogReference(INTERNAL);  //analog reference set to 1.1V
	Led_ON;
}


volatile bool oldBtn = FALSE;
SemIRStates_T SemIRState = Idle;
volatile bool tLock = FALSE;

void loop (){
	bool actBtn  = FALSE;
	
	ReadControlSelection();
	
	switch(SemControl){
		/*Manual control*/
		case Manual:
//			Debounce(digitalRead(pinBtnTouch), &sBtnTouch, 50u); //debounce some time
			actBtn = digitalRead(pinBtnTouch);

			/*button has two state; touch, release. Only touch state is accessible */
			if(actBtn != oldBtn){
				if(actBtn == TRUE){
					SemManualStepIncrease();
					PRINT(F("T "));
					Led_ON;
				}else{
					Led_OFF;
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
			delay(10);
		break;
		
		/* IR control */
		case IR:
			if (IRLremote.available()){
					Led_ON;
					HashIR_data_t rx_data = IRLremote.read();
					RxData.RxBuffer[RxData.dataIndex] = rx_data.command;

					PRINT(RxData.dataIndex);
					PRINT(F(" Rx "));
					PRINTLN(rx_data.command);

					if(RxData.dataIndex < BUFFER_SIZE){
						RxData.dataIndex++;
					}else{
						//buffer full
					}
			}

			/*Receiving is finished in 500ms (650ms is for sure) */
			if(IRLremote.timeout() > 650000u){
				Led_OFF;
				if(RxData.dataIndex != 0){
					SemIRState = RxDecode(&RxData);
//					PRINT(F("Decode "));
//					PRINTLN(SemIRState);

					/*clear buffer */
					do{
						RxData.RxBuffer[RxData.dataIndex--] = 0;   //index --
						PRINT(RxData.dataIndex);
					}while(RxData.dataIndex > 0);

					PRINTLN(F("clear"));
				}
			}

			/*Semaphore state machine*/
			SemIRStateMachine(&SemIRState);
		break;

		/*Transition state; disable IR*/
		case IR2Manual:
			IRLremote.end(pinIR);
			SemControl = Manual;
			delay(10);
		break;
		
		/* Battery low - semaphore error*/
		case BatteryLow:
			Led_OFF;
			SetSemColor(Orange);
			delay(1000);
			SetSemColor(No_light);
			delay(1000);
			Led_ON;
			break;

		default:			
		break;					
	}
}
