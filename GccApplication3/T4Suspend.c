

/*#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <limits.h>
#include "os.h"
#include "error_code.h"


void Pang()
{
	for(;;){
		PORTA|=(1<<PA0);
		PORTA&=~(1<<PA0);
//		Task_Yield();
	}
	//
}


void Pong()
{
	PID p = Task_Create(Pang,1,0);
	for(;;){
		PORTA|=(1<<PA1);
		PORTA&=~(1<<PA1);
		Task_Suspend(p);	
		Task_Yield();
			PORTA|=(1<<PA1);
			PORTA&=~(1<<PA1);
		Task_Resume(p);
		Task_Yield();
	}
}

void Ping()
{
	PID p = Task_Create(Pong,1,0);
	for(;;){
		PORTA|=(1<<PA2);
		PORTA&=~(1<<PA2);
		Task_Suspend(p);
		Task_Yield();
			PORTA|=(1<<PA2);
			PORTA&=~(1<<PA2);
		Task_Resume(p);
		Task_Yield();
	}
}



void a_main(){
	DDRA |= (1<<PA0);
	DDRA |= (1<<PA1);
	DDRA |= (1<<PA2);
	PORTA &= ~(1<<PA0);
	PORTA &= ~(1<<PA1);
	PORTA &= ~(1<<PA2);
	Task_Create(Ping,1,0);

	
	Task_Terminate();
}
*/