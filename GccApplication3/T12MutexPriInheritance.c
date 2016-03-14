/*

#include "os.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#include <util/delay.h>

void Task_2();
MUTEX shared_mutex;

void Task_1(){
	//12. Finally run Task_1's blink LED
		_delay_ms(200);
	PORTA |= (1<<PA2);
	_delay_ms(200);
	PORTA &= ~(1<<PA2);
	_delay_ms(200);
	
}

void Task_3(){
	//7. Lock locked mutex, should block Task 3 and Task 2 should inherit priority
	//   after blocking on mutex lock, dispatches to next highest priority task
	//   which will now be Task_2 since it has inherited Task_3's priority
		PORTA |= (1<<PA1);
	Mutex_Lock(shared_mutex);
	//10. Once Mutex has been released blink the LED
		PORTA &= ~(1<<PA1);
	_delay_ms(200);
	PORTA &= ~(1<<PA1);
	_delay_ms(200);

	//11. Unlock the Mutex for completeness
	Mutex_Unlock(shared_mutex);
	PORTA |= (1<<PA1);
}

void Task_2(){
	//3. Lock the shared mutex
	PORTA |= (1<<PA0);
	Mutex_Lock(shared_mutex);
	_delay_ms(200);
	//4. Create Task_3 with higher priority than Task_2


	int Task_3_id = Task_Create(Task_3, 1, 0);
		//5. Create Task_2 with higher priority than Task_2 but lower Priority than Task_3
	int Task_2_id = Task_Create(Task_1, 2, 0);

	//8. Return from Task_3 here and blink pin 13 LED
	
	_delay_ms(200);
	PORTA &= ~(1<<PA0);
	_delay_ms(200);
	//9. Unlock the Mutex ->  unblocking Task_3
	Mutex_Unlock(shared_mutex);
	
	PORTA |= (1<<PA0);
}


// OLD BUG but good explanation why it is bad to use the number of tasks as the ID
// after this terminates any new tasks will have overlapping id's
// example
// a_main == id == tasks == 0
// task_2_id == tasks == 1
// Terminate a_main -> tasks = tasks - 1
// task_3_id == tasks == 1

void a_main() {
		DDRA |= (1<<PA0);
		DDRA |= (1<<PA1);
		DDRA |= (1<<PA2);
		DDRA |= (1<<PA3);
		PORTA &= ~(1<<PA0);
		PORTA &= ~(1<<PA1);
		PORTA &= ~(1<<PA2);
		PORTA &= ~(1<<PA3);

	// 1. Create the shared mutex
		shared_mutex = Mutex_Init();

	// 2. Create the first task to run aka Task_2, with the lowest priority
		Task_Create(Task_2, 4, 0);
}
*/