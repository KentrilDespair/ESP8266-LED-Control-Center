/**
 * @file leds.h
 * @date 24.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief Header file of leds.ino, Consists of:
 * TODO
 *
 * Version: 1.0
 */


#ifndef LEDS_H
#define LEDS_H

#include "definitions.h"


extern void led_reset();
extern void set_led(uint8_t led_pin, uint8_t state);
extern void change_seq(uint8_t seq_id);
void cur_seq_continue();

/* Single sequences */
/* TODO CHANGE HEADERS HAVE GLOBALS ! */
void seq_individual(int8_t led, int8_t state);
void seq_one_by_one();
void seq_row(uint8_t pos);
void seq_col(uint8_t pos);
void seq_circle(uint8_t led_len, uint8_t direct);
void seq_swap();
void seq_arrow(uint8_t direct);

#endif
