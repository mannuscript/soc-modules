The code is for the simulation of telephony system given to us as assignment for module: CS5233, SIMULATION AND MODELLING TECHNIQUES.
Noting fancy, this can be done in 15-20 mins using arena, however code would give you better understanding of simulation process and implementing "next-event time advance approach" simulation.
Note: Let me know, If you mind (or planning to sue) having this problem statement being online on public repo. Email ID: mannu1200@gmail.com

Problem statement:
Simulation and Analysis of a Telephony system
 
Background and Problem
 
In telephony systems different models are used to evaluate the quality of service (QoS). Consider a model consisting of two nodes connected to each other by a number of communication channels. Typically, a node represents a city and channels are the available telephone lines. A telephone call may originate in any of the two nodes at any time. If there is a free channel available the originating call is established and one of the free channels is allocated to the call. The channel allocation lasts throughout the whole telephone conversation and no other call may use that channel until the conversation is terminated. If all channels are occupied the originating call is blocked. The caller has to hang up and try again later. In some older models the channels can operate in one direction only. In such systems, a predefined number of channels are used in one direction and the rest of the channels are used to initiate calls in the other direction. In the newer systems, all channels can operate in both directions.
 
Your assignment
 
A system like the one described above is used by two cities, X and Y. The system is about to be upgraded from an older type where the channels are unidirectional to a newer type where the channels are bi-directional. The system designers want to create a simulation model of the new system to predict its performance. The following data has been collected from the current system:
 
Times between attempted calls from X to Y  are exponentially distributed with a mean of 12 seconds
Times between attempted calls from Y to X are exponentially distributed with a mean of 15 seconds
Length of conversations are exponentially distributed with a mean of 5 minutes
 
The designers wish to address the following questions:
 
How many channels would be needed in the old system to achieve a QoS guarantee of maximum 5% blocked calls, if there were equal number of channels in each direction?
How could the old system be configured to reduce the number of blocked calls further if it is allowed to assign different number of channels to each direction? 
For the new system, how many channels are needed to provide the same QoS guarantee of maximum 5% blocked calls? How are the channels utilized in this case?
