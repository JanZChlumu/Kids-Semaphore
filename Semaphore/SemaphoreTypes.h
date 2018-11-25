/**
 * 
 * $HeadURL$
 * $Id$
 * $Revision$ 
 *
 * @file   SemaphoreTypes.h
 * @date   23. 11. 2018
 * @author vlady001
 * 
 * Copyright(c) 2018 Leopold Kostal GmbH & Co. KG. Content and presentation
 * are protected worldwide. Copying, issuing to other parties or any kind of use,
 * in whole or in part, is prohibited without prior permission. All rights -
 * including industrial property rights - are reserved.
 *
 */
#ifndef SEMAPHORETYPES_H_
#define SEMAPHORETYPES_H_

#define BUFFER_SIZE 7
#define FALSE 0
#define TRUE 1
#define ModeA 0
#define ModeB 1
#define Frame_0 0
#define Frame_1 1

typedef enum {
	Red = 0,
	Red_plus_Orange,
	Green,
	Orange,
	No_light
}SemColor_T;


typedef enum {
	Manual = 0,
	Manual2IR,
	IR,
	IR2Manual
}SemHumanControlSteps_T;

typedef enum {
	Idle = 0,
	Go2Stop,
	Go2Run,
	Run,
	Stop
}SemIRStates_T;

typedef struct {
	unsigned char dataIndex;
	unsigned char newMsg;
	HashIR_command_t RxBuffer[BUFFER_SIZE];
}RxData_T;

/* Debounce structure*/
typedef struct
{
  /** De bounced value. */
  int DebValue;
  /** Not yet de bounced value. */
  int NotDebValue;
  /** De bouncing counter - actual value. */
  word Counter;
} Deb_TDebData;



#endif /* SEMAPHORETYPES_H_ */
