
/* The Single-Server Queue System */

/* External definitions for single-server queue system */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rand.h"	/* header file for random-number generator. */

#define Q_LIMIT 100	/* Limit on queue length */
#define BUSY 	1	/* Mnemonics for server's being busy */
#define IDLE	0	/* and idle */
#define __INFINITY 1.0e+30

int	next_event_type, num_custs_delayed, num_delays_required, num_events,
	num_in_q, 
  serverToDepart, //To share the information between depart function and timing function
  num_of_servers; //As the name suggest 
float	area_num_in_q, area_server_status, mean_interarrival, mean_service,
	current_time, time_arrival[Q_LIMIT + 1], time_last_event, 
	next_arrival_time, total_of_delays,
  **servers; //Servers is a 2D matrix where 1st column represent status and second column holds the depart time of the job being processed by corresponding server;

void	initialize (void);
void	timing(void);
void	arrive(void);
void	depart(void);
void	report(void);
void	update_time_avg_stats(void);
float	expon(float mean);

int main()	/* main function */
{
   /* specify the number of events for the timing function */
   num_events = 2;

   /* read input parameters */
   printf("Input mean interarrival, mean service, no. of delays required and number of servers\n");
   scanf("%f %f %d %d", &mean_interarrival, &mean_service, 
            &num_delays_required, &num_of_servers);

   /* write report heading and input parameters */
   printf("Multi-server single-queueing system\n\n");
   printf("Mean interarrival time%11.3f minutes\n\n", mean_interarrival);
   printf("Mean service time%16.3f minutes\n\n", mean_service);
   printf("Number of customers%14d\n\n", num_delays_required);
   printf("Number of servers: %d\n", num_of_servers);

   /* Initialise the simulation */

   initialize();
 
   /* run the simulation while more delays are still needed */
   while (num_custs_delayed < num_delays_required) 
   {
     /* determine the next event */
     timing();

     /* update time-average statistical accumulators */
     update_time_avg_stats();

     /* invoke the appropriate event function */
     switch (next_event_type)
     {
        case 1: arrive();
                break;
        case 2: depart();
                break;
     }
   }

   /* invoke the report generator and end the simulation */
   report();
 
   return 0;
}


void initialize(void) /* initialization function */
{
   /* initialise the simulation clock */
   current_time = 0.0;

   /* initialise the state variables */

   servers = malloc(sizeof(int*)*num_of_servers);
   for(int i=0; i< num_of_servers; i++) 
   {  
    servers[i] = malloc(sizeof(float*)*2);
    servers[i][0] = IDLE;
    servers[i][1] = __INFINITY;
   }
   num_in_q = 0;
   time_last_event = 0.0;
 
   /* initialise the statistical counters */

   num_custs_delayed = 0;
   total_of_delays = 0.0;
   area_num_in_q = 0.0;
   area_server_status = 0.0;

   /* initialise event list. Since no customers are present, the departure
      (service completion) event is eliminated from consideration */
   next_arrival_time = current_time + expon(mean_interarrival);
}


void timing(void) /* timing function */
{
   int i;
   float min_time_next_event = 1.0e+29;
   
   next_event_type = 0;

   /* determine the event type of the next event to occur either from list of depart times or arrrival time*/
    //TODO: remove the usage of num_events

   //this will find the coming depart time
   for (i=0; i <num_of_servers; ++i)
   {
     if (servers[i][1] < min_time_next_event)
       {
         min_time_next_event = servers[i][1];
         next_event_type = 2; //TODO: use #define for this
         serverToDepart = i;
       }
   }
   //check if next arrival time is before coming depart time
   if(next_arrival_time < min_time_next_event) {
    min_time_next_event = next_arrival_time;
    next_event_type = 1;
   }
   
   
   /* check to see whether the event list is empty */
   if (next_event_type == 0)
     {
       /* the event list is empty, so stop the simulation */
       printf("\nEvent list empty at time %f", current_time);
       exit(1);
     }

   /* the list is not empty, so advance the simulation clock */
   current_time = min_time_next_event;
}


void arrive(void) /* Arrival event function */
{
  float delay;

  /* schedule next arrival */
  next_arrival_time = current_time + expon(mean_interarrival);

  int 
    areServersBusy = 1,
    freeServerNumber;

  /* check to see whether server is busy */
  for(int i = 0; i< num_of_servers; i++)
  {
    if(servers[i][0] == IDLE)
    {
      areServersBusy = 0;
      freeServerNumber = i;
      break;
    }
  }
  if (areServersBusy) 
  {
    /* servers are busy, so increment no. of customers in queue */
    ++num_in_q;
   
    /* check to see whether an overflow condition exists */

    if (num_in_q > Q_LIMIT)
    { 
      /* the queue has overflowed, so stop the simulation */
      printf("\nOverflow of the array time_arrival at time %f\n", current_time);
      exit (2);
    }

    /* there is still room in the queue, so store the time of arrival of the
       arriving customer at the (new) end of time_arrival */
    time_arrival[num_in_q] = current_time;
  }
  else
  {
    /* server is idle, so arriving customer has a delay of zero 
       (the following two statements are for program clarity and do not 
       affect the results of the simulation)  */   
    delay = 0.0;
    total_of_delays += delay;

    /* increment the no. of customers delayed, and make server busy */
    ++num_custs_delayed;
    servers[freeServerNumber][0] = BUSY;

    /* schedule a departure (service completion) */
    servers[freeServerNumber][1] = current_time + expon(mean_service);
  }

      //Uncomment for debugging:
/*
  printf("Current time: %f     nextArrivalTime: %f         DepartTime: %d\n", current_time, next_arrival_time, freeServerNumber);
    for(int i=0; i<num_of_servers; i++)
      printf("%f %f             ", servers[i][0], servers[i][1]);
    printf("\n----------------");
    */
}



void depart(void) /* Departure event function */
{
  int i;
  float delay;

  /* check to see whether the queue is empty */
  if (num_in_q == 0)
  {
    /* the queue is empty, so make server idle and eliminate the departure
       event from consideration */
    servers[serverToDepart][0] = IDLE;
    servers[serverToDepart][1] = __INFINITY;
  }
  else 
  { 
    /* the queue is not empty, so decrement the no. of customers in queue */
    --num_in_q;

    /* compute the delay of customer who is beginning service and update 
       the total delay accumulator */
    delay = current_time - time_arrival[1];
    total_of_delays += delay;
    
    /* increment the no. of customers delayed, and schedule departure */
    ++num_custs_delayed;
    //This server would be serving the new customer, sorry no break during office hours!
    //Status will remain busy
    servers[serverToDepart][1] = current_time + expon(mean_service);

    /* move each customer in queue (if any) up one place */
    for (i= 1; i <= num_in_q; ++i)
      time_arrival[i] = time_arrival[i+1];
  }
}


void report(void) /* report generator function */
{
   /* compute and write estimates of desired measures of performance */

   printf("\nAverage delay in queue%11.3f minutes\n\n",
      total_of_delays/ num_custs_delayed);
   printf("Average number in queue%10.3f\n\n", area_num_in_q/current_time);
   printf("Server Utilization%15.3f\n\n", area_server_status/current_time);
   printf("Time simulation ended%12.3f\n", current_time);
}


void update_time_avg_stats(void) /* update area accumulators for time-average
                                    statistics */
{
   float time_since_last_event;

   /* Compute time since last event, and update last-event-time marker */
   time_since_last_event = current_time - time_last_event;
   time_last_event = current_time;

   /* update area under number-in-queue function */
   area_num_in_q += num_in_q * time_since_last_event;
 
   /* update area under server-busy indicator function */
   for(int i=0; i< num_of_servers; i++)
   {
    area_server_status += servers[i][0] * time_since_last_event;
   }
}


float expon(float mean) /* exponential variate generation function */
{
  float u;

  /* generate a U(0,1) random veriate */
  u = randno(1);

  /* return an exponential random variate with mean "mean" */
  return -mean*log(u);
}