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
	numberOfChannels = 0,
	channelsPtr,
	channelsLength
	;

float
	time,
 	nextEventTime,
 	nextCallTimes[2],
 	meanInterarrival[] = {0.2,0.25},
	meanService = 5.0,
	//QOS signifies the percentage of calls dropped
	qos = 100,
	qosNode1 = 100,
	qosNode2 = 100,
	numberOfTimesChannelsUsed[] = {0,0},
	channels[1000];

int	nodesAtChannels[1000];


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

	//*************************************************Part C***********************************************
	//Running the simulation until we get QOS <= 5%
	while(qos > 5) 
	{

		//Increment the number of channels, till we reach QOS =< 5%
		numberOfChannels++;

		rumSimulation();
	}


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
	
	channelsLength = numberOfChannels;
	channelsPtr = -1;
	//Initialize the channels with infinite
	for(int j=0; j < channelsLength; j++) {
		channels[j] = INFINITE;
		//let there be garbage value in the column for node number
		if(debug)
			printf("\n\n\nDebug point: INIT:\n Initializing the channels:\n channels[%d]=%f \n\n\n",j,channels[j]);
	}

	for(int i=NODE1; i<= NODE2; i++) 
	{
		numberOfCallsPicked[i] = 0;
		numberOfCallsDropped[i] = 0;
	}
}

//Decide the next event 
void eventManager(void)
{
	//TODO: add debugging points

	/*
	Get next event, four possibility:
		i) Call initiation from node 1
		ii) Call initiation from node 2
		iii) Call termination from node 1
		iv) Call termination from node 2
	*/

	//TODO: find a better approach for this!
	//eventManager is being called at the start of the simulation when both ptrs are set to -1
	//To save our ass from segmentation fault (Accessing channels[-1] we need the following hack)
	int 
		ptr = channelsPtr == -1 ? 0 : channelsPtr;

	switch(findTheSmallestAmongThree(nextCallTimes[NODE1], nextCallTimes[NODE2], channels[ptr]))
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
			nextEventNode = nodesAtChannels[channelsPtr];
			nextEventType = TERMINATE;
			nextEventTime = channels[channelsPtr];
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
	if(channelsPtr <  channelsLength-1)
	{
		channelsPtr++;
		occupyAChannel(nextEventNode, callTerminationTime);
		numberOfCallsPicked[nextEventNode]++;
		numberOfTimesChannelsUsed[nextEventNode]++;
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
	nextEventTime = channels[channelsPtr];
	if(debug)
		printf("\n\n\nDebug Point:\nterminate\nnextEventTime:%f %d\n\n\n", nextEventTime, channelsPtr);
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
		channelsPtr, channelsLength, callTerminationTime);
	//insert into the list
	channels[channelsPtr] = callTerminationTime;
	nodesAtChannels[channelsPtr] = node;
	//and sort the list
	qsort(channels, channelsPtr+1, sizeof(float), floatcomp);
}

//From tail
void freeUpAChannel(int node)
{
	if(debug)
		printf("\n\n\nDebug point:\nfreeUpAChannel\nchannelList[%d].channels[%d] \n\n\n", node, channelsPtr);
	channels[channelsPtr] = INFINITE;
	nodesAtChannels[channelsPtr] = 100;
	channelsPtr--;
}

void getStats(void)
{
	qosNode1 = (float)numberOfCallsDropped[0]*100/(numberOfCallsDropped[0]+numberOfCallsPicked[0]);
	qosNode2 = (float)numberOfCallsDropped[1]*100/(numberOfCallsDropped[1]+numberOfCallsPicked[1]);
	qos = (qosNode1 + qosNode2)/2;
	if(qos < 5) {
		printf("\n\nThis program is going to terminate now\nNumber of calls picked:\nNode1: %d\n Node2: %d\n\n Number of calls dropped:\nNode1:%d\nNode2:%d\nQoS node1:%f\nQoS node2:%f\nTotal qos: %f\nNumber of channels for nodes: %d\nDuration of simulation:%d\n", 
			numberOfCallsPicked[0], 
			numberOfCallsPicked[1], 
			numberOfCallsDropped[0], numberOfCallsDropped[1], qosNode1, qosNode2, qos, numberOfChannels, observationTime);
		printf("Utilization of channels:\nNode1:%f\nNode2:%f\n\n", numberOfTimesChannelsUsed[0]/(numberOfTimesChannelsUsed[1]+numberOfTimesChannelsUsed[0])*100, 
			numberOfTimesChannelsUsed[1]/(numberOfTimesChannelsUsed[0]+numberOfTimesChannelsUsed[1])*100);
	}
	//printf("\nData for Graph 2:  %d,%f\n",numberOfChannels,qos);

}