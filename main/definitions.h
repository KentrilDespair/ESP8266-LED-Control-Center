/**
 * @file definitions.h
 * @date 23.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief TODO
 * 
 * Version: 1.0
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* GPIO Pin definitions */
#define PIN_D0 16
#define PIN_D1 5
#define PIN_D2 4
#define PIN_D3 0
#define PIN_D4 2
#define PIN_D5 14
#define PIN_D6 12
#define PIN_D7 13
#define PIN_D8 15

/* LED - pin mappings */
#define LED_1 PIN_D0
#define LED_2 PIN_D1
#define LED_3 PIN_D2
#define LED_4 PIN_D3
#define LED_5 PIN_D4
#define LED_6 PIN_D5
#define LED_7 PIN_D6
#define LED_8 PIN_D7
#define LED_9 PIN_D8
#define TOTAL_LED 9

#define BUFF_SIZE 4096

#define ON HIGH
#define OFF LOW

/* IDs of individual sequences */
#define SEQ_INDIV 10
#define SEQ_ONE 11
#define SEQ_ROW 12
#define SEQ_COL 13
#define SEQ_CIRC 14
#define SEQ_SWAP 15
#define SEQ_ARROW 16

/* TODO more sequences */

/* LED Matrix positions */
#define POS_TOP 1
#define POS_LEFT 3
#define POS_RIGHT 5
#define POS_BOTTOM 7

/* LED Matrix directions */
#define DIR_CLKW 97			/* Clockwise 		*/
#define DIR_ACLKW 98		/* Anticlockwise 	*/

#define TMP_PIN 0			/* For temperature sensor */

#endif
