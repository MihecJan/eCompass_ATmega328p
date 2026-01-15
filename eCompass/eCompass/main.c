#include "config.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "globals.h"
//#include "cli/cli.h"
#include "services/uart_service.h"
#include "services/eCompass_service.h"
#include "services/button_service.h"
#include "services/twi_service.h"
#include "state/state.h"
#include "state/state_machine.h"
#include "utils/led.h"
#include "utils/utils.h"
#include "drivers/lsm303agr.h"
#include "lcd.h"

#include "hal/spi/spi_hal.h"

#include <util/delay.h>

char buff[UART_SEND_BUFF_SIZE];

void init();
void app_process();
void uart_print_cb(const char *msg);
void mag_azimuth();

int main(void)
{
	button_hal_init();
	init();
				
    while (1)
    {
	    app_process();
	    
	    _delay_ms(100);
    }
}

void app_process()
{
	// Produces HW button events
	button_hal_poll();
			
	//cli_poll();
			
	button_event_t ev = state_machine_step();
	
	int32_t psi;
	int32_t true_azimuth100;
	int16_t true_azimuth_deg;
	int8_t true_azimuth_rem;
	int16_t manual_offset[3];
			
	switch(state_get())
	{
		case STATE_AUTO_CALIBRATION:
			uart_send_string("STATE_AUTO_CALIBRATION\r\n");
			
			eCompass_mag_azimuth(&psi);
			eCompass_true_from_mag(psi, &true_azimuth100);
			
			true_azimuth_deg = true_azimuth100 / 100;
			if (true_azimuth100 < 0) true_azimuth100 = -true_azimuth100;
			true_azimuth_rem = true_azimuth100 % 100;
			
			snprintf(buff, sizeof(buff), "True azimuth: %4d.%02d deg\r\n", true_azimuth_deg, true_azimuth_rem);
			uart_send_string(buff);
			
			lcd_draw_state_auto_calibration(true_azimuth_deg);
			break;
				
		case STATE_CALIBRATING:
			uart_send_string("STATE_CALIBRATING\r\n");
			
			mag_hardiron_calibration_run();
			break;
				
		case STATE_MANUAL_CALIBRATION:
			uart_send_string("STATE_MANUAL_OFFSET\r\n");
			
			eCompass_mag_azimuth(&psi);
			eCompass_true_from_mag(psi, &true_azimuth100);
			
			true_azimuth_deg = true_azimuth100 / 100;
			if (true_azimuth100 < 0) true_azimuth100 = -true_azimuth100;
			true_azimuth_rem = true_azimuth100 % 100;
			
			lcd_draw_state_manual_calibration(true_azimuth_deg);
			break;
				
		case STATE_OFFSET_X:
			uart_send_string("STATE_OFFSET_X\r\n");
			
			mag_get_offset(MAG_OFFSET_PROFILE_MANUAL, manual_offset);
			
			if (ev == BTN_EVENT_INC)
			{
				manual_offset[0] += 10;
			}
			else if (ev == BTN_EVENT_DEC)
			{
				manual_offset[0] -= 10;
			}
			
			mag_set_offsetX(MAG_OFFSET_PROFILE_MANUAL, manual_offset[0]);
			
			snprintf(buff, sizeof(buff), "X offset: %d\n\r", manual_offset[0]);
			uart_send_string(buff);
			
			lcd_draw_state_X_offset(manual_offset[0], manual_offset[1], manual_offset[2]);
			break;
				
		case STATE_OFFSET_Y:
			uart_send_string("STATE_OFFSET_Y\r\n");
			
			mag_get_offset(MAG_OFFSET_PROFILE_MANUAL, manual_offset);
			
			if (ev == BTN_EVENT_INC)
			{
				manual_offset[1] += 10;
			}
			else if (ev == BTN_EVENT_DEC)
			{
				manual_offset[1] -= 10;
			}
			
			mag_set_offsetY(MAG_OFFSET_PROFILE_MANUAL, manual_offset[1]);
			
			snprintf(buff, sizeof(buff), "Y offset: %d\n\r", manual_offset[1]);
			uart_send_string(buff);
			
			lcd_draw_state_Y_offset(manual_offset[0], manual_offset[1], manual_offset[2]);
			break;
				
		case STATE_OFFSET_Z:
			uart_send_string("STATE_OFFSET_Z\r\n");
			
			mag_get_offset(MAG_OFFSET_PROFILE_MANUAL, manual_offset);
						
			if (ev == BTN_EVENT_INC)
			{
				manual_offset[2] += 10;
			}
			else if (ev == BTN_EVENT_DEC)
			{
				manual_offset[2] -= 10;
			}
			
			mag_set_offsetZ(MAG_OFFSET_PROFILE_MANUAL, manual_offset[2]);
						
			snprintf(buff, sizeof(buff), "Z offset: %d\n\r", manual_offset[2]);
			uart_send_string(buff);
			
			lcd_draw_state_Z_offset(manual_offset[0], manual_offset[1], manual_offset[2]);
			break;
				
		case STATE_DECLINATION:
			uart_send_string("STATE_DECLINATION\r\n");
			// print decli
			
			// get decli
			if (ev == BTN_EVENT_INC)
			{
				increment_declination();
				uart_send_string("INC");
			}
			else if (ev == BTN_EVENT_DEC)
			{
				decrement_declination();
			}
			
			lcd_draw_state_declination(declination100);
			break;
			
		case STATE_MOTION_CAL:
			// Output in MotionCal format
			uart_send_string("Raw:");
						
			int16_t acc[3];
			int16_t mag[3];
			
			LSM303AGR_read_acc(acc);
			LSM303AGR_read_mag(mag);
			
			snprintf(buff, sizeof(buff), "%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", 0, 0, 0, 0, 0, 0, mag[0], mag[1], mag[2]);
			uart_send_string(buff);
			
			break;
	}
}

void init()
{
	char *start = "Program start\n\r";
	uint8_t err;
	
	uart_rx_init();
	
	uart_init(115200);

	sei();
	
	uart_send_string(start);
	
	button_hal_init();
	button_service_init();
	
	mag_calib_register_status_cb(uart_print_cb);
	
	led_init(LED_DBG);
	uart_send_string("Initialized debug LED\n\r");
	
	twi_init(F_CPU);
	uart_send_string("Initialized TWI\n\r");

	spi_init();
	uart_send_string("Initialized SPI\n\r");
	
	init_graphic_LCD();
	uart_send_string("Initialized graphic LCD\n\r");

	 _delay_ms(10);
	 
	 err = LSM303AGR_init(MAG_ODR_10_HZ);
	 if (err)
	 {
		 snprintf(buff, sizeof(buff), "LSM303AGR_init returned error: %d", err);
		 uart_send_string(buff);
		 return;
	 }
	 
	 _delay_ms(100);

	 uart_send_string("LSM303AGR initialized:\n\r");
	 
	 //uart_send_string("\tMagnetometer\n\r");
//
	 //uint8_t mag_odr_hz;
	 //err = LSM303AGR_get_odr_mag_hz(&mag_odr_hz);
	 //report_uint16(err, "\t", "ODR", (uint16_t)mag_odr_hz, "Hz");
	 //
	 //char *mag_mode;
	 //err = LSM303AGR_get_mode_mag(&mag_mode);
	 //report(err, "\t", "Mode", mag_mode, NULL);
	 //
	 //char *mag_pwr_mode;
	 //err = LSM303AGR_get_power_mode_mag(&mag_pwr_mode);
	 //report(err, "\t", "Power mode", mag_pwr_mode, NULL);
	 //
	 //uint8_t lp_filter;
	 //err = LSM303AGR_get_lowpass_filter_mag(&lp_filter);
	 //char *lp_filter_str = lp_filter == 0 ? "Disabled" : "Enabled";
	 //report(err, "\t", "Low-pass filter", lp_filter_str, NULL);
//
	 //uint8_t offset_canc;
	 //err = LSM303AGR_get_offset_canc_mag(&offset_canc);
	 //char *offset_canc_str = offset_canc == 0 ? "Disabled" : "Enabled";
	 //report(err, "\t", "Offset cancellation", offset_canc_str, NULL);
	 //
	 //uart_send_string("\n\r");
	 //uart_send_string("\tAccelerometer\n\r");
//
	 //uint16_t acc_odr_hz;
	 //err = LSM303AGR_get_odr_acc_hz(&acc_odr_hz);
	 //report_uint16(err, "\t", "ODR", acc_odr_hz, "Hz");
	 //
	 //char *acc_pwr_mode;
	 //err = LSM303AGR_get_power_mode_acc(&acc_pwr_mode);
	 //report(err, "\t", "Power mode", acc_pwr_mode, NULL);
	 //
	 //uint8_t acc_range_hz;
	 //err = LSM303AGR_get_range_acc(&acc_range_hz);
	 //report_uint16(err, "\t", "Measurement range +-", (uint16_t)acc_range_hz, "g");
	 
	 eCompass_params_init();
	 
	 //cli_init();
}

void uart_print_cb(const char *msg)
{
	uart_send_string(msg);
}

void mag_azimuth()
{
	int32_t psi;
	int8_t mag_azimuth_deg;
	int8_t mag_azimuth_rem;
	eCompass_mag_azimuth(&psi);
	
	mag_azimuth_deg = psi / 100;
	if (psi < 0) psi = -psi;
	mag_azimuth_rem = psi % 100;
	
	snprintf(buff, sizeof(buff), "Magnetic azimuth: %4d.%02d deg\r\n", mag_azimuth_deg, mag_azimuth_rem);
	uart_send_string(buff);
}