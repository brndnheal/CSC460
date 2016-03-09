#ifndef _OS_H_  
#define _OS_H_  
   
#define MAXTHREAD     16       
#define WORKSPACE     256   // in bytes, per THREAD
#define MAXMUTEX      8 
#define MAXEVENT      8      
#define MSECPERTICK   10   // resolution of a system tick in milliseconds
#define MINPRIORITY   10   // 0 is the highest priority, 10 the lowest


#ifndef NULL
#define NULL          0   /* undefined */
#endif

typedef unsigned int PID;        // always non-zero if it is valid
typedef unsigned int MUTEX;      // always non-zero if it is valid
typedef unsigned char PRIORITY;
typedef unsigned int EVENT;      // always non-zero if it is valid
typedef unsigned int TICK;

// void OS_Init(void);      redefined as main()
void OS_Abort(void);

//PID  Task_Create( void (*f)(void), PRIORITY py, int arg); //implement by changing task_create
void Task_Terminate(void); //done
void Task_Yield(void);//done
int  Task_GetArg(void);//what?
void Task_Suspend( PID p );  //implement using mirrored states via suspension flag on process descriptor    
void Task_Resume( PID p ); //implement  using mirrored states via suspension flag on process descriptor 

void Task_Sleep(TICK t);  // sleep time is at least t*MSECPERTICK implement using sleep queue

MUTEX Mutex_Init(void); //Do mutex at end.
void Mutex_Lock(MUTEX m);
void Mutex_Unlock(MUTEX m);

EVENT Event_Init(void);//Implement using event queue?
void Event_Wait(EVENT e);
void Event_Signal(EVENT e);

typedef void (*voidfuncptr) (void); 







/**
  *  This is the set of states that a task can be in at any given time.
  */
typedef enum process_states 
{ 
   DEAD = 0, 
   READY,
	BLOCKED,
   RUNNING 
} PROCESS_STATES;

/**
  * This is the set of kernel requests, i.e., a request code for each system call.
  */
typedef enum kernel_request_type 
{
   NONE = 0,
   CREATE,
   NEXT,
   TERMINATE
} KERNEL_REQUEST_TYPE;

typedef struct ProcessDescriptor PD;

struct ProcessDescriptor 
{
   unsigned char *sp;   /* stack pointer into the "workSpace" */
   unsigned char workSpace[WORKSPACE]; 
   PROCESS_STATES state;
   voidfuncptr  code;   /* function to be executed as a task */
   KERNEL_REQUEST_TYPE request;
	
	//Added by Brendan
	TICK tick;
	PRIORITY priority;
	PID pid;
	
	PD* next;
};

typedef struct
{
	/** The first item in the queue. NULL if the queue is empty. */
	PD*  head;
	/** The last item in the queue. Undefined if the queue is empty. */
	PD*  tail;
}
queue_t;
#endif /* _OS_H_ */
