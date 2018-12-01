#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-12-01 21:11:06

#include "Arduino.h"
#include "IRLremote.h"
#include "SemaphoreTypes.h"
#include <avr/sleep.h>
#include <avr/wdt.h>

void SetSemColor(SemColor_T color);
void SemManualStepIncrease(void);
unsigned int AverageVoltage(void);
void ReadControlSelection(void);
SemIRStates_T RxDecode(HashIR_command_t rxData);
void SemIRStateMachine(SemIRStates_T * state);
void PowerOnExpired(void);
void setup();
void loop ();

#include "Semaphore.ino"


#endif
