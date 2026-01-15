#include <stdint.h>

#include "tiltcomp.h"
#include "itrig.h"
#include "atan.h"

/* roll pitch and yaw angles computed by iecompass */
static int16_t iPhi, iThe, iPsi;

/* magnetic field readings corrected for hard iron effects and PCB orientation */
static int16_t iBfx, iBfy, iBfz;

/* hard iron estimate */
static int16_t iVx = 0;
static int16_t iVy = 0;
static int16_t iVz = 0;

/* tilt-compensated e-Compass code */
void iecompass(int16_t iBpx, int16_t iBpy, int16_t iBpz,
int16_t iGpx, int16_t iGpy, int16_t iGpz)
{	
	/* stack variables */
	/* iBpx, iBpy, iBpz: the three components of the magnetometer sensor */
	/* iGpx, iGpy, iGpz: the three components of the accelerometer sensor */
	
	/* local variables */
	int16_t iSin, iCos; /* sine and cosine */
	
	/* subtract the hard iron offset */
	iBpx -= iVx; /* see Eq 16 */
	iBpy -= iVy; /* see Eq 16 */
	iBpz -= iVz; /* see Eq 16 */
	
	/* calculate current roll angle Phi */
	iPhi = iHundredAtan2Deg(iGpy, iGpz);/* Eq 13 */
	
	/* calculate sin and cosine of roll angle Phi */
	iSin = iTrig(iGpy, iGpz); /* Eq 13: sin = opposite / hypotenuse */
	iCos = iTrig(iGpz, iGpy); /* Eq 13: cos = adjacent / hypotenuse */
	
	/* de-rotate by roll angle Phi */
	iBfy = (int16_t)(((int32_t)iBpy * iCos - (int32_t)iBpz * iSin) >> 15);/* Eq 19 y component */
	iBpz = (int16_t)(((int32_t)iBpy * iSin + (int32_t)iBpz * iCos) >> 15);/* Bpy*sin(Phi)+Bpz*cos(Phi)*/
	iGpz = (int16_t)(((int32_t)iGpy * iSin + (int32_t)iGpz * iCos) >> 15);/* Eq 15 denominator */
	
	/* calculate current pitch angle Theta */
	iThe = iHundredAtan2Deg((int16_t)-iGpx, iGpz);/* Eq 15 */
	
	/* restrict pitch angle to range -90 to 90 degrees */
	if (iThe > 9000) iThe = (int16_t) (18000 - iThe);
	if (iThe < -9000) iThe = (int16_t) (-18000 - iThe);
	
	/* calculate sin and cosine of pitch angle Theta */
	iSin = (int16_t)-iTrig(iGpx, iGpz); /* Eq 15: sin = opposite / hypotenuse */
	iCos = iTrig(iGpz, iGpx); /* Eq 15: cos = adjacent / hypotenuse */
	
	/* correct cosine if pitch not in range -90 to 90 degrees */
	if (iCos < 0) iCos = (int16_t)-iCos;
	
	/* de-rotate by pitch angle Theta */
	iBfx = (int16_t)(((int32_t)iBpx * iCos + (int32_t)iBpz * iSin) >> 15); /* Eq 19: x component */
	iBfz = (int16_t)(((int32_t)-iBpx * iSin + (int32_t)iBpz * iCos) >> 15);/* Eq 19: z component */
	
	/* calculate current yaw = e-compass angle Psi */
	iPsi = iHundredAtan2Deg((int16_t)-iBfy, iBfx); /* Eq 22 */
}

int16_t get_phi()
{
	return iPhi;
}

int16_t get_the()
{
	return iThe;
}

int16_t get_psi()
{
	return iPsi;
}