/**
 * @file leds.h
 * @date 24.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief Header file of leds.ino, Consists of:
 * TODO
 *
 * Version: 1.0
 */


#include "definitions.h"


extern void led_reset();
extern void set_led(uint8_t led_pin, uint8_t state);
extern void change_seq(uint8_t seq_id);


/* Single sequences */
void seq_individual(uint8_t led_pin, bool state);
void seq_one_by_one(uint8_t speed);
void seq_row(uint8_t speed, uint8_t pos);
void seq_col(uint8_t speed, uint8_t pos);
void seq_circle(uint8_t speed, uint8_t led_len, uint8_t direct);
void seq_swap(uint8_t speed);
void seq_arrow(uint8_t speed, uint8_t direct);
