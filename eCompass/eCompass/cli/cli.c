//#include "../config.h"
//
//#include <string.h>
//#include <avr/io.h>
//#include <util/delay.h>
//#include <stdio.h>
//
//#include "cli.h"
//#include "../services/uart_service.h"
//#include "../utils/led.h"
//#include "drivers//lsm303agr.h"
//#include "../globals.h"
//#include "../math/tiltcomp.h"
//#include "../math/itrig.h"
//#include "../math/atan.h"
//#include "../math/mag_calibration.h"
//#include "../utils/utils.h"
//#include "../services/mag_calibration_service.h"
//#include "../services/button_service.h"
//
//#define CLI_MAX_LINE 64
//
//static char line_buff[CLI_MAX_LINE];
//static uint8_t line_len = 0;
//
///* ---- Command handlers ---- */
//static void cmd_help(void);
//static void cmd_led_on(void);
//static void cmd_led_off(void);
//static void cmd_mag(void);
//static void cmd_acc(void);
//static void cmd_mag_azimuth(void);
//static void cmd_hard_calib(void);
//static void cmd_btn_mode(void);
//static void cmd_btn_inc(void);
//static void cmd_btn_dec(void);
//static void cmd_btn_enter(void);
//static void cmd_test_itrig(void);
//static void cmd_test_atan(void);
//static void cmd_test_flat(void);
//static void cmd_test_euler(void);
//
///* ---- Command table ---- */
//typedef struct {
	//const char *name;
	//void (*handler)(void);
//} cli_cmd_t;
//
//static const cli_cmd_t cmd_table[] = {
	//{ "help",		cmd_help		},
	//{ "led on",		cmd_led_on		},
	//{ "led off",	cmd_led_off		},
	//{ "mag",		cmd_mag			},
	//{ "acc",		cmd_acc			},
	//{ "mag az",		cmd_mag_azimuth	},
	//{ "hard calib", cmd_hard_calib	},
	//{ "btn mode",	cmd_btn_mode	},
	//{ "btn inc",	cmd_btn_inc		},
	//{ "btn dec",	cmd_btn_dec		},
	//{ "btn enter",	cmd_btn_enter	},
	//{ "test itrig",	cmd_test_itrig	},
	//{ "test atan",	cmd_test_atan	},
	//{ "test flat",	cmd_test_flat	},
	//{ "test euler",	cmd_test_euler	}
//};
//
//#define CMD_COUNT (sizeof(cmd_table) / sizeof(cmd_table[0]))
//
//static void cli_execute(const char *line)
//{	
	//for (uint8_t i = 0; i < CMD_COUNT; i++)
	//{
		//if (strcmp(line, cmd_table[i].name) == 0)
		//{
			//cmd_table[i].handler();
			//return;
		//}
	//}
//
	///* Unknown command */
	//uart_send_string("ERR: unknown command\n\r");
//}
//
//void cli_init(void)
//{
	//line_len = 0;
//}
//
//void cli_poll(void)
//{		
	//uint8_t c;
//
	//while (uart_rx_pop(&c))
	//{
		//// Ignore CR
		//if (c == '\r')
		//{
			//continue;
		//}
			//
		//if (c == '\n')
		//{
			//line_buff[line_len] = '\0';
			//cli_execute(line_buff);
			//line_len = 0;
			//continue;
		//}
//
		//if (line_len < (CLI_MAX_LINE - 1))
		//{
			//line_buff[line_len++] = (char)c;
		//}
		//else
		//{
			///* Line too long ? reset */
			//line_len = 0;
		//}
	//}
//}
//
//static void cmd_help(void)
//{
	//uart_send_string("help is working\r\n");
//}
//
//static void cmd_led_on(void)
//{
	//led_on(LED_DBG);
//}
//
//static void cmd_led_off(void)
//{
	//led_off(LED_DBG);
//}
//
//static void cmd_mag(void)
//{
	//int16_t data[3];
	//LSM303AGR_read_mag(data);
	//
	//snprintf(buff, sizeof(buff), "%d %d %d\r\n", -data[1], data[0], -data[2]);
	//uart_send_string(buff);
//}
//
//static void cmd_acc(void)
//{
	//int16_t data[3];
	//LSM303AGR_read_acc_unshifted(data);
	//
	//snprintf(buff, sizeof(buff), "%d %d %d\r\n", data[1], -data[0], data[2]);
	//uart_send_string(buff);
//}
//
//static void cmd_mag_azimuth(void)
//{
	//int16_t acc[3];
	//int16_t mag[3];
	//int16_t psi;
	//int8_t mag_azimuth_deg;
	//int8_t mag_azimuth_rem;
	//
	//LSM303AGR_read_acc_unshifted(acc);
	//LSM303AGR_read_mag(mag);
			//
	//// NED: iecompass(magX, magY, magZ, accX, accY, accZ);
	//iecompass(-mag[0], mag[1], -mag[2], acc[1], -acc[1], acc[2]);
			//
	//psi = get_psi();
			//
	//mag_azimuth_deg = psi / 100;
	//if (psi < 0) psi = -psi;
	//mag_azimuth_rem = psi % 100;
			//
	//snprintf(buff, sizeof(buff), "Magnetic azimuth: %4d.%02d deg\r\n", mag_azimuth_deg, mag_azimuth_rem);
	//uart_send_string(buff);
//}
//
//static void cmd_hard_calib(void)
//{
	//mag_hardiron_calibration_run();
//}
//
//static void cmd_btn_mode(void)
//{
	//button_service_post(BTN_EVENT_MODE);
//}
//
//static void cmd_btn_inc(void)
//{
	//button_service_post(BTN_EVENT_INC);
//}
//
//static void cmd_btn_dec(void)
//{
	//button_service_post(BTN_EVENT_DEC);
//}
//
//static void cmd_btn_enter(void)
//{
	//button_service_post(BTN_EVENT_ENTER);
//}
//
//static void cmd_test_itrig(void)
//{
	//uart_send_string("testing iTrig\n\r");
	//int16_t res;
	//
	//res = iTrig(0, 1000);
	//snprintf(buff, sizeof(buff), "0 deg | expected 0, got: %d\r\n", res);
	//uart_send_string(buff);
	//
	//res = iTrig(707, 707);
	//snprintf(buff, sizeof(buff), "45 deg | expected ~23170, got: %d\r\n", res);
	//uart_send_string(buff);
//
	//res = iTrig(1000, 0);
	//snprintf(buff, sizeof(buff), "90 deg | expected 32767, got: %d\r\n", res);
	//uart_send_string(buff);
//
	//res = iTrig(707, -707);
	//snprintf(buff, sizeof(buff), "135 deg | expected ~-23170, got: %d\r\n", res);
	//uart_send_string(buff);
	//
	//res = iTrig(0, -1000);
	//snprintf(buff, sizeof(buff), "180 deg | expected ~0, got: %d\r\n", res);
	//uart_send_string(buff);
	//
	//res = iTrig(-707, -707);
	//snprintf(buff, sizeof(buff), "225 deg | expected ~-23170, got: %d\r\n", res);
	//uart_send_string(buff);
	//
	//res = iTrig(-1000, 0);
	//snprintf(buff, sizeof(buff), "270 deg | expected -32767, got: %d\r\n", res);
	//uart_send_string(buff);
	//
	//res = iTrig(-707, 707);
	//snprintf(buff, sizeof(buff), "315 deg | expected ~-23170, got: %d\r\n", res);
	//uart_send_string(buff);
//}
//
//static void cmd_test_atan(void)
//{
	//uart_send_string("testing iHundredAtan2Deg\n\r");
	//int16_t res;
	//
	//res = iHundredAtan2Deg(0, 1000);
	//snprintf(buff, sizeof(buff), "+X |expected 0, got: %d\r\n", res);
	//uart_send_string(buff);
//
	//res = iHundredAtan2Deg(1000, 0);
	//snprintf(buff, sizeof(buff), "+Y | expected 9000, got: %d\r\n", res);
	//uart_send_string(buff);
//
	//res = iHundredAtan2Deg(0, -1000);
	//snprintf(buff, sizeof(buff), "-X | expected +-18000, got: %d\r\n", res);
	//uart_send_string(buff);
//
	//res = iHundredAtan2Deg(-1000, 0);
	//snprintf(buff, sizeof(buff), "-Y | expected -9000, got: %d\r\n", res);
	//uart_send_string(buff);
	//
	//// Q1 (45°)
	//res = iHundredAtan2Deg(707, 707);
	//snprintf(buff, sizeof(buff), "Q1 | expected 4500, got: %d\r\n", res);
	//uart_send_string(buff);
//
	//// Q2 (135°)
	//res = iHundredAtan2Deg(707, -707);
	//snprintf(buff, sizeof(buff), "Q2 | expected 13500, got: %d\r\n", res);
	//uart_send_string(buff);
//
	//// Q3 (-135°)
	//res = iHundredAtan2Deg(-707, -707);
	//snprintf(buff, sizeof(buff), "Q3 | expected -13500, got: %d\r\n", res);
	//uart_send_string(buff);
//
	//// Q4 (-45°)
	//res = iHundredAtan2Deg(-707, 707);
	//snprintf(buff, sizeof(buff), "Q4 | expected -4500, got: %d\r\n", res);
	//uart_send_string(buff);
	//
	//// Small values (noise robustness)
	//res = iHundredAtan2Deg(1, 1);
	//snprintf(buff, sizeof(buff), "small | expected ~4500, got: %d\r\n", res);
	//uart_send_string(buff);
//}
//
//static void cmd_test_flat(void)
//{
	//uart_send_string("testing flat (no tilt compensation)\n\r");
	//
	//int16_t acc[3];
	//int16_t mag[3];
	//int16_t azimuth;
	//int8_t degrees;
	//int8_t decimal;
//
	//LSM303AGR_read_acc_unshifted(acc);
	//LSM303AGR_read_mag(mag);
	//
	//snprintf(buff, sizeof(buff),
		//"ACC raw: X=%6d Y=%6d Z=%6d\r\n",
		//acc[0], acc[1], acc[2]);
		//uart_send_string(buff);
//
	//snprintf(buff, sizeof(buff),
		//"MAG raw: X=%6d Y=%6d Z=%6d\r\n",
		//mag[0], mag[1], mag[2]);
		//uart_send_string(buff);
	//
	//azimuth = iHundredAtan2Deg(mag[1], mag[0]);
	//degrees = azimuth / 100;
	//if (azimuth < 0) azimuth = -azimuth;
	//decimal = azimuth % 100;
	//snprintf(buff, sizeof(buff), "flat azimuth: %4d.%02d deg\r\n", degrees, decimal);
	//uart_send_string(buff);
	//
	//azimuth = iHundredAtan2Deg(-mag[0], mag[1]);
	//degrees = azimuth / 100;
	//if (azimuth < 0) azimuth = -azimuth;
	//decimal = azimuth % 100;
	//snprintf(buff, sizeof(buff), "flat azimuth (NED): %4d.%02d deg\r\n", degrees, decimal);
	//uart_send_string(buff);
//}
//
//static void cmd_test_euler(void)
//{
	//int16_t acc[3];
	//int16_t mag[3];
//
	//LSM303AGR_read_acc_unshifted(acc);
	//LSM303AGR_read_mag(mag);
	//
	//// NED: iecompass(magX, magY, magZ, accX, accY, accZ);
	//iecompass(
		//-mag[1], mag[0], -mag[2],
		//acc[1], -acc[0], acc[2]
	//);
	//
	//int16_t phi = get_phi();
	//int16_t the = get_the();
	//int16_t psi = get_psi();
	//
	//snprintf(buff, sizeof(buff), "phi: %d the: %d psi: %d\r\n", phi, the, psi);
	//uart_send_string(buff);
//}
//
//void test()
//{
	//cmd_test_euler();
//}