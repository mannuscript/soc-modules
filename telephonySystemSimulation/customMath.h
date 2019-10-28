/* define the constants */
#define MODLUS 2147483647
#define MULT1 	24112
#define MULT2	26143

/* the following three declarations are for use of the random number
   generator rand and the associated functions randst and randgt for
   seed management. This file (rand.h) should be included in any program
   using these functions */
float randno(int stream);
void randst(long zset, int stream);
long randgt(int stream);


struct list{
	float *channels;
	int length;	
	int ptr;
  int nodeNumber;
  float utilization;
};

/*
	Don't waste your time reading code of this function
*/
int findTheSmallestAmongFour(float a, float b, float c, float d)
{
	//printf("\n\n\nDebug:\nfindTheSmallestAmongFour:\n%f\n%f\n%f\n%f\n\n\n",a,b,c,d);
	if(a<=b&&a<=c&&a<=d)return 1;else if(b<=a&&b<=c&&b<=d)return 2;else if(c<=a&&c<=b&&c<=d)return 3;else if(d<=a&&d<=b&&d<=c)return 4;
}

int findTheSmallestAmongThree(float a, float b, float c)
{
  //printf("\n\n\nDebug:\nfindTheSmallestAmongFour:\n%f\n%f\n%f\n%f\n\n\n",a,b,c,d);
  if(a<=b&&a<=c)return 1;else if(b<=a&&b<=c)return 2;else if(c<=a&&c<=b)return 3;
}


/* set the default seeds for all 100 streams */

static long zrng[] =
{ 	0,
 1973272912, 281629770, 20006270, 1280689831, 2096730329, 1933576050,
  913566091, 246780520, 1363774876, 604901985, 1511192140, 1259851944,
  824064364, 150493284, 242708531, 75253171, 1964472944, 1202299975,
  233217322, 1911216000, 726370533, 403498145, 993232223, 1103205531,
  762430696, 1922803170, 1385516923,  76271663, 413692397, 726466604,
  336157058, 1432650381, 1120463904, 595778810, 877722890, 1046574445,
  68911991, 2088367019, 748545416, 622401386, 2122378830, 640690903,
 1774806513, 2132545692, 2079249579, 78130110, 852776735, 1187867272,
  1351423507, 1645973084, 1997049139, 922510944, 2045512870, 898585771,
    243649545, 1004818771, 773686062, 403188473, 372279877, 1901633463,
   498067494, 2087759558, 493157915, 597104727, 1530940798, 1814496276,
   536444882, 1663153658, 855503735, 67784357, 1432404475, 619691088,
  119025595, 880802310, 176192644, 1116780070, 277854671, 1366580350,
  1142483975, 2026948561, 1053920743, 786262391, 1792203830, 1494667770,
1923011392, 1433700034, 1244184613, 1147297105, 539712780, 1545929719,
 190641742, 1645390429, 264907697, 620389253, 1502074852, 927711160,
  364849192, 2049576050, 638580085, 547070247};


/* generate the next random number */
float randno(int stream)
{
   long zi, lowprd, hi31;

   zi	= zrng[stream];
   lowprd = (zi & 65535) * MULT1;
   hi31	= (zi >> 16) * MULT1 + (lowprd >> 16);
   zi	= ((lowprd & 65535) - MODLUS) + ((hi31 &32767) << 16) + (hi31 >> 15);
   if (zi < 0) zi += MODLUS;
   lowprd = (zi & 65535) * MULT2;
   hi31	= (zi >> 16) * MULT2 + (lowprd >> 16);
   zi 	= ((lowprd & 65535) - MODLUS) + ((hi31 & 32767) << 16) + (hi31 >> 15);
   if (zi < 0) zi += MODLUS;
   zrng[stream] = zi;
   return ((zi >> 7 | 1) + 1)/ 16777216.0;
}

/* set the current zrng for stream "stream" to zset */
void randst (long zset, int stream)
{
  zrng[stream] = zset;
}

/* return the current zrng for stream "stream" */
long randgt(int stream)
{
   return zrng[stream];
}



float expon(float mean) /* exponential variate generation function */
{
  float u;

  /* generate a U(0,1) random veriate */
  u = randno(1);

  /* return an exponential random variate with mean "mean" */
  u = -mean*log(u);
  return u;
}