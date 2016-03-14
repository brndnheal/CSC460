/*
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <limits.h>
#include "os.h"
#include "error_code.h"

EVENT e;

void Pong()
{
	for(;;){
		PORTA|=(1<<PA1);
		PORTA&=~(1<<PA1);
		Task_Sleep(100);
		Event_Signal(e);
		//Task_Yield();
	}
}

void Ping()
{
	for(;;){
		PORTA|=(1<<PA2);
		PORTA&=~(1<<PA2);
		Event_Wait(e);
		PORTA|=(1<<PA2);
		PORTA&=~(1<<PA2);
	}
}



void a_main(){
	DDRA |= (1<<PA0);
	DDRA |= (1<<PA1);
	DDRA |= (1<<PA2);
	DDRA |= (1<<PA3);
	PORTA &= ~(1<<PA0);
	PORTA &= ~(1<<PA1);
	PORTA &= ~(1<<PA2);
	PORTA &= ~(1<<PA3);
	e=Event_Init();
	Task_Create(Ping,1,0);
	Task_Create(Pong,2,0);
	Task_Terminate();
}
*/