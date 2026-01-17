#include "config.h"

#include <util/delay.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <string.h>

#include "globals.h"
#include "eCompass_service.h"
#include "drivers/lsm303agr.h"
#include "utils/utils.h"
#include "math/mag_calibration.h"
#include "math/tiltcomp.h"
#include "math/util.h"
#include "LCD/lcd.h"

#include  "services/uart_service.h"

static mag_status_cb_t status_cb;

// eeprom instance
mag_eeprom_t EEMEM mag_eeprom;

// ram
static mag_offset_t mag_offsets[MAG_OFFSET_PROFILE_COUNT];

void mag_calib_register_status_cb(mag_status_cb_t cb)
{
	status_cb = cb;
}

void eCompass_params_init()
{
	mag_eeprom_t tmp;
	eeprom_read_block(&tmp, &mag_eeprom, sizeof(tmp));

	if (tmp.magic != MAG_EEPROM_MAGIC)
	{
		// EEPROM ni inicializiran - nastavi 0
		for (uint8_t i = 0; i < MAG_OFFSET_PROFILE_COUNT; i++) {
			for (uint8_t j = 0; j < 3; j++) {
				mag_offsets[i].offset[j] = 0;
			}
		}
		
		declination100 = 450;

		tmp.magic = MAG_EEPROM_MAGIC;
		memcpy(tmp.profiles, mag_offsets, sizeof(mag_offsets));
		eeprom_update_block(&tmp, &mag_eeprom, sizeof(tmp));
		tmp.declination100 = declination100;
	}
	else
	{
		// EEPROM OK - kopiraj v RAM
		memcpy(mag_offsets, tmp.profiles, sizeof(mag_offsets));
		declination100 = tmp.declination100 - (tmp.declination100 % DECL_STEP);
	}
}

void increment_declination()
{
	declination100 += DECL_STEP;
	
	eeprom_update_block(
		&declination100,
		&mag_eeprom.declination100,
		sizeof(declination100)
	);
}

void decrement_declination()
{
	declination100 -= DECL_STEP;
	
	eeprom_update_block(
		&declination100,
		&mag_eeprom.declination100,
		sizeof(declination100)
	);
}

uint8_t eCompass_mag_azimuth(int32_t *psi_out)
{
	if (!psi_out) return 1;
	
	int16_t acc[3];
	int16_t mag[3];
	
	LSM303AGR_read_acc_unshifted(acc);
	LSM303AGR_read_mag(mag);
	
	// NED: iecompass(magX, magY, magZ, accX, accY, accZ);
	iecompass(-mag[0], mag[1], -mag[2], acc[0], -acc[1], acc[2]);
	
	int32_t psi = get_psi();
	//snprintf(buff, sizeof(buff), "%ld\n\r", 36000 - ((psi + 36000) % 36000));
	//uart_send_string(buff);
	psi += 36000;
	psi %= 36000;
	
	*psi_out = 36000 - psi;
	
	return 0;
}

uint8_t eCompass_true_from_mag(int32_t psi, int32_t *true_out)
{
	*true_out = (psi - declination100 + 36000) % 36000;
	
	return 0;
}

void mag_hardiron_calibration_run(void)
{
	/* reset offset in sensor */
	uint8_t err = LSM303AGR_set_hard_iron_offset(0, 0, 0);
	if (err)
	{
		status_cb("Failed to reset offset\r\n");
		return;
	}
		
	status_cb("Premikaj tipalo v vse smeri za 10 sekund...\r\n");
		
	math_mag_calib_hard_init();
	
	uint32_t elapsed = 0;
	int16_t mag[3];
	int16_t mag_off[3];
	
	uint16_t i = 0;
	while (elapsed < MAG_HARD_CALIB_TIME_MS)
	{
		LSM303AGR_read_mag(mag);
		math_mag_calib_hard_update(mag);
		math_mag_calib_hard_compute_offset();
		
		math_mag_get_offset(mag_off);
		
		#ifdef DEBUG
			snprintf(buff, sizeof(buff), "X: %d, Y: %d, Z: %d\r\n", mag_off[0], mag_off[1], mag_off[2]);
			status_cb(buff);
		#endif
		
		if (i % 10 == 0)
		{
			LCD_draw_state_calibrating(mag_off);
		}

		_delay_ms(MAG_HARD_CALIB_PERIOD_MS);
		elapsed += MAG_HARD_CALIB_PERIOD_MS;
		
		i++;
	}

	math_mag_calib_hard_compute_offset();

	/* fetch calculated offset */
	math_mag_get_offset(mag_off);

	snprintf(buff, sizeof(buff),
	"Calculated offset: X: %d, Y: %d, Z: %d\r\n",
	mag_off[0], mag_off[1], mag_off[2]);
	status_cb(buff);

	mag_set_offset(MAG_OFFSET_PROFILE_AUTO, mag_off[0], mag_off[1], mag_off[2]);
	status_cb("Updated profile\n\r");

	mag_apply_offset(MAG_OFFSET_PROFILE_AUTO);
	status_cb("Using profile AUTO\n\r");
}

void mag_set_offset(mag_offset_profile_t profile, int16_t x, int16_t y, int16_t z)
{
	mag_offsets[profile].offset[0] = x;
	mag_offsets[profile].offset[1] = y;
	mag_offsets[profile].offset[2] = z;
	
	eeprom_update_block(
		&x,
		&mag_eeprom.profiles[profile].offset[0],
		sizeof(int16_t)
	);
	
	eeprom_update_block(
		&y,
		&mag_eeprom.profiles[profile].offset[1],
		sizeof(int16_t)
	);
	
	eeprom_update_block(
		&z,
		&mag_eeprom.profiles[profile].offset[2],
		sizeof(int16_t)
	);
}
void mag_set_offsetX(mag_offset_profile_t profile, int16_t x)
{
	mag_offsets[profile].offset[0] = x;
	
	eeprom_update_block(
		&x,
		&mag_eeprom.profiles[profile].offset[0],
		sizeof(int16_t)
	);
}

void mag_set_offsetY(mag_offset_profile_t profile, int16_t y)
{
	mag_offsets[profile].offset[1] = y;
	
	eeprom_update_block(
		&y,
		&mag_eeprom.profiles[profile].offset[1],
		sizeof(int16_t)
	);
}

void mag_set_offsetZ(mag_offset_profile_t profile, int16_t z)
{
	mag_offsets[profile].offset[2] = z;
	
	eeprom_update_block(
		&z,
		&mag_eeprom.profiles[profile].offset[2],
		sizeof(int16_t)
	);
}

void mag_get_offset(mag_offset_profile_t profile, int16_t out[3])
{
	out[0] = mag_offsets[profile].offset[0];
	out[1] = mag_offsets[profile].offset[1];
	out[2] = mag_offsets[profile].offset[2];
}

void mag_get_offsetX(mag_offset_profile_t profile, int16_t *outX)
{
	*outX = mag_offsets[profile].offset[0];
}

void mag_get_offsetY(mag_offset_profile_t profile, int16_t *outY)
{
	*outY = mag_offsets[profile].offset[1];
}

void mag_get_offsetZ(mag_offset_profile_t profile, int16_t *outZ)
{
	*outZ = mag_offsets[profile].offset[2];
}

uint8_t mag_apply_offset(mag_offset_profile_t profile)
{
	return LSM303AGR_set_hard_iron_offset(
		mag_offsets[profile].offset[0],
		mag_offsets[profile].offset[1],
		mag_offsets[profile].offset[2]
	);
}

int16_t LSB_to_micro_tesla(int16_t LSB)
{
	return ((int32_t)LSB * 15) / 100;
}


int16_t get_approx_total_field()
{
	int16_t mag[3];
	LSM303AGR_read_mag(mag);
	
	return MATH_get_approx_total_field(mag);
}









uint8_t eCompass_angles(int16_t *phi_out, int16_t *the_out, int16_t *psi_out)
{
	if (!psi_out) return 1;
	
	int16_t acc[3];
	int16_t mag[3];
	
	LSM303AGR_read_acc_unshifted(acc);
	LSM303AGR_read_mag(mag);
	
	// NED: iecompass(magX, magY, magZ, accX, accY, accZ);
	iecompass(-mag[0], mag[1], -mag[2], acc[1], -acc[1], acc[2]);
	
	*phi_out = get_phi();
	*the_out = get_the();
	*psi_out = get_psi();
	
	return 0;
}