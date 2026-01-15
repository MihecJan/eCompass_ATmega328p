#ifndef ECOMPASS_SERVICE_H_
#define ECOMPASS_SERVICE_H_


#include <stdint.h>

#define MAG_HARD_CALIB_TIME_MS		10000
#define MAG_HARD_CALIB_PERIOD_MS	20	// ~50 Hz

#define MAG_EEPROM_MAGIC 0x4D47

#define DECL_STEP 50

typedef enum {
	MAG_OFFSET_PROFILE_AUTO = 0,
	MAG_OFFSET_PROFILE_MANUAL,
	MAG_OFFSET_PROFILE_COUNT
} mag_offset_profile_t;

typedef struct 
{
	int16_t offset[3];	
} mag_offset_t;

typedef struct {
	uint16_t magic;
	mag_offset_t profiles[MAG_OFFSET_PROFILE_COUNT];
	int16_t declination100;
} mag_eeprom_t;

typedef void (*mag_status_cb_t)(const char *msg);

int16_t declination100;

void mag_calib_register_status_cb(mag_status_cb_t cb);

void eCompass_params_init();

void increment_declination();
void decrement_declination();

uint8_t eCompass_mag_azimuth(int32_t *psi_out);
uint8_t eCompass_true_from_mag(int32_t psi, int32_t *true_out);

void mag_hardiron_calibration_run(void);

void mag_set_offset(mag_offset_profile_t profile, int16_t x, int16_t y, int16_t z);
void mag_set_offsetX(mag_offset_profile_t profile, int16_t x);
void mag_set_offsetY(mag_offset_profile_t profile, int16_t y);
void mag_set_offsetZ(mag_offset_profile_t profile, int16_t z);

void mag_get_offset(mag_offset_profile_t profile, int16_t out[3]);
void mag_get_offsetX(mag_offset_profile_t profile, int16_t *outX);
void mag_get_offsetY(mag_offset_profile_t profile, int16_t *outY);
void mag_get_offsetZ(mag_offset_profile_t profile, int16_t *outZ);

uint8_t mag_apply_offset(mag_offset_profile_t profile);

int16_t LSB_to_micro_tesla(int16_t LSB);

int16_t get_approx_total_field();


#endif /* ECOMPASS_SERVICE_H_ */