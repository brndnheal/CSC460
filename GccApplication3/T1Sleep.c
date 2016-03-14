/*
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <limits.h>
#include "os.h"



void Ping(){
	for(;;) {
		PORTA &= ~(1<<PA1); 
		Task_Sleep(500);
	}
}

void Pong(){
	for(;;) {
		PORTA |= (1<<PA1);
		Task_Sleep(500);
	}
}

void Ping2(){
	for(;;) {
		PORTA &= ~(1<<PA0); 
		Task_Sleep(500);
	}
}

void Pong2(){
	for(;;) {
		PORTA |= (1<<PA0);
		Task_Sleep(500);
	}
}

void a_main(){
	DDRA |= (1<<PA0);
	DDRA |= (1<<PA1);
	DDRA |= (1<<PA2);
	PORTA &= ~(1<<PA0);
	PORTA &= ~(1<<PA1);
	PORTA &= ~(1<<PA2);
	Task_Create(Ping,5,0);
	Task_Create(Pong,5,0);
	Task_Create(Ping2,4,0);
	Task_Create(Pong2,4,0);
	Task_Terminate();
}
*/