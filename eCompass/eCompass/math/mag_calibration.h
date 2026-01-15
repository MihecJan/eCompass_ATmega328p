#ifndef MAG_CALIBRATION_H_
#define MAG_CALIBRATION_H_


#include <stdint.h>

void math_mag_get_offset(int16_t offset[3]);
void math_mag_calib_hard_init();
void math_mag_calib_hard_compute_offset();
void math_mag_calib_hard_update(const int16_t mag[3]);

#endif /* MAG_CALIBRATION_H_ */