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

typedef unsigned char PRIORITY;
typedef unsigned int EVENT;      // always non-zero if it is valid
typedef unsigned int TICK;
typedef unsigned int MUTEX;


// void OS_Init(void);      redefined as main()
void OS_Abort(void);

PID  Task_Create( void (*f)(void), PRIORITY py, int arg); //DONE
void Task_Terminate(void); //DONE
void Task_Yield(void);//DONE
int  Task_GetArg(void);//DONE
void Task_Suspend( PID p );  //implement using mirrored states via suspension flag on process descriptor    
void Task_Resume( PID p ); //implement  using mirrored states via suspension flag on process descriptor 

void Task_Sleep(TICK t);  // GOUDINE

MUTEX Mutex_Init(void); //Do mutex at end.
void Mutex_Lock(MUTEX m);
void Mutex_Unlock(MUTEX m);

EVENT Event_Init(void);//Implement using event queue?
void Event_Wait(EVENT e);
void Event_Signal(EVENT e);

void preemption();

typedef void (*voidfuncptr) (void); 

void a_main(void);

typedef enum mutex_states
{
	OPEN = 0,
	FREE,
	LOCKED
} MUTEX_STATES;


typedef struct create_args
{
	/** The code the new task is to run.*/
	voidfuncptr code;
	/** A new task may be created with an argument that it can retrieve later. */
	int arg;
	/** Priority of the new task: RR, PERIODIC, SYSTEM */
	PRIORITY py;
	
	PID pid;
}
create_args;
/**
  *  This is the set of states that a task can be in at any given time.
  */
typedef enum process_states 
{ 
   DEAD = 0, 
   READY,
   BLOCKED,
   RUNNING,
	WAITING,
   SLEEPING
} PROCESS_STATES;

/**
  * This is the set of kernel requests, i.e., a request code for each system call.
  */
typedef enum kernel_request_type 
{
   NONE = 0,
   CREATE,
   NEXT,
   TERMINATE,
   SLEEP,
   SUSPEND,
	YIELD,
	RESUME,
	LOCK,
	UNLOCK,
	EVENT_INIT,
	EVENT_SIGNAL,
	EVENT_WAIT,
   WAKE
} KERNEL_REQUEST_TYPE;

typedef struct ProcessDescriptor PD;

struct ProcessDescriptor 
{
   unsigned volatile char *sp;   /* stack pointer into the "workSpace" */
   unsigned char workSpace[WORKSPACE]; 
   PROCESS_STATES state;
   voidfuncptr  code;   /* function to be executed as a task */
   KERNEL_REQUEST_TYPE request;
	
	//Added by Brendan
	TICK tick;
	PRIORITY priority;
	PRIORITY past;
	PID pid;
	unsigned int arg;
	unsigned int arg2;
	unsigned int suspend;
	volatile PD* next;
};

typedef struct
{
	/** The first item in the queue. NULL if the queue is empty. */
	volatile PD*  head;
	/** The last item in the queue. Undefined if the queue is empty. */
	volatile PD*  tail;
}
queue_t;


typedef struct Mutex_Descriptor MD;

typedef struct Mutex_Descriptor
{
	MUTEX_STATES state;
	volatile PD* owner;
	queue_t* mutex_queue;
	volatile unsigned int count;
};

#endif /* _OS_H_ */
