dtmc

//MAP constants
const int N;
const int north = 0;
const int east = 1;
const int south = 2;
const int west = 3;

//PROBABILITIES
const double q;
const double p = (1-2*q)/2;

//Initial states
const int initialXA = 1;
const int initialYA = floor(2*N/3);
const int initialXB = N-1;
const int initialYB = N;
const int initialDirA = east;
const int initialDirB = north;

//Gas station position
const int xGasStation = floor(2*N/3) - 1;
const int yGasStation = N - 3;


formula carAOnBorderFormula = (xA = 1 | xA = N | yA = 1 | yA = N);

//Crash formula
formula carCrash = (xA = xB & yA = yB);
formula crashOnCorner1 = ( carCrash & xA = 1 & yA = 1);
formula crashOnCorner2 = ( carCrash & xA = N & yA = 1);
formula crashOnCorner3 = ( carCrash & xA = N & yA = N);
formula crashOnCorner4 = ( carCrash & xA = 1 & yA = N);

//Reaching gas station
formula carAReachingGasStation = (xA = xGasStation & yA = yGasStation);
formula carBReachingGasStation = (xB = xGasStation & yB = yGasStation);


module carA


	xA : [0..N] init initialXA;
	yA : [0..N] init initialYA;
	dirA : [0..4] init initialDirA;


	//Initial states:
	//Considering special states for initial states because cars will never be in such state during the simulation
	[Move] xA = 1 & yA = floor(2*N/3) & dirA = east -> 
		p*4/3 : (xA' = min(xA + 1, N)) & (dirA' = east) +
		q*4/3 : (yA' = min(yA + 1, N)) & (dirA' = south) + 
		q*4/3 : (yA' = max(yA - 1, 1)) & (dirA' = north);

			
	[Move] xA = N-1 & yA = N & dirA = north -> 
		p*4/3 : (dirA' = north) & (yA' = max(yA - 1, 1)) +
		q*4/3 : (dirA' = west) & (xA' = max(xA - 1, 1)) + 
		q*4/3 : (dirA' = east) & (xA' = min(xA + 1, N));

	

	//Part 1: (at 1*1)
	[Move] xA = 1 & yA = 1 & dirA = north -> 
			2*p : (yA' = min(yA + 1, N)) & (dirA' = south) + 
			2*q : (xA' = min(xA + 1, N)) & (dirA' = east);

	[Move] xA = 1 & yA = 1 & dirA = west -> 
			2*p : (xA' = min(xA + 1, N)) & (dirA' = east) + 
			2*q : (yA' = min(yA + 1, N)) & (dirA' = south);


	//Part 2: (at N*1)
	[Move] xA = N & yA = 1 & dirA = east -> 
			2*p : (xA' = max(xA - 1, 1)) & (dirA' = west) + 
			2*q : (yA' = min(yA + 1, N)) & (dirA' = south); 

	[Move] xA = N & yA = 1 & dirA = north -> 
			2*p : (yA' = min(yA + 1, N)) & (dirA' = south) + 
			2*q : (xA' = max(xA - 1, 1)) & (dirA' = west); 





	//Part 3: (at N*N)
	[Move] xA = N & yA = N & dirA = south -> 
			2*p : (yA' = max(yA - 1, 1)) & (dirA' = north) + 
			2*q : (xA' = max(xA - 1, 1)) & (dirA' = west); 

	[Move] xA = N & yA = N & dirA = east -> 
			2*p : (xA' = max(xA - 1, 1)) & (dirA' = west) + 
			2*q : (yA' = max(yA - 1, 1)) & (dirA' = north);





	//Part 4: (at 1*N)
	[Move] xA = 1 & yA = N & dirA = south -> 
			2*p : (yA' = max(yA - 1, 1)) & (dirA' = north) + 
			2*q : (xA' = min(xA + 1, N)) & (dirA' = east);

	[Move] xA = 1 & yA = N & dirA = west -> 
			2*p : (xA' = min(xA + 1, N)) & (dirA' = east) + 
			2*q : (yA' = max(yA - 1, 1)) & (dirA' = north);


	
	
	//Part 5
	[Move] xA != 1 & xA != N & yA = 1 & dirA = north -> 
			p*4/3 : (yA' = min(yA + 1, N)) & (dirA' = south) + 
			q*4/3 : (xA' = min(xA + 1, N)) & (dirA' = east) + 
			q*4/3 : (xA' = max(xA - 1, 1)) & (dirA' = west);
	
	[Move] xA != 1 & xA != N & yA = 1 & (dirA = east | dirA = west) -> 
			p*4/3 : (xA' = min(xA + 1, N)) & (dirA' = east) + 
			p*4/3 : (xA' = max(xA - 1, 1)) & (dirA' = west) + 
			q*4/3 : (yA' = min(yA + 1, N)) & (dirA' = south);


	
	//Part 6
	[Move] xA = N & yA != 1 & yA != N & dirA = east -> 
			p*4/3 : (dirA' = west) & (xA' = max(xA - 1, 1)) + 
			q*4/3 : (dirA' = north) & (yA' = max(yA - 1, 1)) + 
			q*4/3 : (dirA'  = south) & (yA' = min(yA + 1, N));

	[Move] xA = N & yA != 1 & yA != N & (dirA = north | dirA = south) -> 
			p*4/3 : (dirA' = north) & (yA' = max(yA - 1, 1)) + 
			p*4/3 : (dirA' = south) & (yA' = min(yA + 1, N)) + 
			q*4/3 : (dirA' = west) & (xA' = max(xA - 1, 1));





	//Part 7
	[Move] xA != 1 & xA != N & yA = N & dirA = south -> 
			p*4/3 : (dirA' = north) & (yA' = max(yA - 1, 1)) + 
			q*4/3 : (dirA' = east) & (xA' = min(xA + 1, N)) + 
			q*4/3 : (dirA' = west) & (xA' = max(xA - 1, 1));

	[Move] xA != 1 & xA != N & yA = N & (dirA = east | dirA = west) -> 
			p*4/3 : (dirA' = east) & (xA' = min(xA + 1, N)) + 
			p*4/3 : (dirA' = west) & (xA' = max(xA - 1, 1)) + 
			q*4/3 : (dirA' = north) & (yA' = max(yA - 1, 1)); 





	//Part 8
	[Move] xA = 1 & yA > 1 & yA < N & dirA = west -> 
			p*4/3 : (dirA' = east) & (xA' = min(xA + 1, N)) + 
			q*4/3 : (dirA' = north) & (yA' = max(yA - 1, 1)) + 
			q*4/3 : (dirA' = south) & (yA' = min(yA + 1, N));

	[Move] xA = 1 & yA > 1 & yA < N & (dirA = north | dirA = south) -> 
			p*4/3 : (dirA' = north) & (yA' = max(yA - 1, 1)) + 
			p*4/3 : (dirA' = south) & (yA' = min(yA + 1, N)) + 
			q*4/3 : (dirA' = east) & (xA' = min(xA + 1, N));





	//Part 9
	[Move] xA != 1 & xA != N & yA != 1 & yA != N & (dirA = north | dirA = south) -> 
			p : (dirA' = north) & (yA' = max(yA - 1, 1)) + 
			p : (dirA' = south) & (yA' = min(yA + 1, N)) + 
			q : (dirA' = east) & (xA' = min(xA + 1, N)) + 
			q : (dirA' = west) & (xA' = max(xA - 1, 1));


	[Move] xA != 1 & xA != N & yA != 1 & yA != N & (dirA = east | dirA = west) -> 
			p : (dirA' = east) & (xA' = min(xA + 1, N)) + 
			p : (dirA' = west) & (xA' = max(xA - 1, 1)) + 
			q : (dirA' = north) & (yA' = max(yA - 1, 1)) + 
			q : (dirA' = south) & (yA' = min(yA + 1, N));

endmodule

module carB=carA[
	xA = xB,
	yA = yB,
	dirA = dirB,
	initialXA = initialXB,
	initialYA = initialYB,
	initialDirA = initialDirB
] endmodule


label "AorBreachingGasStation" = carAReachingGasStation | carBReachingGasStation;
label "AandBreachingGasStation" = carAReachingGasStation & carAReachingGasStation;

//P=?[F<100 "carCrash"]
//P=?[F<30  "AorBreachingGasStation"]
//R=?[F carAReachingGasStation]
//P=?[F crashOnCorner1 & F crashOnCorner2 & F crashOnCorner3 & crashOnCorner4]
//P=?[GF AandBreachingGasStation]

rewards
	carAOnBorderFormula : 1;
endrewards