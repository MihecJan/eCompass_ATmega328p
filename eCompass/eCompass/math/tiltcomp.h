#ifndef TILTCOMP_H_
#define TILTCOMP_H_


#include <stdint.h>

void iecompass(int16_t iBpx, int16_t iBpy, int16_t iBpz,
int16_t iGpx, int16_t iGpy, int16_t iGpz);
int16_t get_phi();
int16_t get_the();
int16_t get_psi();

#endif /* TILTCOMP_H_ */