//LTL properties:


//* Cars will not crash
ltl {[] !(currentRoad[0] == currentRoad[1] && currentDirection[0] == currentDirection[1] && currentLocation[0] == currentLocation[1])}

//* Globally speed must be lower than or equal to 25
ltl speed {[] (currentSpeed[1] <= 25 && currentSpeed[0] <= 25)}

//* Cars will eventually leave the map
ltl leaveMap {<> (isInMap[0] == 0) && <> (isInMap[1] == 0)}

//* Cars should be on defined roads
ltl onDefinedRoads {[] (currentRoad[0) <7 && currentRoad(1) < 7 )}