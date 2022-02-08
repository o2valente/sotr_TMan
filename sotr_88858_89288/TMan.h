/* Priorities of the demo application tasks (high numb. -> high prio.) */
#define print_PRIORITY  ( tskIDLE_PRIORITY + 1 )
#define tsk_PRIORITY	( tskIDLE_PRIORITY + 2 )
#define tics_PRIORITY	( tskIDLE_PRIORITY + 5 )

#define TMan_PERIOD 200

#define TASK_STATE_INITIAL 0
#define TASK_STATE_RUNNING 1
#define TASK_STATE_BLOCKED 2

#define IMAXCOUNT 12
#define JMAXCOUNT 12

/*Task Structure*/
typedef struct TMan_task {
    int period;         /*Task Period in TMan_tick*/
    int phase;          /*Task Phase in TMan_tick*/
    int deadline;       /*Task Deadline in TMan_tick*/
    int current_a;      /*Task Current Activation in TMan_tick*/
    int next_a;         /*Task Next Activation in TMan_tick*/
    int deadline_count; /*Number of Deadlines missed*/
    int activation_num; /*Number of activations of a Task*/
    int state;          /*Task State (TASK_STATE_INITIAL 0 | TASK_STATE_RUNNING 1 | TASK_STATE_BLOCKED 2)*/
    int conclusion;     /*Task Conclusion in TMan_tick*/
    int priority;       /*Task Priority*/
    int wcet;           /*Task Worst Case Execution Time*/
    const char* precedence;  /*Task from which precedes it*/
    const char* name;        /*Task name*/
}TMan_task;


/*initialisation of the framework*/
void TMAN_Init (int);

/*terminate the framework*/
void TMAN_Close (void);

/*Add a task to the framework initializing it's attributes 
 Creates the task */
void TMAN_TaskAdd (const char*, int);

/*Register attributes (e.g. period, phase, deadline, precedence
constraints) for a task already added to the framework*/
/*Use the letter Z to assume no Precedence*/
void TMAN_TaskRegisterAttributes (const char*, int, int, int, int, const char*);

/*Called by a task to signal the termination of an instance and wait for
the next activation*/
void TMAN_TaskWaitPeriod (void);

/*Returns statistical information about a task. Provided information must
include at least the number of activations, but additional info (e.g. number of deadline misses)
will be valued. */
void TMAN_TaskStats (void);

/* Verify the Feasibility of the systems, processing the data from all tasks */
int TMAN_Feasibility (void);

/* Calculates the Worst Case Scenario Times from all tasks, regarding the 
 existence of higher priority ones, until convergence or deadline miss*/
int TMAN_Wcet(TMan_task task);

/*Checks if a task has precedence, if so this task becomes sporadic
 If the precedent task is in BLOCKED state and has reached is conclusion then the task is allowed to run
 If a task does not have precedence then it checks if it can run, if it can checks if it has crossed a deadline and 
 updates accordingly*/
void TMAN_getTicks (void*);

/*Calls TMAN_TaskWaitPeriod
 Sends to the terminal the respective Task Name and the current Tic
 Runs some calculations to consume time
 In the end updates the task state to BLOCKED and saves the Tic of it's conclusion*/
void BusyWait (void*);

/*Allows the program to send messages orderly to the terminal according to a Queue*/
void printTask(void*);

/*Returns a task's index in the global array according to it's name*/
int getTaskIndex(const char*);

/* Checks if a task is sporadic or periodic and according to that updates the current activation and next activation.
 * Updates a given task's number of activations and state aswell as change its state to RUNNING. 
 * After the updates resumes the task.*/
void updateTask(int , BaseType_t);

/*Initializes the UART*/
void uartInit(void);

