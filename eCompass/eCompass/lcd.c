#include <u8g2.h>
#include <u8x8_avr.h>
#include <avr/power.h>
#include <util/delay.h>
#include <stdio.h>

#include "lcd.h"
#include "globals.h"
#include "services/uart_service.h"
#include "drivers/lsm303agr.h"
#include "math/sin.h"
#include "services/eCompass_service.h"

u8g2_t u8g2;

static void draw_state(const char *state);
static void draw_compass(int16_t deg);
static void draw_vcenter(const char *str, uint8_t y);

uint8_t u8x8_byte_uart(
u8x8_t *u8x8,
uint8_t msg,
uint8_t arg_int,
void *arg_ptr)
{
	switch (msg)
	{
		case U8X8_MSG_BYTE_START_TRANSFER:
		uart_send_byte(0xAA);
		break;
		
		case U8X8_MSG_BYTE_SEND:
		{
			uint8_t *data = (uint8_t *)arg_ptr;
			for (uint8_t i = 0; i < arg_int; i++)
			{
				uart_send_byte(data[i]);
			}
			break;
		}
		
		case U8X8_MSG_BYTE_END_TRANSFER:
		uart_send_byte(0x55);
		break;
	}
	
	return 1;
}

uint8_t u8x8_gpio_and_delay_dummy(
	u8x8_t *u8x8,
	uint8_t msg,
	uint8_t arg_int,
	void *arg_ptr)
{
	return 1;
}

uint8_t u8x8_gpio_and_delay (u8x8_t * u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	//Re-use library for delays
	if (u8x8_avr_delay(u8x8, msg, arg_int, arg_ptr))
	{
		return 1;
	}

	switch (msg) {
		//called once during init phase of u8g2/u8x8
		//can be used to setup pins
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			CS_DDR |= _BV(CS_BIT);
			DC_DDR |= _BV(DC_BIT);
			RESET_DDR |= _BV(RESET_BIT);

			RESET_PORT &= ~_BV(RESET_BIT);
			_delay_ms(10);
			RESET_PORT |= _BV(RESET_BIT);
			_delay_ms(10);
			break;
			
		case U8X8_MSG_GPIO_CS:
			CS_PORT &= ~_BV(CS_BIT); // always LOW
			break;
			
		//DC (data/cmd, A0, register select) pin: Output level in arg_int
		case U8X8_MSG_GPIO_DC:
			if (arg_int)
			{
				DC_PORT |= _BV(DC_BIT);
			}
			else
			{
				DC_PORT &= ~_BV(DC_BIT);
			}
			break;
		//Reset pin: Output level in arg_int
		case U8X8_MSG_GPIO_RESET:
			break;
			
		default:
			u8x8_SetGPIOResult(u8x8, 1);
			break;
	}
	
	return 1;
}

void init_graphics()
{
	u8g2_Setup_st7565_nhd_c12864_1(
		&u8g2,
		U8G2_R0,
		u8x8_byte_uart,
		u8x8_gpio_and_delay_dummy
	);
	
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
}

void init_graphic_LCD()
{	
	u8g2_Setup_st7565_erc12864_1(
		&u8g2,
		U8G2_R0,
		u8x8_byte_avr_hw_spi,
		u8x8_gpio_and_delay
	);
	
	u8g2_InitDisplay(&u8g2);
	u8g2_SetContrast(&u8g2, 16);
	u8g2_SetPowerSave(&u8g2, 0);
}

void lcd_draw_state_auto_calibration(int16_t true_azimuth_deg)
{
	u8g2_FirstPage(&u8g2);
	do
	{
		u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
		draw_state("AUTO CALIB");
		
		snprintf(buff, sizeof(buff), "TRUE: %d deg", true_azimuth_deg);
		u8g2_DrawStr(&u8g2, 0, 16, buff);
		
		u8g2_DrawStr(&u8g2, 0, 25, "offsets:");
		
		int16_t offX;
		mag_get_offsetX(MAG_OFFSET_PROFILE_AUTO, &offX);
		snprintf(buff, sizeof(buff), "X: %d uT", LSB_to_micro_tesla(offX));
		u8g2_DrawStr(&u8g2, 0, 33, buff);
		
		int16_t offY;
		mag_get_offsetY(MAG_OFFSET_PROFILE_AUTO, &offY);
		snprintf(buff, sizeof(buff), "Y: %d uT", LSB_to_micro_tesla(offY));
		u8g2_DrawStr(&u8g2, 0, 40, buff);
		
		int16_t offZ;
		mag_get_offsetZ(MAG_OFFSET_PROFILE_AUTO, &offZ);
		snprintf(buff, sizeof(buff), "Z: %d uT", LSB_to_micro_tesla(offZ));
		u8g2_DrawStr(&u8g2, 0, 47, buff);
		
		u8g2_DrawStr(&u8g2, 0, 56, "Approx field:");
		snprintf(buff, sizeof(buff), "%d uT", LSB_to_micro_tesla(get_approx_total_field()));
		u8g2_DrawStr(&u8g2, 0, 63, buff);
				
		draw_compass(true_azimuth_deg);
	}
	while (u8g2_NextPage(&u8g2));
}

void lcd_draw_state_calibrating(int16_t mag_off[3])
{
	u8g2_FirstPage(&u8g2);
	do
	{
		u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
		draw_state("CALIBRATING");
		
		draw_vcenter("Current offsets:", 20);
		
		snprintf(buff, sizeof(buff), "X: %d [%d uT]", mag_off[0], LSB_to_micro_tesla(mag_off[0]));
		draw_vcenter(buff, 35);
		snprintf(buff, sizeof(buff), "Y: %d [%d uT]", mag_off[1], LSB_to_micro_tesla(mag_off[1]));
		draw_vcenter(buff, 45);
		snprintf(buff, sizeof(buff), "Z: %d [%d uT]", mag_off[2], LSB_to_micro_tesla(mag_off[2]));
		draw_vcenter(buff, 55);
	}
	while (u8g2_NextPage(&u8g2));
}

void lcd_draw_state_manual_calibration(int16_t true_azimuth_deg)
{
	u8g2_FirstPage(&u8g2);
	do
	{
		u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
		draw_state("MANUAL CALIB");
		
		snprintf(buff, sizeof(buff), "TRUE: %d deg", true_azimuth_deg);
		u8g2_DrawStr(&u8g2, 0, 16, buff);
		
		u8g2_DrawStr(&u8g2, 0, 25, "offsets:");
		
		int16_t offX;
		mag_get_offsetX(MAG_OFFSET_PROFILE_MANUAL, &offX);
		snprintf(buff, sizeof(buff), "X: %d uT", LSB_to_micro_tesla(offX));
		u8g2_DrawStr(&u8g2, 0, 33, buff);
		
		int16_t offY;
		mag_get_offsetY(MAG_OFFSET_PROFILE_MANUAL, &offY);
		snprintf(buff, sizeof(buff), "Y: %d uT", LSB_to_micro_tesla(offY));
		u8g2_DrawStr(&u8g2, 0, 40, buff);
		
		int16_t offZ;
		mag_get_offsetZ(MAG_OFFSET_PROFILE_MANUAL, &offZ);
		snprintf(buff, sizeof(buff), "Z: %d uT", LSB_to_micro_tesla(offZ));
		u8g2_DrawStr(&u8g2, 0, 47, buff);
		
		u8g2_DrawStr(&u8g2, 0, 56, "Approx field:");
		snprintf(buff, sizeof(buff), "%d uT", LSB_to_micro_tesla(get_approx_total_field()));
		u8g2_DrawStr(&u8g2, 0, 63, buff);
		
		draw_compass(true_azimuth_deg);
	}
	while (u8g2_NextPage(&u8g2));
}

void lcd_draw_state_X_offset(int16_t manual_offsetX, int16_t manual_offsetY, int16_t manual_offsetZ)
{
	u8g2_FirstPage(&u8g2);
	do
	{
		u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
		draw_state("X OFFSET");
		
		snprintf(buff, sizeof(buff), "Y: %d [%ld nT]", manual_offsetY, LSM303AGR_mag_raw_to_nT(manual_offsetY));
		draw_vcenter(buff, 35);
		
		snprintf(buff, sizeof(buff), "Z: %d [%ld nT]", manual_offsetZ, LSM303AGR_mag_raw_to_nT(manual_offsetZ));
		draw_vcenter(buff, 45);
		
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tf);
		
		snprintf(buff, sizeof(buff), "X: %d [%ld nT]", manual_offsetX, LSM303AGR_mag_raw_to_nT(manual_offsetX));
		draw_vcenter(buff, 25);
	}
	while (u8g2_NextPage(&u8g2));
}

void lcd_draw_state_Y_offset(int16_t manual_offsetX, int16_t manual_offsetY, int16_t manual_offsetZ)
{
	u8g2_FirstPage(&u8g2);
	do
	{
		u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
		draw_state("Y OFFSET");
		
		snprintf(buff, sizeof(buff), "X: %d [%ld nT]", manual_offsetX, LSM303AGR_mag_raw_to_nT(manual_offsetX));
		draw_vcenter(buff, 22);
		
		snprintf(buff, sizeof(buff), "Z: %d [%ld nT]", manual_offsetZ, LSM303AGR_mag_raw_to_nT(manual_offsetZ));
		draw_vcenter(buff, 45);
		
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tf);
		
		snprintf(buff, sizeof(buff), "Y: %d [%ld nT]", manual_offsetY, LSM303AGR_mag_raw_to_nT(manual_offsetY));
		draw_vcenter(buff, 35);
	}
	while (u8g2_NextPage(&u8g2));
}

void lcd_draw_state_Z_offset(int16_t manual_offsetX, int16_t manual_offsetY, int16_t manual_offsetZ)
{
	u8g2_FirstPage(&u8g2);
	do
	{
		u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
		draw_state("Z OFFSET");
		
		snprintf(buff, sizeof(buff), "X: %d [%ld nT]", manual_offsetX, LSM303AGR_mag_raw_to_nT(manual_offsetX));
		draw_vcenter(buff, 22);
		
		snprintf(buff, sizeof(buff), "Y: %d [%ld nT]", manual_offsetY, LSM303AGR_mag_raw_to_nT(manual_offsetY));
		draw_vcenter(buff, 34);
		
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tf);
		
		snprintf(buff, sizeof(buff), "Z: %d [%ld nT]", manual_offsetZ, LSM303AGR_mag_raw_to_nT(manual_offsetZ));
		draw_vcenter(buff, 48);
	}
	while (u8g2_NextPage(&u8g2));
}

void lcd_draw_state_declination(int16_t declination)
{
	u8g2_FirstPage(&u8g2);
	do
	{
		u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
		draw_state("DECLINATION");
		
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tf);
		
		char *sign = "";
		int16_t d = declination / 100;
		int8_t f = declination % 100;
		if (f < 0) f = -f;
		if (declination < 0 && declination > -100) sign = "-";
		snprintf(buff, sizeof(buff), "%s%d.%02d deg", sign, d, f);
		draw_vcenter(buff, 35);
	}
	while (u8g2_NextPage(&u8g2));
}

static void draw_vcenter(const char *str, uint8_t y)
{	
	uint8_t sw = u8g2_GetDisplayWidth(&u8g2);

	uint8_t text_w = u8g2_GetStrWidth(&u8g2, str);

	uint8_t x = (sw - text_w) / 2;

	u8g2_DrawStr(&u8g2, x, y, str);
}


static void draw_state(const char *state)
{
	uint16_t x = 0;
	uint16_t y = 6;   // baseline, NOT top

	u8g2_DrawStr(&u8g2, x, y, state);
}

static void draw_compass(int16_t deg)
{	
	uint8_t cx = 95;
	uint8_t cy = 31;
	uint8_t radius = 31;
	uint8_t needle_len = 31;
	uint8_t needle_width = 5;
	
	u8g2_DrawCircle(&u8g2, cx, cy, radius, U8G2_DRAW_ALL);
	
	int16_t deg90 = deg + 90;
	deg90 %= 360;
	
	uint8_t needle_north_x = cx - (((int16_t)needle_len * (int16_t)math_sin1000(deg)) / 1000);
	uint8_t needle_north_y = cy + (((int16_t)needle_len * (int16_t)-math_cos1000(deg)) / 1000);
	
	uint8_t needle_south_x = cx + (((int16_t)needle_len * (int16_t)math_sin1000(deg)) / 1000);
	uint8_t needle_south_y = cy - (((int16_t)needle_len * (int16_t)-math_cos1000(deg)) / 1000);
		
	uint8_t needle_west_x = cx + (((int16_t)(needle_width) * (int16_t)math_sin1000(deg90)) / 1000);
	uint8_t needle_west_y = cy - (((int16_t)(needle_width) * (int16_t)-math_cos1000(deg90)) / 1000);
	
	uint8_t needle_east_x = cx - (((int16_t)(needle_width) * (int16_t)math_sin1000(deg90)) / 1000);
	uint8_t needle_east_y = cy + (((int16_t)(needle_width) * (int16_t)-math_cos1000(deg90)) / 1000);
	
	u8g2_DrawTriangle(&u8g2, needle_north_x, needle_north_y, needle_west_x, needle_west_y, needle_east_x, needle_east_y);
	u8g2_DrawLine(&u8g2, needle_east_x, needle_east_y, needle_south_x, needle_south_y);
	u8g2_DrawLine(&u8g2, needle_south_x, needle_south_y, needle_west_x, needle_west_y);
}
















void draw_test(void)
{
	u8g2_FirstPage(&u8g2);
	do
	{
		u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
		u8g2_SetFontDirection(&u8g2, 0);
		u8g2_DrawStr(&u8g2, 19, 12, "128 px");
		u8g2_SetFontDirection(&u8g2, 1);
		u8g2_DrawStr(&u8g2, 5, 18, "64 px");
		
		u8g2_DrawHLine(&u8g2, 2, 2, 80);
		u8g2_DrawLine(&u8g2, 82, 2, 78, 0);
		u8g2_DrawLine(&u8g2, 82, 2, 78, 4);
		
		u8g2_DrawVLine(&u8g2, 2, 2, 60);
		u8g2_DrawLine(&u8g2, 2, 62, 0, 58);
		u8g2_DrawLine(&u8g2, 2, 62, 4, 58);


		//u8g2_DrawFrame(&u8g2, 0, 16, 128, 48);
	}
	while (u8g2_NextPage(&u8g2));
}