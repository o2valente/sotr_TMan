typedef struct TMan_task {
    int period;
    int phase;
    int deadline;
    //precedence constrains (?))
    char *name;
}TMan_task;


/*initialisation of the framework*/
void TMAN_Init (void);

/*terminate the framework*/
void TMAN_Close (void);

/*Add a task to the framework*/
void TMAN_TaskAdd (void);

/*Register attributes (e.g. period, phase, deadline, precedence
constraints) for a task already added to the framework*/
void TMAN_TaskRegisterAttributes (void);

/*Called by a task to signal the termination of an instance and wait for
the next activation*/
void TMAN_TaskWaitPeriod (void);

/*returns statistical information about a task. Provided information must
include at least the number of activations, but additional info (e.g. number of deadline misses)
will be valued. */
void TMAN_TaskStats (void);