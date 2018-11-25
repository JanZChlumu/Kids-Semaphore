#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-11-25 22:32:11

#include "Arduino.h"
#include "IRLremote.h"
#include "SemaphoreTypes.h"

void SetSemColor(SemColor_T color);
void SemManualStepIncrease(void);
void Debounce(int ActualState, Deb_TDebData * const DebounceData, word SampleCount) ;
void ReadControlSelection(void);
SemIRStates_T RxDecode(RxData_T * rx);
void SemIRStateMachine(SemIRStates_T * state);
void setup();
void loop ();

#include "Semaphore.ino"


#endif
