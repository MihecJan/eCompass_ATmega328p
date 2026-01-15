#include <stdint.h>

#include "idiv.h"

int16_t iDivide(int16_t num, int16_t den)
{
	if (den == 0) return 32767;
	int32_t tmp = ((int32_t)num << 15) / den;
	if (tmp > 32767) return 32767;
	if (tmp < 0) return 0;
	return (int16_t)tmp;
}


//const uint16_t MINDELTADIV = 1; /* final step size for iDivide */
///* function to calculate ir = iy / ix with iy <= ix, and ix, iy both > 0 */
//int16_t iDivide(int16_t iy, int16_t ix)
//{
	//int16_t itmp; /* scratch */
	//int16_t ir; /* result = iy / ix range 0., 1. returned in range 0 to 32767 */
	//int16_t idelta; /* delta on candidate result dividing each stage by factor of 2 */
	///* set result r to zero and binary search step to 16384 = 0.5 */
	//ir = 0;
	//idelta = 16384; /* set as 2^14 = 0.5 */
	///* to reduce quantization effects, boost ix and iy to the maximum signed 16 bit value */
	//while ((ix < 16384) && (iy < 16384))
	//{
		//ix = (int16_t)(ix + ix);
		//iy = (int16_t)(iy + iy);
	//}
	///* loop over binary sub-division algorithm solving for ir*ix = iy */
	//do
	//{
		///* generate new candidate solution for ir and test if we are too high or too low */
		//itmp = (int16_t)(ir + idelta); /* itmp=ir+delta, the candidate solution */
		//itmp = (int16_t)((itmp * ix) >> 15);
		//if (itmp <= iy) ir += idelta;
		//idelta = (int16_t)(idelta >> 1); /* divide by 2 using right shift one bit */
	//} while (idelta >= MINDELTADIV); /* last loop is performed for idelta=MINDELTADIV */
	//return (ir);
//}