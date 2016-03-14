#include "os.h"
//
// LAB - TEST 1
//	Noah Spriggs, Murray Dunne
//
//
// EXPECTED RUNNING ORDER: P1,P2,P3,P1,P2,P3,P1
//
// P1 sleep              lock(attept)            lock
// P2      sleep                     signal
// P3           lock wait                  unlock
#include "os.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#include <util/delay.h>

MUTEX mut;
EVENT evt;

void Task_P1()
{
	PORTA |= (1<<PA1);
	Task_Sleep(10); // sleep 100
	Mutex_Lock(mut);
	PORTA &= ~(1<<PA1);
    for(;;);
}

void Task_P2()
{
	PORTA |= (1<<PA1);
	Task_Sleep(20); // sleep 200ms
	Event_Signal(evt);
	PORTA &= ~(1<<PA1);
    for(;;);

}

void Task_P3()
{
	PORTA |= (1<<PA0);
	Mutex_Lock(mut);
	Event_Wait(evt);
	Mutex_Unlock(mut);
	PORTA &= ~(1<<PA0);
    for(;;);
}

void a_main()
{

	//Place these as necessary to display output if not already doing so inside the RTOS
	//initialize pins
	DDRA |= (1<<PA0);
	DDRA |= (1<<PA1);
	DDRA |= (1<<PA2);
	PORTA &= ~(1<<PA0);
	PORTA &= ~(1<<PA1);
	PORTA &= ~(1<<PA2);

	mut = Mutex_Init();
	evt = Event_Init();

	Task_Create(Task_P1, 1, 0);
	Task_Create(Task_P2, 2, 0);
	Task_Create(Task_P3, 3, 0);
}