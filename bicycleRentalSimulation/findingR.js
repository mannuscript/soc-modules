var r = 3;
var ts = [2.353, 2.132, 2.015, 1.943, 1.895, 1.860, 1.833, 1.812, 1.796, 1.782, 1.771, 1.761, 1.753, 1.746, 1.740, 1.734, 1.729, 1.725, 1.721, 1.717, 1.714, 1.711, 1.708, 1.706, 1.703, 1.701, 1.699, 1.697];

for(var i = 0; i < ts.length; i++) {
	console.log(++r, ts[i] , 0.03854 * 0.03854 * ts[i]* ts[i] / (.04 * .04) );
}