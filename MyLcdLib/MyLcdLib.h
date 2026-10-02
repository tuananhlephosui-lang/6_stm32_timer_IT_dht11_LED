#ifndef MYLCDLIB_H
#define MYLCDLIB_H
#include <stdint.h>
#include <string.h>
#include "stm32f1xx_hal.h"

#define MDATA 0x01
#define MCMD	0x00

typedef struct {
	GPIO_TypeDef 	*PORT;
	uint16_t			PIN;
}PORT_PIN;

typedef struct {
	PORT_PIN	D0;
	PORT_PIN	D1;
	PORT_PIN	D2;
	PORT_PIN	D3;
	PORT_PIN	D4;
	PORT_PIN	D5;
	PORT_PIN	D6;
	PORT_PIN	D7;
	PORT_PIN	RS;
	PORT_PIN	RW;
	PORT_PIN	En;
}LCD_Config_t;


void LCD_init(const LCD_Config_t *lcd);
void LCD_Write_Byte(const LCD_Config_t *lcd, const uint8_t* data, uint8_t mode);
void LCD_Write_data(const LCD_Config_t *lcd, uint8_t data);
void LCD_Write_cmd(const LCD_Config_t *lcd, uint8_t cmd);
void LCD_Print(const LCD_Config_t *lcd, char *str);
void LCD_SetCursor(const LCD_Config_t *lcd, uint8_t row, uint8_t col);
void LCD_ScrollText_Circular(const LCD_Config_t *lcd, const char *str, uint16_t delay_ms);

#endif // MYLCDLIB_H