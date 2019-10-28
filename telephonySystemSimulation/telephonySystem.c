#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"customMath.h"

#define INFINITE 1.0e+30
#define NODE1 0
#define NODE2 1
#define CALL 1
#define TERMINATE 2
#define MAX_CHANNELS 1000

/*
	Assumptions:
	Number of channels can not go beyond 1k
	Simulation is working in single thread, both nodes are working on a single thread of C
	There is no bug in the code :p 
*/

/*
	List of channels will be in descending sorted order, Every call terminate operation would be of O(1)
*/

int debug;

//Event variable
int
	nextEventNode, //could be node1/node2
	nextEventType, //could be call/terminate
	observationTime,
	numberOfChannels[2] = {0,0},
	iterators[] = {0,0}; //iterator would be used hold the decision whether to increase the channels or decrease the channels for given nodes
float
	time,
 	nextEventTime,
 	nextCallTimes[2],
 	meanInterarrival[] = {0.2,0.25},
	meanService = 5.0,
	//QOS signifies the percentage of calls dropped
	qos = 100,
	qosNode1 = 100,
	qosNode2 = 100;


 struct list
 	channelList[2]; //list of channels, Every list would represent channels from that node to another


//Statistical data
int
	numberOfCallsPicked[2],
	numberOfCallsDropped[2];   //array for two node

//function declarations 
void init(void);	//Initializes the global parameters
void rumSimulation(void);
void call(void);
void terminate(void);
void getStats(void);
void freeUpAChannel(int);
void occupyAChannel(int, float);
void eventManager(void);
int comp(const void *, const void *);

int main(void)
{

	//Take inputs
	printf("Input the simulation running time(Hours) :");
	scanf("%d", &observationTime);
	printf("Do you want to run in debug mode(1/0)?");
	scanf("%d",&debug);

	for(int i=NODE1; i<=NODE2; i++) 
	{
		channelList[i].channels = malloc(sizeof(float*) * MAX_CHANNELS);
	}

	//*************************************************Part A***********************************************
	//Running the simulation until we get QOS <= 5%
	while(qos > 5) 
	{

		//Increment the number of channels, till we reach QOS =< 5%
		numberOfChannels[NODE1]++;
		numberOfChannels[NODE2]++; 

		rumSimulation();
	}

	//*************************************************Part B***********************************************
	//Now we have found the individual qos, and cumulative qos < 5, Lets try to reduce even further 
	//Decide for which node needs more channels and which one needs less
	if(qosNode1 < 5 && qosNode2 > 5)
	{
		iterators[NODE1] = -1;
		iterators[NODE2] = 1; 
	} else if(qosNode1 > 5 && qosNode2 < 5) 
	{
		iterators[NODE1] = 1;
		iterators[NODE2] = -1; 
	} else { //crap, both less than 5
		if(qosNode1 < qosNode2)
		{
			iterators[NODE1] = -1;
			iterators[NODE2] = 0; 
		} else {
			iterators[NODE1] = 0;
			iterators[NODE2] = -1; 
		}
		//could not be the case qos1 = qos2

	}
	//Lets run the simulation again to reduce qos even further for numberOfChannels[NODE1] != numberOfChannels[NODE2]
	float oldQos = qos;
	do {
		numberOfChannels[NODE1] += iterators[NODE1];
		numberOfChannels[NODE2] += iterators[NODE2];
		rumSimulation();
	} while(qos > oldQos);




	//*************************************************Part C***********************************************


	return 0;
}

void rumSimulation(void)
{
	init();
	eventManager();

	while(time < observationTime*60)
	{
		switch(nextEventType)
		{
			case CALL: call();
				break;
			case TERMINATE: terminate();
				break;
		}
		//update data structures for next event
		eventManager();
		//printf("\nProgress: currentTime: %f, observationTime: %d, next event time: %f, next event type: %d ,next event node: %d\n", 
		//	time, observationTime, nextEventTime, nextEventType, nextEventNode);
	}
	getStats();
}

//Initialize 
void init(void)
{
	time = 0.0;
	
	nextEventType = CALL;
	nextCallTimes[NODE1] = expon(meanInterarrival[NODE1]);
	nextCallTimes[NODE2] = expon(meanInterarrival[NODE2]);

	if(nextCallTimes[NODE1] < nextCallTimes[NODE2]) {
		nextEventNode = NODE1;
		nextEventTime = nextCallTimes[NODE1];
	} else {
		nextEventNode = NODE2;
		nextEventTime = nextCallTimes[NODE2];
	}
	//TODO: check problem with following line of code
	//channelList = malloc(sizeof(struct list*)*2);
	for(int i=NODE1; i<= NODE2; i++) 
	{
		numberOfCallsPicked[i] = 0;
		numberOfCallsDropped[i] = 0;
		channelList[i].length = numberOfChannels[i];
		channelList[i].ptr = -1;
		channelList[i].utilization = 0;
		//Initialize the channels with infinite
		for(int j=0; j < channelList[i].length; j++) {
			channelList[i].channels[j] = INFINITE;
			if(debug)
				printf("\n\n\nDebug point: INIT:\n Initializing the channels:\n channelList[%d].channels[%d]=%f \n\n\n",i,j,channelList[i].channels[j]);
		}
	}
}

//Decide the next event 
void eventManager(void)
{

	/*
	Get next event, four possibility:
		i) Call initiation from node 1
		ii) Call initiation from node 2
		iii) Call termination from node 1
		iv) Call termination from node 2
	*/

	//TODO: find a better approach for this!
	//eventManager is being called at the start of the simulation when both ptrs are set to -1
	//To save our ass from segmentation fault (Accessing channels[-1] we need the followingg hack)
	int 
		ptr1 = channelList[NODE1].ptr == -1 ? 0 : channelList[NODE1].ptr,
		ptr2 = channelList[NODE2].ptr == -1 ? 0 : channelList[NODE2].ptr;

	switch(findTheSmallestAmongFour(nextCallTimes[NODE1], nextCallTimes[NODE2], 
		channelList[NODE1].channels[ptr1],
		channelList[NODE2].channels[ptr2]))
	{
		case 1: 
			nextEventNode = NODE1;
			nextEventType = CALL;
			nextEventTime = nextCallTimes[NODE1];
			nextCallTimes[NODE1] = INFINITE;
			break;
		case 2: 
			nextEventNode = NODE2;
			nextEventType = CALL;
			nextEventTime = nextCallTimes[NODE2];
			nextCallTimes[NODE2] = INFINITE;
			break;
		case 3:
			nextEventNode = NODE1;
			nextEventType = TERMINATE;
			nextEventTime = channelList[NODE1].channels[channelList[NODE1].ptr];
			break;
		case 4:
			nextEventNode = NODE2;
			nextEventType = TERMINATE;
			nextEventTime = channelList[NODE2].channels[channelList[NODE2].ptr];
			break;
	}
	time = nextEventTime;
}

/*
	Set up a call or drop it
*/
void call(void)
{
	
	float callTerminationTime = time + expon(meanService);
	//Check if there is any free channel 
	if(channelList[nextEventNode].ptr <  channelList[nextEventNode].length-1)
	{
		channelList[nextEventNode].ptr++;
		occupyAChannel(nextEventNode, callTerminationTime);
		numberOfCallsPicked[nextEventNode]++;
	}
	else 
	{
		//Holy crap, call drop!
		numberOfCallsDropped[nextEventNode]++;
	}

	//Set the next call time
	nextCallTimes[nextEventNode] = time + expon(meanInterarrival[nextEventNode]);
}


//Terminate a call and free up the channel
void terminate(void)
{
	nextEventTime = channelList[nextEventNode].channels[channelList[nextEventNode].ptr];
	if(debug)
		printf("\n\n\nDebug Point:\nterminate\nnextEventTime:%f %d\n\n\n",nextEventTime,channelList[nextEventNode].ptr);
	//channelList[nextEventNode].utilization += ;
	freeUpAChannel(nextEventNode);
}

//Ignore this, its just helper function for sorting the list
//TODO: check if this can be moved into customMaths.h

int floatcomp(const void* elem1, const void* elem2)
{
    if(*(const float*)elem1 > *(const float*)elem2)
        return -1;
    return *(const float*)elem1 < *(const float*)elem2;
}

//The key is to keep the list of channels in descending order
void occupyAChannel(int node, float callTerminationTime)
{
	if(debug)
		printf("\n\n\n\n Debug point: occupyAChannel\n current node:%d\n ptr:%d \n length:%d \n call termination time:%f \n\n\n", node, 
		channelList[node].ptr, channelList[node].length, callTerminationTime);
	//insert into the list
	channelList[node].channels[channelList[node].ptr] = callTerminationTime;
	//and sort the list
	qsort(channelList[node].channels, channelList[node].ptr+1, sizeof(float), floatcomp);
}

//From tail
void freeUpAChannel(int node)
{
	if(debug)
		printf("\n\n\nDebug point:\nfreeUpAChannel\nchannelList[%d].channels[%d] \n\n\n",node,channelList[node].ptr);
	channelList[node].channels[channelList[node].ptr] = INFINITE;
	channelList[node].ptr--;
}

void getStats(void)
{
	qosNode1 = (float)numberOfCallsDropped[0]*100/(numberOfCallsDropped[0]+numberOfCallsPicked[0]);
	qosNode2 = (float)numberOfCallsDropped[1]*100/(numberOfCallsDropped[1]+numberOfCallsPicked[1]);
	qos = (qosNode1 + qosNode2)/2;
	if(qos < 5)
	printf("\n\nThis program is going to terminate now\nNumber of calls picked:\nNode1: %d\n Node2: %d\n\n Number of calls dropped:\nNode1:%d\nNode2:%d\nQoS node1:%f\nQoS node2:%f\nTotal qos: %f\nNumber of Channels for node1:%d\nNumber of channels for node2: %d\nDuration of simulation:%d\n", 
		numberOfCallsPicked[0], 
		numberOfCallsPicked[1], 
		numberOfCallsDropped[0], numberOfCallsDropped[1], qosNode1, qosNode2, qos, numberOfChannels[NODE1], numberOfChannels[NODE2], observationTime);
	printf("\nTime simulation ended:%f",time);
	//printf("\nData for Graph 1:  %d,%f\n",numberOfChannels[NODE1],qos);

}