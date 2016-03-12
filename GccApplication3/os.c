

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <limits.h>
#include "os.h"


/**
 * \file active.c
 * \brief A Skeleton Implementation of an RTOS
 * 
 * \mainpage A Skeleton Implementation of a "Full-Served" RTOS Model
 * This is an example of how to implement context-switching based on a 
 * full-served model. That is, the RTOS is implemented by an independent
 * "kernel" task, which has its own stack and calls the appropriate kernel 
 * function on behalf of the user task.
 *
 * \author Dr. Mantis Cheng
 * \date 29 September 2006
 *
 * ChangeLog: Modified by Alexander M. Hoole, October 2006.
 *			  -Rectified errors and enabled context switching.
 *			  -LED Testing code added for development (remove later).
 *
*/


#define WORKSPACE     256
#define MAXPROCESS   8


/*===========
  * RTOS Internal
  *===========
  */

/**
  * This internal kernel function is the context switching mechanism.
  * It is done in a "funny" way in that it consists two halves: the top half
  * is called "Exit_Kernel()", and the bottom half is called "Enter_Kernel()".
  * When kernel calls this function, it starts the top half (i.e., exit). Right in
  * the middle, "Cp" is activated; as a result, Cp is running and the kernel is
  * suspended in the middle of this function. When Cp makes a system call,
  * it enters the kernel via the Enter_Kernel() software interrupt into
  * the middle of this function, where the kernel was suspended.
  * After executing the bottom half, the context of Cp is saved and the context
  * of the kernel is restore. Hence, when this function returns, kernel is active
  * again, but Cp is not running any more. 
  * (See file "switch.S" for details.)
  */
extern void CSwitch();
extern void Exit_Kernel();    /* this is the same as CSwitch() */

/* Prototype */
void Task_Terminate(void);

extern void Enter_Kernel();

#define Disable_Interrupt()		asm volatile ("cli"::)
#define Enable_Interrupt()		asm volatile ("sei"::)
  


/**
  * This table contains ALL process descriptors. It doesn't matter what
  * state a task is in.
  */

volatile static PD Process[MAXPROCESS];

static queue_t ready_queue[11];
static queue_t sleep_queue;
static queue_t dead_pool_queue;

// To keep track of sleep time
static int sleep_timer = 0;
static int max_timer = INT_MAX;

volatile int preempt=0;

/**
  * The process descriptor of the currently RUNNING task.
  */
volatile static PD* Cp; 

/** 
  * Since this is a "full-served" model, the kernel is executing using its own
  * stack. We can allocate a new workspace for this kernel stack, or we can
  * use the stack of the "main()" function, i.e., the initial C runtime stack.
  * (Note: This and the following stack pointers are used primarily by the
  *   context switching code, i.e., CSwitch(), which is written in assembly
  *   language.)
  */         
volatile unsigned char *KernelSp;

/**
  * This is a "shadow" copy of the stack pointer of "Cp", the currently
  * running task. During context switching, we need to save and restore
  * it into the appropriate process descriptor.
  */
volatile unsigned char *CurrentSp;

/** index to next task to run */
volatile static unsigned int NextP;  

/** 1 if kernel has been started; 0 otherwise. */
volatile static unsigned int KernelActive;  

/** number of tasks created so far */
volatile static unsigned int Tasks;  

static volatile create_args kernel_request_create_args;



static void enqueue(queue_t* queue_ptr, volatile PD* task_to_add)
{
    task_to_add->next = NULL;

    if(queue_ptr->head == NULL)
    {
        /* empty queue */
        queue_ptr->head = task_to_add;
        queue_ptr->tail = task_to_add;
    }
    else
    {
        /* put task at the back of the queue */
        queue_ptr->tail->next = task_to_add;
        queue_ptr->tail = task_to_add;
    }
}


static void enqueue_sleep(volatile PD* task_to_add)
{
	task_to_add->next = NULL;
	volatile PD *curr;
	volatile PD *prev;
	
	int rotations = task_to_add->tick / max_timer;
	int remainder = task_to_add->tick % max_timer;
	
	int curr_rotations = 0;
	int curr_remainder = 0;
	
	if(sleep_queue.head == NULL)
	{
		sleep_queue.head = task_to_add;
		sleep_queue.tail = task_to_add;
	}

	else 
	{		
		curr=sleep_queue.head;
		prev=sleep_queue.head;
		curr_rotations = curr->tick/max_timer;
		curr_remainder = curr->tick % max_timer;
		if(rotations<curr_rotations)
		{
			sleep_queue.head=task_to_add;
			task_to_add->next=curr;
		}
		else if(rotations==curr_rotations)
		{
			if(remainder<curr_remainder)
			{
				sleep_queue.head=task_to_add;
				task_to_add->next=curr;
			}	
		}

		else
		{
			curr = prev->next;
			while (curr != NULL)
			{
				curr_rotations = curr->tick/max_timer;
				curr_remainder = curr->tick % max_timer;

				if (rotations<curr_rotations)
				{
					prev->next = task_to_add;
					task_to_add = curr;
					return;
				}
				else if (rotations == curr_rotations)
				{
					if (remainder<curr_remainder)
					{
						prev->next = task_to_add;
						task_to_add = curr;
						return;
					}
				}
				prev=prev->next;
				curr=curr->next;
			}
			prev->next=task_to_add;
			sleep_queue.tail=task_to_add;
		}
	}

}


		


/**
 * @brief Pops head of queue and returns it.
 *
 * @param queue_ptr the queue to pop
 * @return the popped task descriptor
 */
static volatile PD* dequeue(queue_t* queue_ptr)
{
    volatile PD* task_ptr = queue_ptr->head;

    if(queue_ptr->head != NULL)
    {
        queue_ptr->head = queue_ptr->head->next;
        task_ptr->next = NULL;
    }

    return task_ptr;
}

static volatile PD* dequeue_from_ready(queue_t* queue_ptr)
{
	volatile PD* current = queue_ptr->head;
	volatile PD* prev = queue_ptr->head;


	
	if(queue_ptr->head != NULL)
	{
		if (current->suspend==0){
			queue_ptr->head=current->next;
			current->next=NULL;
			return current;
		}
		current=prev->next;
		while(current!=NULL){
			if(current->suspend==0){
				prev->next=current->next;
				current->next=NULL;
				return current;
			}
			prev=prev->next;
			current=current->next;
		}
	}
	OS_Abort();
	return current;
	//Error nothing valid in queue

}







/**
 * When creating a new task, it is important to initialize its stack just like
 * it has called "Enter_Kernel()"; so that when we switch to it later, we
 * can just restore its execution context on its stack.
 * (See file "cswitch.S" for details.)
 */
static void Kernel_Create_Task_At(volatile PD *p, voidfuncptr f , PRIORITY py, int arg) 
{   
   unsigned char *sp;
   //Changed -2 to -1 to fix off by one error.
   sp = (unsigned char *) &(p->workSpace[WORKSPACE-1]);



   /*----BEGIN of NEW CODE----*/
   //Initialize the workspace (i.e., stack) and PD here!

   //Clear the contents of the workspace
   memset(&(p->workSpace),0,WORKSPACE);

   //Notice that we are placing the address (16-bit) of the functions
   //onto the stack in reverse byte order (least significant first, followed
   //by most significant).  This is because the "return" assembly instructions 
   //(rtn and rti) pop addresses off in BIG ENDIAN (most sig. first, least sig. 
   //second), even though the AT90 is LITTLE ENDIAN machine.

   //Store terminate at the bottom of stack to protect against stack underrun.
   *(unsigned char *)sp-- = ((unsigned int)Task_Terminate) & 0xff;
   *(unsigned char *)sp-- = (((unsigned int)Task_Terminate) >> 8) & 0xff;
	*(unsigned char *)sp-- = 0x00;

   //Place return address of function at bottom of stack
   *(unsigned char *)sp-- = ((unsigned int)f) & 0xff;
   *(unsigned char *)sp-- = (((unsigned int)f) >> 8) & 0xff;
	*(unsigned char *)sp-- = 0x00;

	sp = sp - 34;
   p->sp = sp;		/* stack pointer into the "workSpace" */
   p->code = f;		/* function to be executed as a task */
   p->request = NONE;
	p->arg= arg;
	p->priority=py;
	p->suspend=0;
   /*----END of NEW CODE----*/
   p->state = READY;
	enqueue(&ready_queue[p->priority],p);
}


/**
  *  Create a new task
  */
static PID Kernel_Create_Task( voidfuncptr f , PRIORITY py, int arg) 
{

   if (Tasks == MAXPROCESS) return -1;  /* Too many task! */

   /* find a DEAD PD that we can use  */

   volatile PD* p=dequeue(&dead_pool_queue);

   ++Tasks;
   Kernel_Create_Task_At( p, f ,py, arg);
   return p->pid;
}


/**
  * This internal kernel function is a part of the "scheduler". It chooses the 
  * next task to run, i.e., Cp.
  */
static void Dispatch()
{
     /* find the next READY task
       * Note: if there is no READY task, then this will loop forever!.
       */
	  //find a ready task that isnt suspended
	  //in 10 ready queues
	 
	int i;
	int found=0;
	volatile PD* current;
	
	for (i=0;i<11;i++){
		current=ready_queue[i].head;
		while(current!=NULL){
			if(!current->suspend){
				Cp=dequeue_from_ready(&ready_queue[i]);
				CurrentSp = Cp->sp;
				Cp->state = RUNNING;
				found=1;
				Enable_Interrupt();
				break;
			}
			current=current->next;
		}
		if (found){
			break;
		}
	}
	if(!found){
		//error;
		OS_Abort();
	}

}

int check_rqueue(){
	volatile PD* current;
	int i;
	for(i=0;i<Cp->priority;i++){	
		
		if(ready_queue[i].head!=NULL){
			
			current=ready_queue[i].head;
			while(current!=NULL){
				if(!current->suspend){
					return 1;
				}
				current=current->next;
			}
			
		}
	}
	return 0;
}


void preemption(){
	if(check_rqueue()){
		Cp->state=READY;
		//enqueue(&ready_queue[Cp->priority],Cp);
		
		Cp->next=ready_queue[Cp->priority].head;
		ready_queue[Cp->priority].head=Cp;
		
		Dispatch();
	}
}
/**
  * This internal kernel function is the "main" driving loop of this full-served
  * model architecture. Basically, on OS_Start(), the kernel repeatedly
  * requests the next user task's next system call and then invokes the
  * corresponding kernel function on its behalf.
  *
  * This is the main loop of our kernel, called by OS_Start().
  */
static void Next_Kernel_Request() 
{
   Dispatch();  /* select a new task to run */

   while(1) {
       /* activate this newly selected task */
      CurrentSp = Cp->sp;
		if(Cp->suspend){
		//error!!!	
			OS_Abort();
		}
      Exit_Kernel();    /* or CSwitch() */

       /* if this task makes a system call, it will return to here! */
        /* save the Cp's stack pointer */
      Cp->sp = CurrentSp;

      switch(Cp->request){
			
      case CREATE:
           kernel_request_create_args.pid= Kernel_Create_Task( kernel_request_create_args.code , kernel_request_create_args.py, kernel_request_create_args.arg );
			  preemption();
           break;
			  
      case NEXT:
		 //Enqueue appropriately
		    Cp->state = READY;
			Dispatch();
			break;
			
		case YIELD:
		 //Enqueue appropriately
			Cp->state = READY;
			enqueue(&ready_queue[Cp->priority],Cp);
			Dispatch();
			break;
			
			//If a task suspends itself, put back in ready queue and dispatch??
		 case SUSPEND:			
			Cp->state=READY;
			Cp->suspend=1;
			enqueue(&ready_queue[Cp->priority],Cp);
			Dispatch();
			break;
		 
		 //
		 case RESUME:
			 preemption();
			 break;
		
	   case NONE:
	//shouldn't happen
          break;
       case TERMINATE:
          /* deallocate all resources used by this task */
          Cp->state = DEAD;
			 enqueue(&dead_pool_queue,Cp);
          Dispatch();
          break;
	   case SLEEP:
		  Cp->state = SLEEPING;
		  // now enqueue based on sleep time
		  // enqueue to sleep queue in sleep call
		  //enqueue_sleep(Cp);		  
		  Dispatch();
		  break;
       default:
          /* Houston! we have a problem here! */
          break;
       }
    } 
}

void set_timer()
{
	TCCR1A = 0;
	TCCR1B = 0;
	//Set to CTC (mode 4)
	TCCR1B |= (1<<WGM32);
	
	//Set prescaler /8
	TCCR1B |= (1<<CS31);
	
	//Set TOP value (0.01 seconds)
	OCR1A = 20000;
	
	//Enable interupt A for timer 3.
	TIMSK1 |= (1<<OCIE1A);

}



/*================
  * RTOS  API  and Stubs
  *================
  */

/**
  * This function initializes the RTOS and must be called before any other
  * system calls.
  */
void OS_Init() 
{

	DDRA= (1<<PA0);
	DDRA |= (1<<PA1);
	PORTA &= ~(1<<PA0);
	PORTA &= ~(1<<PA1);
	
	
	set_timer();
	
   int x;

   Tasks = 0;
   KernelActive = 0;
   NextP = 0;
	//Reminder: Clear the memory for the task on creation.
   for (x = 0; x < MAXPROCESS-1; x++) {
      memset(&(Process[x]), 0, sizeof(PD));
      Process[x].state = DEAD;
		Process[x].pid=x;
		Process[x].next=&Process[x+1];
   }
	Process[MAXPROCESS-1].state=DEAD;
	Process[MAXPROCESS-1].next=NULL;
	dead_pool_queue.head = &Process[0];
	dead_pool_queue.tail = &Process[MAXPROCESS - 1];
}

void OS_Abort(void)
{
	for(;;){
		
		}
}

/**
  * This function starts the RTOS after creating a few tasks.
  */
void OS_Start() 
{   
   if ( (! KernelActive) && (Tasks > 0)) {
       Disable_Interrupt();
      /* we may have to initialize the interrupt vector for Enter_Kernel() here. */

      /* here we go...  */
      KernelActive = 1;
      Next_Kernel_Request();
      /* NEVER RETURNS!!! */
   }
}


/**
  * For this example, we only support cooperatively multitasking, i.e.,
  * each task gives up its share of the processor voluntarily by calling
  * Task_Next().
  */
PID Task_Create( voidfuncptr f, PRIORITY py, int arg)
{
   if (KernelActive ) {
     Disable_Interrupt();
	  kernel_request_create_args.code = (voidfuncptr)f;
	  kernel_request_create_args.arg = arg;
	  kernel_request_create_args.py = py;
     Cp ->request = CREATE;
     Enter_Kernel();
	  return kernel_request_create_args.pid;
   } else { 
      /* call the RTOS function directly */
      return Kernel_Create_Task( f,py,arg );
   }
}


/**
  * The calling task gives up its share of the processor voluntarily.
  */
void Task_Next() 
{
   if (KernelActive) {
     Disable_Interrupt();
     Cp ->request = NEXT;
     Enter_Kernel();
	  
  }
}


void Task_Sleep(TICK t)
{
	
	if (KernelActive) 
	{
		Disable_Interrupt();
		Cp ->request = SLEEP;
		Cp->tick = t;
		Enter_Kernel();
		
	}
}

void Task_Yield()
{
	if (KernelActive) {
		Disable_Interrupt();
		Cp ->request = YIELD;
		Enter_Kernel();
	}
}

int  Task_GetArg(void){
	return Cp->arg;
}


void Task_Suspend( PID p ){
	int i;
	for(i=0;i<11;i++){
		if (Process[i].pid==p){
			Process[i].suspend=1;
			break;
		}
	}
	if (p==Cp->pid){
		Disable_Interrupt();
		Cp ->request = SUSPEND;
		Enter_Kernel();
	}
}  
void Task_Resume( PID p ){
	int i;
	for(i=0;i<11;i++){
		if (Process[i].pid==p){
			Process[i].suspend=0;
			break;
		}
	}
	Disable_Interrupt();
	Cp ->request = RESUME;
	Enter_Kernel();
}
/**
  * The calling task terminates itself.
  */
void Task_Terminate() 
{
   if (KernelActive) {
      Disable_Interrupt();
      Cp-> request = TERMINATE;
      Enter_Kernel();
     /* never returns here! */
   }
}


void Ping2(){
	int x;
	for(;;) {
		PORTA &= ~(1<<PA0);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
		Task_Yield();
	}
}
/**
  * A cooperative "Ping" task.
  * Added testing code for LEDs.
  */

void Pong2(){
	int x;
	PID p;
	p=Task_Create(Ping2,0,0);
			//Task_Suspend(p);
	for(;;) {
		//LED on
		PORTA |= (1<<PA0);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);

		Task_Suspend(p);
		//Task_Resume(p);
		Task_Yield();
		//PORTA &= ~(1<<PA0);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
	}
}
void Ping() 
{
	int x;
//	PORTA &= ~(1<<PA0);
	for(x=0;x<32000;x++);
	for(x=0;x<32000;x++);
	for(x=0;x<32000;x++);
	for(x=0;x<32000;x++);
	Task_Create(Pong2,0,0);
  for(;;){
	  Task_Yield();

  }
}


/**
  * A cooperative "Pong" task.
  * Added testing code for LEDs.
  */
void Pong() 
{
	//PORTA &= ~(1<<PA0);  
	Task_Create(Ping,1,0);
  for(;;) {
		Task_Yield(); 
  }
}

void a_main(){
	 Task_Create(Pong,5,0);
	 Task_Terminate();
 }
 
/*
ISR(TIMER1_COMPA_vect)
{
	//sleep queue handling
	sleep_timer = (sleep_timer+1)%max_timer;
	if(sleep_timer==sleep_queue.head->tick)
	{
		PD* p=dequeue(&sleep_queue);
	}
}
*/
int main() 
{
   OS_Init();
   Task_Create( a_main , 0, 0 );
   OS_Start();
}

