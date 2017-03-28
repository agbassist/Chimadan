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
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements.
*/
priqueue_t Q;


typedef struct
{
  int max_priority_num;
  int max_priority_index;
  int max_priority_pid;

} max_priority_vals;

typedef struct
{
  int max_remaining_time_num;
  int max_remaining_time_index;
  int max_remaining_time_pid;

} max_remaining_time_vals;


typedef struct _job_t
{
  int job_number;       //job id
  int arrival_time;     //time arrived at the queue
  int priority;         //priority of the job
  int runtime;          //total time for the job to complete
  int time_remaining;   //time remaining before the job finishes
  int start_time;       //time that the job is added to the core and started

} job_t;


job_t ** corearr; // job task array

priqueue_t Queue;
int printtime()
{
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  printf("\n\n\n\n\n\n\n");
  printf ( "Current local time and date: %s", asctime (timeinfo) );
  return 1;
}

printtime();


int ShortestJobFirst(const void * x, const void * y)
{
  if( (*(job_t*)x).time_remaining > (*(job_t*)y).time_remaining) //if the first arrived later return the second
  {
    printf("\n\n\n\n\n\n111111111111111111\n\n\n\n\n\n");
    return 1;
  }
  else if ((*(job_t*)x).time_remaining < (*(job_t*)y).time_remaining)// if the first arrived earlier return the first
  {
    return -1;
  }
  else
  {
    return 0;
  }

}
int PriorityFirst(const void * x, const void * y)
{
  if( (*(job_t*)x).priority > (*(job_t*)y).priority) //If the first has a larger priority value, return the second
  {
    return 1;
  }
  else if ( (*(job_t*)x).priority < (*(job_t*)y).priority)// If the first has a lower priority value, return the first
  {
    return -1;
  }
  else
  {
    if( (*(job_t*)x).runtime > (*(job_t*)y).runtime) //if the first arrived later return the second
    {
      return 1;
    }
    else if ((*(job_t*)x).runtime < (*(job_t*)y).runtime)// if the first arrived earlier return the first
    {
      return -1;
    }
    else
    {
      return 0;
    }


  }

}
int FirstComeFirstServe(const void * a, const void * b)
{
    return 1;
}

bool hasjobfinished(job_t * job)
{
  if (job->time_remaining == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

int scheduler_core_available(job_t* newjob)
{
  int a = 0;
  while( a < ncores  )
  {
    if(corearr[a] ==  NULL)
    {
      corearr[a] = newjob;

      return a;
    }
    a++;
  }

  return -1;
}
max_priority_vals Max_Priority_Finder(job_t* newjob)
{
  int a = 0;
  printf("\n%s\n","15" );
  max_priority_vals returnvals;
  returnvals.max_priority_num = INT_MIN;
  returnvals.max_priority_index = INT_MIN;
  returnvals.max_priority_pid = INT_MIN;

    printf("\n%s\n","16" );
    //int b = corearr[1]->priority;
    //printf("%s %i\n","cores:",ncores );
  while( a < ncores  )
  {
      printf("%s %i\n","maxvalue:",returnvals.max_priority_num );
      printf("%s %i\n","cores:",ncores );
      printf("%s %i\n","a:",a );
      printf("\n%s\n","17" );
    if(corearr[a]->priority > returnvals.max_priority_num)
    {
      printf("\n%s\n","19" );
      returnvals.max_priority_num = corearr[a]->priority;
      returnvals.max_priority_index =a;
      returnvals.max_priority_pid =  corearr[a]->job_number;


    }
    else if (corearr[a]->priority == returnvals.max_priority_num)
    {
      printf("\n%s\n","20" );
        if(corearr[a]->job_number > returnvals.max_priority_pid)
        {
          printf("\n%s\n","21" );
          returnvals.max_priority_num = corearr[a]->priority;
          returnvals.max_priority_index = a;
          returnvals.max_priority_pid = corearr[a]->job_number;
        }
    }
    else
    {
        //the priority is not the highest
    }
    a++;
  }
  printf("%s\n","18" );

  return returnvals;
}


max_remaining_time_vals Remaining_time_finder(int time)
{
  int a = 0;
  max_remaining_time_vals returnvals;
  int temp = 0;

  returnvals.max_remaining_time_num = INT_MAX;
  returnvals.max_remaining_time_index = INT_MAX;
  returnvals.max_remaining_time_pid = INT_MAX;

  while( a < ncores  )
  {
    int time_diff = 0;
    time_diff = time - corearr[a]->start_time;
    temp = corearr[a]->time_remaining - time_diff;

    if( temp < returnvals.max_remaining_time_num )
    {
      returnvals.max_remaining_time_num = temp;
      returnvals.max_remaining_time_index = a ;
      returnvals.max_remaining_time_pid = corearr[a]->job_number;
    }
    else if (corearr[a]->time_remaining == returnvals.max_remaining_time_num)
    {
        if(corearr[a]->job_number > returnvals.max_remaining_time_pid)
        {
          returnvals.max_remaining_time_num = temp;
          returnvals.max_remaining_time_index = a ;
          returnvals.max_remaining_time_pid = corearr[a]->job_number;
        }
    }
    else
    {
        //the priority is not the highest
    }
    a++;
  }

  return returnvals;
}



/**
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
  printf("\n%s\n","3" );
//printf("%s %n\n","Cores amount:", cores );
    //assign the type of comprison with the initialization of the queue
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

    wait_time = 0;
    turnaround_time = 0;
    response_time =0;
    nJobs = 0;
    type = scheme;

    ncores = cores;
    printf("%s %d\n","cores:",cores );
    int sojt = sizeof(job_t);
      printf("%s %d\n","sojt:",sojt );
    int allocamount = ncores *sojt;
    printf("%s %d\n","allocamount:",allocamount );
    corearr = (job_t**)malloc(allocamount);
    int a;
    for (a =0; a < ncores; a++)
    {
        printf("%s %d\n","a:",a );
      corearr[a] = NULL;//NULL
    }

      printf("\n%s\n","4" );




    //priqueue_init(Queue);

}


/**
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
    printf("\n%s\n","5" );

   	job_t* newjob = malloc(sizeof(job_t));
    newjob->priority = priority;
    newjob->runtime = running_time;
    newjob->time_remaining = running_time;
    newjob->start_time = time;
    newjob->arrival_time = time;
    newjob->job_number = job_number;



    max_priority_vals priority_vals;
      printf("\n%s\n","9" );


    int resp = scheduler_core_available(newjob);
    printf("\n%s %i\n","resp:",resp );

      printf("\n%s\n","10" );
    if (resp != -1 )
    {
        printf("\n%s\n","11" );
        return resp;
      //the new job has been added
    }
    else
    {
        printf("\n%s\n","12" );
      priority_vals = Max_Priority_Finder(newjob);
        printf("\n%s\n","14" );

        printf("\n%s\n","13" );

        if(type == PSJF){

            //update the time remaining in the current core
            corearr[0]->time_remaining -= (time - corearr[0]->start_time);
            if(newjob->time_remaining < corearr[0]->time_remaining){
                priqueue_offer(&Q, corearr[0]);
                corearr[0] = newjob;

                return 0;
            }
        }

      if ( type == PPRI)

      {
        printf("\n%s\n","23" );
        if( newjob->priority < priority_vals.max_priority_num)
        {
          printf("\n%s\n","24" );
          job_t * current_job = corearr[priority_vals.max_priority_index];
            printf("%s %d\n","priority_vals.max_priority_index:",priority_vals.max_priority_index );
          //printf("\n%s\n","25" );
          int temp_time;
          printf("\n%s\n","33" );
          temp_time = time - current_job->start_time;
            printf("%s %d\n","priority_vals.max_priority_index:",priority_vals.max_priority_index );
          //printf("\n%s\n","26" );

          int newtime = current_job->time_remaining - temp_time;
          printf("\n%s\n","27" );
          current_job->time_remaining = newtime;
          //printf("\n%s\n","28" );
          if (hasjobfinished(current_job))
          {
            printf("\n%s\n","29" );
            response_time += -1 *( time - current_job->arrival_time);
          }
        //  printf("\n%s\n","30" );
          priqueue_print(&Q);
          printf("\n%s\n","40" );
          priqueue_offer(&Q, current_job);
        //    printf("\n%s\n","41" );
            priqueue_print(&Q);
          printf("\n%s\n","31" );
          newjob->start_time = time;
        //  printf("\n%s\n","32" );
          return priority_vals.max_priority_index;
        }
      }

    }
  printf("\n%s\n","8" );
  	priqueue_offer(&Q, newjob);
    return -1;

}


/**
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
    //Grab the finished job from the core
    printf("\n%s\n","6" );
    job_t* finished_job = corearr[core_id];

    //Calculate the different time measurements from the finished job
    wait_time       += (time - finished_job->runtime - finished_job->arrival_time);
    turnaround_time += (time - finished_job->arrival_time);
    response_time   += (finished_job->start_time - finished_job->arrival_time);
    printf("-------------\n");
    printf("Start Time: %i \nArrival Time: %i\n",finished_job->start_time,finished_job->arrival_time);
    printf("-------------\n");
    nJobs++;

    //Cleanup the job
    free(finished_job);
    finished_job = NULL;
    corearr[core_id] = NULL;

    //Schedule the next job to the core

    if(priqueue_peek(&Q) != NULL){ //Check if the Queue is empty
        printf("\n%s\n","37" );
        //priqueue_print(&Q);


        //Grab the job at the top of the queue and add it to the core
        priqueue_print(&Q);
        job_t* new_job = (job_t*)priqueue_poll(&Q);

        new_job->start_time = time;
        corearr[core_id] = new_job;
        return new_job->job_number;
    }
    else{ //if the Queue is empty, the core remains idle
        printf("\n%s\n","25" );
        return -1;
    }
      printf("\n%s\n","7" );
}


/**
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

    if(current_job != NULL){ //Check if the Queue is empty
      corearr[core_id]->time_remaining -= time - corearr[core_id]->start_time;
      //Add the expired job to the queue, and schedule the next job on the queue
      //remaining
      //temp_job = priqueue_poll(&Q);
      current_job->start_time = time;
      corearr[core_id]= current_job;

      if(current_job->time_remaining - current_job->runtime == 0)
      {
        response_time -= (time - current_job->arrival_time);
      }

      return current_job->job_number;

}
    else{

      corearr[core_id]->time_remaining -= time - corearr[core_id]->start_time;
      //Return -1 if the Queue is empty and remain idle
      return -1;
  }



}


/**
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
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  int x =0 ;
  while (x < ncores)
  {
    if(corearr[x] == NULL)
    {
     //do nothing beacause no job_task inside
    }
    else
    {
       free(corearr[x]);

    }
    x++;
  }
  free(corearr);
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
