/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Name : job_t
  Description :Stores information making up a job to be scheduled including any statistics.
*/

typedef struct _job_t
{
  int job_number;       /* job id */
  int arrival_time;     /* time arrived at the queue */
  int priority;         /* priority of the job */
  int runtime;          /* total time for the job to complete */
  int time_remaining;   /* time remaining before the job finishes */
  int start_time;       /* time that the job is added to the core and started */
  int response;         /* time that the job first has a response */
  bool flag;            /* checks whether the job has used any cpu time */

} job_t;

priqueue_t Q; /* Queue for the Program */
job_t ** corearr; /* Array for the cores in the program */

/*
Name: ShortestJobFirst(const void * x, const void * y)

Description: Sorts the provided jobs by which is the shortest

Return: Returns a 1 if the time remaining is greater in the second job
  and returns a -1 if the time remaining is the is greater in the fist job
  and returns a 0 if the time remaining is the same for both Jobs

 */
int ShortestJobFirst(const void * x, const void * y)
{
        /* returns a 1 if the time remaining is greater in the second job */
        if( (*(job_t*)x).time_remaining > (*(job_t*)y).time_remaining) //if the first arrived later return the second
        {
                return 1;
        }
        /* returns a -1 if the time remaining is the is greater in the fist job */
        else if ((*(job_t*)x).time_remaining < (*(job_t*)y).time_remaining)// if the first arrived earlier return the first
        {
                return -1;
        }
        else
        /* returns a 0 if the time remaining is the same for both Jobs */
        {
                return 0;
        }
}

/*
Name: PriorityFirst(const void * x, const void * y)

Description: Sorts the provided jobs by which has the greater priority

Return: Returns a 1 if the priority is greater in the second job
  and returns a -1 if the priority is the is greater in the fist job
  and returns a 0 if the priority is the same for both Jobs

 */
int PriorityFirst(const void * x, const void * y)
{
        if( (*(job_t*)x).priority > (*(job_t*)y).priority)
        /* If the first has a larger priority value, return the second */
        {
                return 1;
        }
        else if ( (*(job_t*)x).priority < (*(job_t*)y).priority)
        /* If the first has a lower priority value, return the first */
        {
                return -1;
        }
        else
        {
                if( (*(job_t*)x).arrival_time > (*(job_t*)y).arrival_time)
                /* if the first arrived later return the second */
                {
                        return 1;
                }
                else if ((*(job_t*)x).arrival_time < (*(job_t*)y).arrival_time)
                /* if the first arrived earlier return the first */
                {
                        return -1;
                }
                else
                /*returns a zero if the priority and then arrival time are the same */
                {
                return 0;
                }
        }
}

/*
Name: FirstComeFirstServe(const void * a, const void * b)

Description: Sorts the provided jobs by which came first

Return: Returns a 1
*/
int FirstComeFirstServe(const void * a, const void * b)
{
        return 1;
}

/*
Name: hasjobfinished(job_t * job)

Description: Returns whether the job has finished

Return: Returns true if remaining_time is 0, else a false
*/
bool hasjobfinished(job_t * job)
{
        /* Returns true if remaining_time is 0, else a false */
        if (job->time_remaining == 0)
        {
                return true;
        }
        else
        {
                return false;
        }
}

/*
Name: scheduler_core_available(job_t * newjob)

Description: Returns whether there is a core that is idle

Return: Returns the index of an avaliable core
        else, returns -1
*/
int scheduler_core_available(job_t* newjob)
{
        int a = 0;
        /* Loops to check if there is a core avaliable */
        while( a < ncores  )
        {
                /* Checks if the core is idle */
                if(corearr[a] ==  NULL)
                {
                        corearr[a] = newjob;
                        /* Returns the index of the available core */
                        return a;
                }
                a++;
        }
        /* If no core is available then return -1 */
        return -1;
}


/**
  Name: scheduler_start_up(int cores, scheme_t scheme)

  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{

        /*assign the type of comprison with the initialization of the queue*/
        if(scheme == FCFS)
        {
                priqueue_init(&Q, FirstComeFirstServe);
        }
        if(scheme == RR)
        {
                priqueue_init(&Q, FirstComeFirstServe);
        }
        if(scheme == SJF)
        {
                priqueue_init(&Q, ShortestJobFirst);
        }
        if(scheme == PSJF)
        {
                priqueue_init(&Q, ShortestJobFirst);
        }
        if(scheme == PRI)
        {
                priqueue_init(&Q, PriorityFirst);
        }
        if(scheme == PPRI)
        {
                priqueue_init(&Q, PriorityFirst);
        }

        /* Initialize Global variables to 0 */
        wait_time = 0;
        turnaround_time = 0;
        response_time =0;
        nJobs = 0;
        type = scheme;

        /* Initialize the number of cores to the cores parameter */
        ncores = cores;
        /* Create an int holding the size of a job_t */
        int sojt = sizeof(job_t);
        /* Create an int to hold the amount of total alloment for cores that will be needed */
        int allocamount = ncores *sojt;
        /* Allacote an array of job_t to corearr */
        corearr = (job_t**)malloc(allocamount);
        int a;
        /* Loop through the amount of cores and set them in corearr to NULL */
        for (a =0; a < ncores; a++)
        {
                corearr[a] = NULL;//NULL
        }
}

/**
  Name: scheduler_new_job(int job_number, int time, int running_time, int priority)

  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.

 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
        /* Create a newjob job_t */
        job_t* newjob = malloc(sizeof(job_t));
        /* Set the values of the parameters to the parameters inside of newjob */
        newjob->priority = priority;
        newjob->runtime = running_time;
        newjob->time_remaining = running_time;
        newjob->start_time = time;
        newjob->arrival_time = time;
        newjob->job_number = job_number;
        newjob->response = INT_MIN;

        /* Create an int to house if there is available core */
        int resp = scheduler_core_available(newjob);
        /* If there is a core available */
        if (resp != -1 )
        {
                newjob->response = 0;
                /* returns the index of the core that newjob was placed inside */
                return resp;
        }
        /* If there is no core available */
        else
        {
                if(type == PSJF)
                {
                        /* Find the core with the max remaining time */
                        int max_remaining_time = INT_MIN;
                        int index_of_max = 0;
                        /* Loops through the cores */
                        for(int i=0; i<ncores;i++)
                        {
                                /*Update the remaining time for each core to find max */
                                corearr[i]->time_remaining -= (time - corearr[i]->start_time);

                                /* If a new max is found, update location and value of max */
                                if(corearr[i]->time_remaining > max_remaining_time)
                                {
                                        max_remaining_time = corearr[i]->time_remaining;
                                        index_of_max = i;
                                }
                        }
                        /* If the new job's remaining time is less than the chose core */
                        if(newjob->time_remaining < corearr[index_of_max]->time_remaining)
                        {
                                /* Add the currently executing job to the queue */
                                priqueue_offer(&Q, corearr[index_of_max]);
                                /* Put the new job in the core */
                                newjob->response = 0;
                                corearr[index_of_max] = newjob;
                                return index_of_max;
                        }
                }
                if (type == PPRI)
                {
                        /* Find the core with the max remaining time */
                        int max_priority_num = INT_MIN;
                        int index_of_max = 0;
                        /* Loops through the cores */
                        for(int i=0; i<ncores;i++)
                        {

                                /* Update the remaining time for each core (need to do this to calculate response time) */
                                corearr[i]->time_remaining -= (time - corearr[i]->start_time);

                                if(corearr[i]->priority > max_priority_num)
                                {
                                        /* If a new max is found, update location and value of max */
                                        max_priority_num = corearr[i]->priority;
                                        index_of_max = i;
                                }
                                /* If the priorities are the same then check the arrival time */
                                else if(corearr[i]->priority == max_priority_num)
                                {
                                        if(corearr[i]->arrival_time > corearr[index_of_max]->arrival_time)
                                        {
                                                max_priority_num = corearr[i]->priority;
                                                index_of_max = i;
                                        }
                                }
                        }
                        /* If the new job's remaining time is less than the chose core */
                        if(newjob->priority < corearr[index_of_max]->priority)
                        {
                                /*Add the currently executing job to the queue*/
                                priqueue_offer(&Q, corearr[index_of_max]);
                                /*Put the new job in the core*/
                                newjob->response = 0;
                                corearr[index_of_max] = newjob;
                                return index_of_max;
                        }
                }
        }
        /* Add the new job to the Queue */
        priqueue_offer(&Q, newjob);
        return -1;
}


/**
  Name: scheduler_job_finished(int core_id, int job_number, int time)
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience.
  You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
        /* Grab the finished job from the core */
        job_t* finished_job = corearr[core_id];

        /* Calculate the different time measurements from the finished job */
        wait_time       += (time - finished_job->runtime - finished_job->arrival_time);
        turnaround_time += (time - finished_job->arrival_time);
        response_time += finished_job->response;
        /* Increment the number of Jobs */
        nJobs++;

        /* Cleanup the job */
        free(finished_job);
        finished_job = NULL;
        corearr[core_id] = NULL;

        /* Schedule the next job to the core */
        /* Check if the Queue is empty */
        if(priqueue_peek(&Q) != NULL)
        {
                /* Grab the job at the top of the queue and add it to the core */
                job_t* new_job = (job_t*)priqueue_poll(&Q);
                new_job->start_time = time;

                /* Change the response time if it hasn't been redefined */
                if(new_job->response == INT_MIN)
                {
                        new_job->response = time - new_job->arrival_time;
                }
                else if(new_job->time_remaining == new_job->runtime)
                {
                        new_job->response = time - new_job->arrival_time;
                }
                /* Change the job in the core */
                corearr[core_id] = new_job;
                /* return the job number for the new job */
                return new_job->job_number;
        }
        /* if the Queue is empty, the core remains idle */
        else
        {
                return -1;
        }
}


/**
  Name: scheduler_quantum_expired(int core_id, int time)
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{

        priqueue_offer(&Q,corearr[core_id]);
        job_t * current_job = priqueue_poll(&Q);

        /* Check if the Queue is empty */
        if(current_job != NULL)
        {
                /* Add the expired job to the queue, and schedule the next job on the queue */
                corearr[core_id]->time_remaining -= (time - corearr[core_id]->start_time);
                current_job->start_time = time;
                corearr[core_id]= current_job;

                /* If the response variable has not been set yet then redife it */
                if(corearr[core_id]->response == INT_MIN)
                {
                        corearr[core_id]->response = time - corearr[core_id]->arrival_time;
                }
                return current_job->job_number;
        }
        /* Return -1 if the Queue is empty and remain idle */
        else
        {
                return -1;
        }
}


/**
  Name: scheduler_average_waiting_time()
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
        float sawt = (float)wait_time / (float)nJobs;
        return sawt;
}


/**
  Name: scheduler_average_turnaround_time()
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
        float avgtt = (float)turnaround_time / (float)nJobs;
        return avgtt;
}


/**
  Name: scheduler_average_response_time()
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
        float respavg = (float)response_time / (float)nJobs;
        return respavg;
}


/**
  Name: scheduler_clean_up()
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
        int x =0 ;
        /* Loop through the cores */
        while (x < ncores)
        {
                if(corearr[x] == NULL)
                {
                        /*do nothing beacause no job_task inside */
                }
                else
                {
                        /* free the core */
                        free(corearr[x]);
                }
                x++;
        }

        /* Free the corearr and Q structures */
        free(corearr);
        priqueue_destroy(&Q);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{


}
