#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "os.h"

// Hello!

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
static PD Process[MAXPROCESS];

static queue_t ready_queue[10];
static queue_t sleep_queue;
static queue_t dead_pool_queue;


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




static void enqueue(queue_t* queue_ptr, PD* task_to_add)
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


/**
 * @brief Pops head of queue and returns it.
 *
 * @param queue_ptr the queue to pop
 * @return the popped task descriptor
 */
static PD* dequeue(queue_t* queue_ptr)
{
    PD* task_ptr = queue_ptr->head;

    if(queue_ptr->head != NULL)
    {
        queue_ptr->head = queue_ptr->head->next;
        task_ptr->next = NULL;
    }

    return task_ptr;
}







/**
 * When creating a new task, it is important to initialize its stack just like
 * it has called "Enter_Kernel()"; so that when we switch to it later, we
 * can just restore its execution context on its stack.
 * (See file "cswitch.S" for details.)
 */
void Kernel_Create_Task_At( PD *p, voidfuncptr f , PRIORITY py, int arg) 
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
   /*----END of NEW CODE----*/
   p->state = READY;
	enqueue(&ready_queue[p->priority],p);
}


/**
  *  Create a new task
  */
static void Kernel_Create_Task( voidfuncptr f , PRIORITY py, int arg) 
{
   int x;

   if (Tasks == MAXPROCESS) return;  /* Too many task! */

   /* find a DEAD PD that we can use  */
   for (x = 0; x < MAXPROCESS; x++) {
       if (Process[x].state == DEAD) break;
   }

   ++Tasks;
   Kernel_Create_Task_At( &(Process[x]), f ,py, arg);

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
	  //find a ready task
	  //in 10 ready queues
	  
	  
   /*while(Process[NextP].state != READY) {
      NextP = (NextP + 1) % MAXPROCESS;
   }*/
	int i;
	
	for (i=0;i<10;i++){
		if (ready_queue[i].head!=NULL){
			Cp=dequeue(&ready_queue[i]);
			CurrentSp = Cp->sp;
			Cp->state = RUNNING;
			Enable_Interrupt();
			break;
		}
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
       Exit_Kernel();    /* or CSwitch() */

       /* if this task makes a system call, it will return to here! */

        /* save the Cp's stack pointer */
       Cp->sp = CurrentSp;

       switch(Cp->request){
       case CREATE:
           Kernel_Create_Task( Cp->code , Cp->priority, Cp->arg );
           break;
       case NEXT:
		 //Enqueue appropriately
		   Cp->state = READY;
			Dispatch();
			break;
	   case NONE:
	//shouldn't happen
          break;
       case TERMINATE:
          /* deallocate all resources used by this task */
          Cp->state = DEAD;
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
	DDRA = (1<<PA1);
	PORTA &= ~(1<<PA0);
	PORTA &= ~(1<<PA1);
	
	
	set_timer();
	
   int x;

   Tasks = 0;
   KernelActive = 0;
   NextP = 0;
	//Reminder: Clear the memory for the task on creation.
   for (x = 0; x < MAXPROCESS-1; x++) {
      memset(&(Process[x]),0,sizeof(PD));
      Process[x].state = DEAD;
		Process[x].next=&Process[x+1];
   }
	Process[MAXPROCESS-1].state=DEAD;
	Process[MAXPROCESS-1].next=NULL;
	dead_pool_queue.head = &Process[0];
	dead_pool_queue.tail = &Process[MAXPROCESS - 1];
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
     Cp ->request = CREATE;
     Cp->code = f;
	  Cp->arg=arg;
	  Cp->priority=py;
     Enter_Kernel();
	  return Cp->pid;
   } else { 
      /* call the RTOS function directly */
      Kernel_Create_Task( f,py,arg );
		return Cp->pid;
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
	  enqueue(&ready_queue[Cp->priority],Cp);
     Enter_Kernel();
	  
  }
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

/*============
  * A Simple Test 
  *============
  */

/**
  * A cooperative "Ping" task.
  * Added testing code for LEDs.
  */
void Ping() 
{
	int x;
  for(;;){

	PORTA &= ~(1<<PA1);
	for(x=0;x<32000;x++);
	for(x=0;x<32000;x++);
	for(x=0;x<32000;x++);
	for(x=0;x<32000;x++);
	Task_Next();
  }
}


/**
  * A cooperative "Pong" task.
  * Added testing code for LEDs.
  */
void Pong() 
{
	int x;
  for(;;) {
	//LED on

	//LED off
	PORTA |= (1<<PA1); 
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
		for(x=0;x<32000;x++);
	
	Task_Next();
  }
}


ISR(TIMER1_COMPA_vect)
{
	//sleep queue handling
}

int main() 
{
   OS_Init();
   Task_Create( Pong , 0, 0 );
   Task_Create( Ping , 0, 0 );
   OS_Start();
}

