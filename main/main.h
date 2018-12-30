/**
 * @file main.h
 * @date 23.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief Header file of the main.ino file, consists of:
 * 	Global variables, used by leds.ino
 *	Prototypes of functions handling web server files.
 * Version: 1.0
 */

#ifndef MAIN_H
#define MAIN_H

/* LED Global variables */
uint16_t cur_speed;		/* Current speed */
uint8_t cur_pos;		/* Current position */
uint8_t cur_len;		/* Current Number of LEDs */
uint8_t cur_direct;		/* Current direction */

/* File handlers */
void handle_root();
void handle_not_found();
void handle_fcss();
void handle_fjs();
void handle_app_js();

#endif
