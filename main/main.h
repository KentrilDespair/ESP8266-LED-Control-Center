/**
 * @file main.h
 * @date 23.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief Header file of the main.ino file, consists of:
 * TODO
 *
 * Version: 1.0
 */

#ifndef MAIN_H
#define MAIN_H

/* LED Global variables */
uint8_t cur_seq;		/* current sequence TODO in HEADER?? */
uint8_t LED_cnt;		/* Auxilliary LED counter */
uint16_t cur_speed;	/* Current speed */
uint8_t cur_pos;		/* Current position */
uint8_t cur_len;		/* Current Number of LEDs */
uint8_t cur_direct;	/* Current direction */

void get_diag_info();
void new_cli();
void send_file(const char *fn, const char *c_type);

/* File handlers */
void handle_root();
void handle_not_found();
void handle_fcss();
void handle_fjs();
void handle_app_js();

#endif
