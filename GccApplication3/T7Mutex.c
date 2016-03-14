

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <limits.h>
#include "os.h"



MUTEX m1;
MUTEX m2;



void Ping(){
	for(;;) {
		Mutex_Lock(m1);
		PORTA |= (1<<PA0);
		Mutex_Unlock(m1);
		PORTA &= ~(1<<PA0);
		Task_Sleep(100);
		
	}
}

void Pong(){
	for(;;) {
		Mutex_Lock(m1);
		PORTA |= (1<<PA1);
		PORTA &= ~(1<<PA1);
		Mutex_Unlock(m1);
	}
}



void a_main(){
	
	DDRA |= (1<<PA0);
	DDRA |= (1<<PA1);
	DDRA |= (1<<PA2);
	PORTA &= ~(1<<PA0);
	PORTA &= ~(1<<PA1);
	PORTA &= ~(1<<PA2);
	
	m1=Mutex_Init();
	m2=Mutex_Init();
	Task_Create(Ping,5,0);
	Task_Create(Pong,6,0);
	Task_Terminate();
}
