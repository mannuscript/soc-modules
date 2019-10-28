mtype = { A, B, C, D };

//Channel for randomness
chan randomSpeed = [1] of {int};
chan randomEntryPoint = [1] of {mtype};
chan takeTurnTo[4] = [1] of {int};  //one channel for every type of turn

//Define dimensions of the roads
int 
lengthOfRoads[7] = {50,200,80,50,80,50,80};

//Cars parameters
	int 
	currentSpeed[2], 
	currentLocation[2], 
	currentDirection[2],
	currentRoad[2] = {0,1},
	isInMap[2],
	incremental[2], 
	turnTo[2];

inline map(carId)
{

	
	//Know the direction
	if
	::currentDirection[carId] == 1 -> incremental[carId] = 1;
	::else incremental[carId] = -1;
	fi

	//Now decide the steps to jump according to speed
	incremental[carId] = incremental[carId] * currentSpeed[carId]/5;
		
	//new location
	currentLocation[carId] = currentLocation[carId] + incremental[carId];

	//Debug point:
	printf("\n\nDEBUG:::::::: \ncar: %d \nLocation: %d \nRoad: %d\n LengthOfRoad: %d\n incremental: %d \nSpeed: %d\n\n\n", 
			carId, currentLocation[carId], currentRoad[carId], lengthOfRoads[currentRoad[carId]], incremental[carId], currentSpeed[carId]);

	if

	::currentRoad[carId] == 0 ->
		if
		::currentLocation[carId] > lengthOfRoads[currentRoad[carId]] ->
			//At turn: X1
			takeTurnTo[0]?turnTo[carId]; 	//Get the random turn
			if
			::turnTo[carId] == 1 ->
				//X1 to X2
				currentRoad[carId] = 1;
				currentLocation[carId] = 1;
				currentDirection[carId] = 1;
			::turnTo[carId] == 2 -> 
				//X1 to X3
				currentRoad[carId] = 3;
				currentLocation[carId] = 1;
				currentDirection[carId] = 1;
			::turnTo[carId] == 3 ->
				//X1 to X4
				currentRoad[carId] = 2;
				currentLocation[carId] = lengthOfRoads[2];
				currentDirection[carId] = 0;
			::else assert(1);
			fi
			//if-fi for turn to take

		::currentLocation[carId] < 0 -> 
			//Car took exit from exit A
			isInMap[carId] = 0;

		::else //ride on the same road
		fi
		//if-fi for currentLocation[carId] < >


	::currentRoad[carId] == 1 ->
		if
		::currentLocation[carId] > lengthOfRoads[currentRoad[carId]] ->
			//at turn : Z2
			takeTurnTo[1]?turnTo[carId];
			if
			::turnTo[carId] == 2 
				//Z2 to Z3
				currentRoad[carId] = 6;
				currentLocation[carId] = lengthOfRoads[6];
				currentDirection[carId] = 0;
			::turnTo[carId] == 3
				//Z2 to Z1
				currentRoad[carId] = 5;
				currentLocation[carId] = lengthOfRoads[5];
				currentDirection[carId] = 0;
			::else
			fi
			//if-fi for turn to take
		::currentLocation[carId] < 0 ->
			//at turn X2
			takeTurnTo[0]?turnTo[carId];
			if
			::turnTo[carId] == 1 ->
				//X2 to X3
				currentRoad[carId] = 3;
				currentLocation[carId] = 1;
				currentDirection[carId] = 1;
			::turnTo[carId] == 2 ->
				//X2 to X4
				currentRoad[carId] = 2;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[2];
			::turnTo[carId] == 3 ->
				//X2 to X1
				currentRoad[carId] = 0;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[0];
			::else
			fi
			//if-fi for turn to take
		::else
		fi
		//if-fi for currentLocation[carId] < > 


	::currentRoad[carId] == 2 ->
		if
		::currentLocation[carId] > lengthOfRoads[currentRoad[carId]] ->
			//At turn: X4
			takeTurnTo[0]?turnTo[carId];
			if
			::turnTo[carId] == 1 ->
				//X4 to X1
				currentRoad[carId] = 0;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[0];
			::turnTo[carId] == 2 ->
				//X4 to X2
				currentRoad[carId] = 1;
				currentDirection[carId] = 1;
				currentLocation[carId] = 1;
			::turnTo[carId] == 3 ->
				//X4 to X3
				currentRoad[carId] = 3;
				currentDirection[carId] = 1;
				currentLocation[carId] = 1;
			::else
			fi
			//if-fi for take turn to
		::currentLocation[carId] < 0
			//Take exit from Exit B
			isInMap[carId] = 0;
		::else
		fi
		//if-fi for currentLocation[carId] < >


	::currentRoad[carId] == 3 ->
		if
		::currentLocation[carId] > lengthOfRoads[currentRoad[carId]] ->
			//turn: Y1
			takeTurnTo[1]?turnTo[carId];
			if
			::turnTo[carId] == 2->
				//Y1 to Y2
				currentRoad[carId] = 5;
				currentDirection[carId] = 1;
				currentLocation[carId] = 1;
			::turnTo[carId] == 3->
				//Y1 to Y3
				currentRoad[carId] = 4;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[4];
			::else
			fi
			//if-fi for take turn to
		::currentLocation[carId] < 0 ->
			//Turn: X3
			takeTurnTo[0]?turnTo[carId];
			if
			::turnTo[carId] == 1->
				//X3 to X4
				currentRoad[carId] = 2;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[2];
			::turnTo[carId] == 2->
				//X3 to X1
				currentRoad[carId] = 0;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[0];
			::turnTo[carId] == 3->
				//X3 to X2
				currentRoad[carId] = 1;
				currentDirection[carId] = 1;
				currentLocation[carId] = 1;
			fi
			//If-fi for take turn to
		::else
		fi
		//if-fi for currentLocation[carId]


	::currentRoad[carId] == 4 ->
		if
		::currentLocation[carId] > lengthOfRoads[currentRoad[carId]] ->
			//Turn: Y3
			takeTurnTo[3]?turnTo[carId];
			if
			::turnTo[carId] == 1->
				//Y3 to Y1
				currentRoad[carId] = 3;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[3];
			::turnTo[carId] == 3->
				//Y3 to Y1
				currentRoad[carId] = 5;
				currentDirection[carId] = 1;
				currentLocation[carId] = 1;
			::else
			fi
			//if-fi for take turn to
		::currentLocation[carId] < 0 ->
			isInMap[carId] = 0;
			//Exit C
		::else
		fi
		//if-fi for currentLocation[carId]


	::currentRoad[carId] == 5 ->
		if
		::currentLocation[carId] > lengthOfRoads[currentRoad[carId]] ->
			//At turn: Z1
			takeTurnTo[3]?turnTo[carId];
			if
			::turnTo[carId] == 1->
				//Z1 to Z2
				currentRoad[carId] = 1;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[1];
			::turnTo[carId] == 3->
				//Z1 to Z3
				currentRoad[carId] = 6;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[6];
			::else
			fi
			//if-fi for take turn to
		::currentLocation[carId] < 0 ->
			//At turn Y2
			takeTurnTo[2]?turnTo[carId];
			if
			::turnTo[carId] == 1->
				//Y2 to Y3
				currentRoad[carId] = 4;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[4];
			::turnTo[carId] == 2->
				//Y2 to Y1
				currentRoad[carId] = 3;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[3];
			fi
			//If-fi for take turn to
		::else
		fi
		//if-fi for currentLocation[carId]


	::currentRoad[carId] == 6 ->
		if
		::currentLocation[carId] > lengthOfRoads[currentRoad[carId]] ->
		//at turn: z3
			takeTurnTo[2]?turnTo[carId];
			if
			::turnTo[carId] == 1->
				//Z3 to Z1
				currentRoad[carId] = 5;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[5];
			::turnTo[carId] == 2->
				//Z3 to Z2
				currentRoad[carId] = 1;
				currentDirection[carId] = 0;
				currentLocation[carId] = lengthOfRoads[1];
			::else
			fi
			//if-fi for take turn to
		::currentLocation[carId] < 0 ->
			isInMap[carId] = 0;
			//exit D
		::else
		fi
		//if-fi for currentLocation[carId]
	::else 
	fi
}

active [2] proctype car() {

	mtype entryPoint;
	
	//Decide current road, current speed, current location and direction for the car to start
	
start:
	randomEntryPoint ? entryPoint;
	currentLocation[_pid] = 1;
	currentDirection[_pid] = 1;
	randomSpeed?currentSpeed[_pid];
	isInMap[_pid] = 1;
	

	if
	::entryPoint == A -> currentRoad[_pid] = 0;
	::entryPoint == B -> currentRoad[_pid] = 2;
	::entryPoint == C -> currentRoad[_pid] = 4;
	::entryPoint == D -> currentRoad[_pid] = 6;
	fi
		currentRoad[0] = 0;
		currentRoad[1] = 1;

	//Simulate the driving of car....
drive:
	randomSpeed?currentSpeed[_pid];
	map(_pid);
	
	if
	::isInMap[_pid] == 0 -> goto start;
	::else
	fi

	printf("Car %d is riding on road: %d and is at location: %d and has taken the turn: %d", _pid, currentRoad[_pid], currentLocation[_pid], turnTo[_pid]);
	//Debug log
	//printf("Car %d entered through %c is driving at speed: %d and is at location: %d", _pid, entryPoint, currentSpeed[_pid] nextLocation[_pid]);
	goto drive;
}

active proctype getStartingPoint() {
	do
		::randomEntryPoint  ! C;
		::randomEntryPoint  ! B;
		::randomEntryPoint  ! A;
		::randomEntryPoint  ! D;
	od
}

active proctype getTurn1() {
	do
		::takeTurnTo[0] ! 1;
		::takeTurnTo[0] ! 2;
		::takeTurnTo[0] ! 3;
	od
}

active proctype getTurn2() {
	do
		::takeTurnTo[1] ! 2;
		::takeTurnTo[1] ! 3;
	od
}

active proctype getTurn3() {
	do
		::takeTurnTo[2] ! 2;
		::takeTurnTo[2] ! 1;
	od
}

active proctype getTurn4() {
	do
		::takeTurnTo[3] ! 1;
		::takeTurnTo[3] ! 3;
	od
}

active proctype getSpeed() {
	do
		::randomSpeed ! 25;
		//::randomSpeed ! 20;
		//::randomSpeed ! 15;
		//::randomSpeed ! 10;
		::randomSpeed ! 5; 
	od
}

	ltl  { <> (isInMap[0] == 0 ->  isInMap[0] == 1)}
