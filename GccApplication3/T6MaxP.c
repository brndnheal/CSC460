/*
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <limits.h>
#include "os.h"
//#include "error_code.h"



void Ping1()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping2()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping3()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping4()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping5()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping6()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping7()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping8()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping9()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping10()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping11()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping12()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping13()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping14()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping15()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}



void Ping17()
{
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);
	Task_Yield();
	for(;;);
}

void Ping16()
{
	
	PORTA|=(1<<PA2);   // Flap pin 24 (on/off)
	PORTA&=~(1<<PA2);

	Task_Create(Ping17,1,0);
	Task_Yield();
	for(;;);
}


void a_main(){

	DDRA |= (1<<PA0);
	DDRA |= (1<<PA1);
	DDRA |= (1<<PA2);
	PORTA &= ~(1<<PA0);
	PORTA &= ~(1<<PA1);
	PORTA &= ~(1<<PA2);
	
	Task_Create(Ping1,1,0);
	Task_Create(Ping2,1,0);
	Task_Create(Ping3,1,0);
	Task_Create(Ping4,1,0);
	Task_Create(Ping5,1,0);
	Task_Create(Ping6,1,0);
	Task_Create(Ping7,1,0);
	Task_Create(Ping8,1,0);
	Task_Create(Ping9,1,0);
	Task_Create(Ping10,1,0);
	Task_Create(Ping11,1,0);
	Task_Create(Ping12,1,0);
	Task_Create(Ping13,1,0);
	Task_Create(Ping14,1,0);
	Task_Create(Ping15,1,0);
	Task_Create(Ping16,1,0);
	
	Task_Terminate();
}
*/