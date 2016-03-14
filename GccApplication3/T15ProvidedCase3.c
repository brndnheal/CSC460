
#include "os.h"
//
// LAB - TEST 3
//	Noah Spriggs, Murray Dunne, Daniel McIlvaney
//
// EXPECTED RUNNING ORDER: P1,P2,P3,P2,P1,P3,P2,P1
//
// P1 sleep                     lock 1                               unlock 1
// P2 lock 1  sleep    lock 2                    unlock 2 unlock 1
// P3 sleep   lock 2   sleep           unlock 2
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#include <util/delay.h>

MUTEX mut1;
MUTEX mut2;

void Task_P1()
{
		PORTA |= (1<<PA2);
	Task_Sleep(30);
	Mutex_Lock(mut1);
    Mutex_Unlock(mut1);
	 	PORTA &= ~(1<<PA2);
    for(;;);
}

void Task_P2()
{
		PORTA |= (1<<PA1);
    Mutex_Lock(mut1);
    Task_Sleep(20);
    Mutex_Lock(mut2);
    Task_Sleep(10);
    Mutex_Unlock(mut2);
	Mutex_Unlock(mut1);
		PORTA &= ~(1<<PA1);
    for(;;);
}

void Task_P3()
{
		PORTA|= (1<<PA0);
	Task_Sleep(10);
    Mutex_Lock(mut2);
    Task_Sleep(20);
    Mutex_Unlock(mut2);
	 	PORTA &= ~(1<<PA0);
    for(;;);
}

void a_main()
{
		DDRA |= (1<<PA0);
		DDRA |= (1<<PA1);
		DDRA |= (1<<PA2);
		PORTA &= ~(1<<PA0);
		PORTA &= ~(1<<PA1);
		PORTA &= ~(1<<PA2);
	mut1 = Mutex_Init();
    mut2 = Mutex_Init();

	Task_Create(Task_P1, 1, 0);
	Task_Create(Task_P2, 2, 0);
	Task_Create(Task_P3, 3, 0);
}