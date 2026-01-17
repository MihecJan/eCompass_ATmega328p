#ifndef LCD_H_
#define LCD_H_


#define CS_DDR DDRB
#define CS_PORT PORTB
#define CS_BIT 2

#define DC_DDR DDRB
#define DC_PORT PORTB
#define DC_BIT 1

#define RESET_DDR DDRB
#define RESET_PORT PORTB
#define RESET_BIT 0

void LCD_init();

void LCD_draw_state_auto_calibration(int16_t true_azimuth_deg);
void LCD_draw_state_calibrating();
void LCD_draw_state_manual_calibration(int16_t true_azimuth_deg);
void LCD_draw_state_X_offset(int16_t manual_offsetX, int16_t manual_offsetY, int16_t manual_offsetZ);
void LCD_draw_state_Y_offset(int16_t manual_offsetX, int16_t manual_offsetY, int16_t manual_offsetZ);
void LCD_draw_state_Z_offset(int16_t manual_offsetX, int16_t manual_offsetY, int16_t manual_offsetZ);
void LCD_draw_state_declination();


#endif /* LCD_H_ */