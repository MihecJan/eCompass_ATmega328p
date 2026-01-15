#ifndef LSM303AGR_H_
#define LSM303AGR_H_


#include <stdint.h>

#define LSM303AGR_ACC_ADDR		0x19 // SA0 low
#define LSM303AGR_MAG_ADDR		0x1E // SA1/SD0 depends on board

#define CTRL_REG1_A				0x20
#define CTRL_REG4_A				0x23
#define OUT_X_L_A				0x28

#define XEN						0
#define YEN						1
#define ZEN						2
#define LPEN					3
#define ODR0					4
#define ODR1					5
#define ODR2					6
#define ODR3					7

#define SPI_ENABLE				0
#define ST0						1
#define ST1						2
#define HR						3
#define FS0						4
#define FS1						5
#define BLE_A					6
#define BDU_A					7

#define OFFSET_X_L_REG_M		0x45
#define OFFSET_X_H_REG_M		0x46
#define OFFSET_Y_L_REG_M		0x47
#define OFFSET_Y_H_REG_M		0x48
#define OFFSET_Z_L_REG_M		0x49
#define OFFSET_Z_H_REG_M		0x4A

#define CFG_REG_A_M				0x60
#define CFG_REG_B_M				0x61
#define CFG_REG_C_M				0x62
#define OUTX_L_REG_M			0x68

#define MD0						0
#define MD1						1
#define ORD0					2
#define ORD1					3
#define LP						4
#define SOFT_RST				5
#define REBOOT					6
#define COMP_TEMP_EN			7

#define LPF						0
#define OFF_CANC				1
#define SET_FREQ				2
#define INT_ON_DATAOFF			3
#define OFF_CANC_ONE_SHOT		4

#define INT_MAG					0
#define SELF_TEST				1
#define BLE_M					3
#define BDU_M					4
#define I2C_DIS					5
#define INT_MAG_PIN				6

#define MAG_SAMPLE_COUNT 500

#define MAG_ODR_10_HZ			0
#define MAG_ODR_20_HZ			1
#define MAG_ODR_50_HZ			2
#define MAG_ODR_100_HZ			3

typedef struct {
	int16_t x[MAG_SAMPLE_COUNT];
	int16_t y[MAG_SAMPLE_COUNT];
	int16_t z[MAG_SAMPLE_COUNT];
	uint16_t count;
} MagSamples_t;

uint8_t LSM303AGR_init(uint8_t odr);

uint8_t LSM303AGR_get_odr_mag_hz(uint8_t *odr_hz);
uint8_t LSM303AGR_get_mode_mag(char **mode);
uint8_t LSM303AGR_get_power_mode_mag(char **pwr_mode);
uint8_t LSM303AGR_get_lowpass_filter_mag(uint8_t *lp_filter);
uint8_t LSM303AGR_get_offset_canc_mag(uint8_t *offset_canc);

uint8_t LSM303AGR_get_odr_acc_hz(uint16_t *odr_hz);
uint8_t LSM303AGR_get_power_mode_acc(char **pwr_mode);
uint8_t LSM303AGR_get_range_acc(uint8_t *measurement_range_g);

uint8_t LSM303AGR_read_acc(int16_t data[3]);
uint8_t LSM303AGR_read_acc_unshifted(int16_t data[3]);

uint8_t LSM303AGR_read_mag(int16_t data[3]);

uint8_t LSM303AGR_set_hard_iron_offset(int16_t xOffset, int16_t yOffset, int16_t zOffset);


int32_t lsm303agrAccRawToMg(int16_t raw);
int32_t lsm303agrMagRawToNT(int16_t raw);
int16_t lsm303agrMagNTToRaw(int32_t ntValue);
void lsm303agrCollectMagSamples(MagSamples_t *samples, uint16_t duration_ms, uint16_t sampleRate_hz);
void lsm303agrComputeHardIron(MagSamples_t *samples, int16_t *offset_x, int16_t *offset_y, int16_t *offset_z);

int32_t LSM303AGR_mag_raw_to_nT(int16_t raw);


#endif /* LSM303AGR_H_ */