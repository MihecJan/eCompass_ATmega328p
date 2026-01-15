#include <stdint.h>

#include "itrig.h"

const uint16_t MINDELTATRIG = 1; /* final step size for iTrig */

/* function to calculate ir = ix / sqrt(ix*ix+iy*iy) using binary division */
int16_t iTrig(int16_t ix, int16_t iy)
{
	uint32_t itmp; /* scratch */
	uint32_t ixsq; /* ix * ix */
	int16_t isignx; /* storage for sign of x. algorithm assumes x >= 0 then corrects later */
	uint32_t ihypsq; /* (ix * ix) + (iy * iy) */
	int16_t ir; /* result = ix / sqrt(ix*ix+iy*iy) range -1, 1 returned as signed Int16 */
	int16_t idelta; /* delta on candidate result dividing each stage by factor of 2 */
	/* stack variables */
	/* ix, iy: signed 16 bit integers representing sensor reading in range -32768 to 32767 */
	/* function returns signed Int16 as signed fraction (ie +32767=0.99997, -32768=-1.0000) */
	/* algorithm solves for ir*ir*(ix*ix+iy*iy)=ix*ix */
	/* correct for pathological case: ix==iy==0 */
	if ((ix == 0) && (iy == 0)) ix = iy = 1;
	/* check for -32768 which is not handled correctly */
	if (ix == -32768) ix = -32767;
	if (iy == -32768) iy = -32767;
	/* store the sign for later use. algorithm assumes x is positive for convenience */
	isignx = 1;
	if (ix < 0)
	{
		ix = (int16_t)-ix;
		isignx = -1;
	}
	/* for convenience in the boosting set iy to be positive as well as ix */
	iy = (int16_t)iy;
	if (iy < 0) iy = -iy;
	/* to reduce quantization effects, boost ix and iy but keep below maximum signed 16 bit */
	while ((ix < 16384) && (iy < 16384))
	{
		ix = (int16_t)(ix + ix);
		iy = (int16_t)(iy + iy);
	}
	/* calculate ix*ix and the hypotenuse squared */
	ixsq = (uint32_t)ix * (uint32_t)ix; /* ixsq=ix*ix: 0 to 32767^2 = 1073676289 */
	ihypsq = ixsq + (uint32_t)iy * (uint32_t)iy; /* ihypsq=(ix*ix+iy*iy) 0 to 2*32767*32767=2147352578 */
	/* set result r to zero and binary search step to 16384 = 0.5 */
	ir = 0;
	idelta = 16384; /* set as 2^14 = 0.5 */
	/* loop over binary sub-division algorithm */
	do
	{
		/* generate new candidate solution for ir and test if we are too high or too low */
		/* itmp=(ir+delta)^2, range 0 to 32767*32767 = 2^30 = 1073676289 */
		int32_t t = (int32_t)(ir + idelta);
		itmp = (uint32_t)t * (uint32_t)t;
		/* itmp=(ir+delta)^2*(ix*ix+iy*iy), range 0 to 2^31 = 2147221516 */
		itmp = (itmp >> 15) * (ihypsq >> 15);
		if (itmp <= ixsq) ir += idelta;
		idelta = (int16_t)(idelta >> 1); /* divide by 2 using right shift one bit */
	} while (idelta >= MINDELTATRIG); /* last loop is performed for idelta=MINDELTATRIG */
		
	/* correct the sign before returning */
	return (int16_t)(ir * isignx);
}