#include <stdint.h>

#include "atan.h"
#include "idiv.h"

/* calculates 100*atan2(iy/ix)=100*atan2(iy,ix) in deg for ix, iy in range -32768 to 32767 */
int16_t iHundredAtan2Deg(int16_t iy, int16_t ix)
{
	int16_t iResult; /* angle in degrees times 100 */
	/* check for -32768 which is not handled correctly */
	if (ix == -32768) ix = -32767;
	if (iy == -32768) iy = -32767;
	/* check for quadrants */
	if ((ix >= 0) && (iy >= 0)) /* range 0 to 90 degrees */
	iResult = iHundredAtanDeg(iy, ix);
	else if ((ix <= 0) && (iy >= 0)) /* range 90 to 180 degrees */
	iResult = (int16_t)(18000 - (int16_t)iHundredAtanDeg(iy, (int16_t)-ix));
	else if ((ix <= 0) && (iy <= 0)) /* range -180 to -90 degrees */
	iResult = (int16_t)((int16_t)-18000 + iHundredAtanDeg((int16_t)-iy, (int16_t)-ix));
	else /* ix >=0 and iy <= 0 giving range -90 to 0 degrees */
	iResult = (int16_t)(-iHundredAtanDeg((int16_t)-iy, ix));
	return (iResult);
}

/* fifth order of polynomial approximation giving 0.05 deg max error */
const int16_t K1 = 5701;
const int16_t K2 = -1645;
const int16_t K3 = 446;
/* calculates 100*atan(iy/ix) range 0 to 9000 for all ix, iy positive in range 0 to 32767 */
int16_t iHundredAtanDeg(int16_t iy, int16_t ix)
{
	int32_t iAngle; /* angle in degrees times 100 */
	int16_t iRatio; /* ratio of iy / ix or vice versa */
	int32_t iTmp; /* temporary variable */
	/* check for pathological cases */
	if ((ix == 0) && (iy == 0)) return (0);
	if ((ix == 0) && (iy != 0)) return (9000);
	/* check for non-pathological cases */
	if (iy <= ix)
	iRatio = iDivide(iy, ix); /* return a fraction in range 0. to 32767 = 0. to 1. */
	else
	iRatio = iDivide(ix, iy); /* return a fraction in range 0. to 32767 = 0. to 1. */
	/* first, third and fifth order polynomial approximation */
	iAngle = (int32_t) K1 * (int32_t) iRatio;
	iTmp = ((int32_t) iRatio >> 5) * ((int32_t) iRatio >> 5) * ((int32_t) iRatio >> 5);
	iAngle += (iTmp >> 15) * (int32_t) K2;
	iTmp = (iTmp >> 20) * ((int32_t) iRatio >> 5) * ((int32_t) iRatio >> 5);
	iAngle += (iTmp >> 15) * (int32_t) K3;
	iAngle = iAngle >> 15;
	/* check if above 45 degrees */
	if (iy > ix) iAngle = (int32_t)(9000 - iAngle);
	/* for tidiness, limit result to range 0 to 9000 equals 0.0 to 90.0 degrees */
	if (iAngle < 0) iAngle = 0;
	if (iAngle > 9000) iAngle = 9000;
	return ((int32_t) iAngle);
}