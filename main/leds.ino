/**
 * @file leds.ino
 * @date 24.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief Implements LED switching/sequences, consists of:
 *	- Global variables section
 *	- Function definitions
 * Version: 1.0
 */

#include "leds.h"
#include "main.h"


/* Global declarations */
extern uint16_t cur_speed;	/**< Current speed 							*/
extern uint8_t cur_pos;		/**< Current position 						*/
extern uint8_t cur_len;		/**< Current Number of LEDs					*/
extern uint8_t cur_direct;	/**< Current direction 						*/

static uint8_t cur_seq;		/**< Current sequence 						*/
static uint8_t cur_state;	/**< Current state of sequence 				*/
static uint8_t LED_cnt;		/**< Auxilliary LED counter					*/
static uint8_t last_len;	/**< To save last LED length				*/
static uint8_t seq_arr[] = {LED_1, LED_2, LED_3, 	/**< Matix of LEDs 	*/
							LED_4, LED_5, LED_6, 
							LED_7, LED_8, LED_9};

/**
 * @brief Sets all LED pins to digital LOW
 */
inline void led_reset()
{
	digitalWrite(LED_1, LOW);
	digitalWrite(LED_2, LOW);
	digitalWrite(LED_3, LOW);
	digitalWrite(LED_4, LOW);
	digitalWrite(LED_5, LOW);
	digitalWrite(LED_6, LOW);
	digitalWrite(LED_7, LOW);
	digitalWrite(LED_8, LOW);
	digitalWrite(LED_9, LOW);
}

/**
 * @brief Switches an LED on/off
 * @param led_pin Predefined LED pin
 * @param state ON or OFF
 */
inline void set_led(uint8_t led_pin, uint8_t state)
{
	digitalWrite(led_pin, state);
}

/**
 * @brief If current sequence changes, appropriate resets are made
 * @param seq_id Uniquely identifies an LED sequence
 */
inline void change_seq(uint8_t seq_id)
{
	if (!seq_id) { return; }

	if (cur_seq != seq_id)		/* New sequence */
	{
		led_reset();
		cur_seq = seq_id;
		cur_state = LED_cnt = last_len = 0;
	}
}

/**
 * @brief According to current sequence ID calls appropriate
 *	sequence function
 */
void cur_seq_continue()
{
	switch(cur_seq)
	{
	/* case SEQ_INDIV: Is manual */
	case SEQ_ONE:
		Serial.println("Sequence ONE");	// TODO REMOVE
		seq_one_by_one();
		break;
	case SEQ_ROW:
		Serial.println("Sequence ROW");
		seq_row();
		break;
	case SEQ_COL:
		Serial.println("Sequence COLUMN");
		seq_col();
		break;
	case SEQ_CIRC:
		Serial.println("Sequence CIRCLE");
		seq_circle();
		break;
	case SEQ_SWAP:
		Serial.println("Sequence SWAP");
		seq_swap();
		break;
	case SEQ_ARROW:
		Serial.println("Sequence ARROW");
		seq_arrow();
		break;
	}
}

/**
 * @brief Individual LED on/off
 * @param led Number of the led
 * @param state 1 ON, 0 OFF
 */
void seq_individual(int8_t led, int8_t state)
{	
	if (state > 1 || state < 0) { 
		return;
	}

	if (led > 0 && led < 10) {
		set_led(seq_arr[led-1], state);
	}
}

/**
 * @brief One by one LED (sweep)
 * @param speed Switching speed 100 - 3000
 */
void seq_one_by_one()
{
	switch(cur_state)
	{
	case 0:
		set_led(seq_arr[LED_cnt], ON);
		cur_state = 1;
		break;
	case 1:
    	set_led(seq_arr[LED_cnt], OFF);
		LED_cnt = (LED_cnt+1)%TOTAL_LED;
    	set_led(seq_arr[LED_cnt], ON);
		break;
	}
}

/**
 * @brief Row by Row sequence
 * @param speed Switching speed
 * @param pos Starting position
 */
void seq_row()
{
	uint8_t start_pos = cur_pos;

	switch(cur_state)
	{
	case 0:
		if (LED_cnt != 0) {
			set_led(seq_arr[LED_cnt-1], OFF);
			set_led(seq_arr[LED_cnt], OFF);
			set_led(seq_arr[LED_cnt+1], OFF);       
		}
		set_led(seq_arr[start_pos-1], ON);
		set_led(seq_arr[start_pos], ON);
		set_led(seq_arr[start_pos+1], ON);
		LED_cnt = start_pos;
		cur_state = 1;
		break;
	case 1:
		set_led(seq_arr[LED_cnt-1], OFF);
		set_led(seq_arr[LED_cnt], OFF);
		set_led(seq_arr[LED_cnt+1], OFF);
		set_led(LED_4, ON);
		set_led(LED_5, ON);
		set_led(LED_6, ON); 
		cur_state = 2;
		break;
	case 2:
		switch(start_pos)
		{
		case POS_TOP:
  			start_pos = POS_BOTTOM;   /* setting next position */
  			break;
		case POS_BOTTOM:
  			start_pos = POS_TOP;
  			break;
		}
		set_led(LED_4, OFF);
		set_led(LED_5, OFF);
		set_led(LED_6, OFF);  
		set_led(seq_arr[start_pos-1], ON);
		set_led(seq_arr[start_pos], ON);
		set_led(seq_arr[start_pos+1], ON); 
		LED_cnt = start_pos;
		cur_state = 0;
	}
}

/**
 * @brief Column by Column sequence
 * @param speed Switching speed
 * @param pos Starting position
 */
void seq_col()
{
	uint8_t start_pos = cur_pos;

	switch(cur_state)
	{
	case 0:
		if (LED_cnt >= POS_LEFT) {
			set_led(seq_arr[LED_cnt-3], OFF);
			set_led(seq_arr[LED_cnt], OFF);
			set_led(seq_arr[LED_cnt+3], OFF);
		}
		set_led(seq_arr[start_pos-3], ON);
		set_led(seq_arr[start_pos], ON);
		set_led(seq_arr[start_pos+3], ON);
		LED_cnt = start_pos;
		cur_state = 1;
		break;
	case 1:
		set_led(seq_arr[LED_cnt-3], OFF);
		set_led(seq_arr[LED_cnt], OFF);
		set_led(seq_arr[LED_cnt+3], OFF);
		set_led(LED_2, ON);
		set_led(LED_5, ON);
		set_led(LED_8, ON);	
		cur_state = 2;
		break;
	case 2:
		switch(start_pos)
		{
		case POS_LEFT:
      		start_pos = POS_RIGHT;
			break;
		case POS_RIGHT:
			start_pos = POS_LEFT;
			break;
		}
		set_led(LED_2, OFF);
		set_led(LED_5, OFF);
		set_led(LED_8, OFF);	
		set_led(seq_arr[start_pos-3], ON);
		set_led(seq_arr[start_pos], ON);
		set_led(seq_arr[start_pos+3], ON);	
		LED_cnt = start_pos;
		cur_state = 0;
	}
}

/**
 * @brief Performs rotation, one by one in a circle fashion
 * @param speed Speed of switching, 100 - 3000
 * @param led_len Total LEDs switch at once 1 - 9
 * @param direct Direction - clockwise / Anticlockwise
 */
void seq_circle()
{
	uint8_t cseq_arr[] = {LED_1, LED_2, LED_3, 
						  LED_6,		LED_9, 
						  LED_8, LED_7, LED_4};	

	int8_t i = LED_cnt;		/* Current index of LED */
	bool add_up = false;

	led_reset();          /* simply reset */
	if (cur_len > 1) { add_up = true; }

	if (cur_direct == DIR_ACLKW) 
	{
		for (uint8_t num = 0; num < cur_len; num++)
		{
			set_led(cseq_arr[i], ON);
			if (add_up && num+1 == cur_len) {
				break;
			}
			if (i == 0) { i = 8; }
			i = (i-1)%(TOTAL_LED-1);
		}
	} 
	else 
	{
		for (uint8_t num = 0; num < cur_len; num++)
		{
			set_led(cseq_arr[i], ON);
			if (add_up && num+1 == cur_len) {
				break;
			}
			i = (i+1)%(TOTAL_LED-1);        
		}
	}
	LED_cnt = i;
	last_len = cur_len;
}

/**
 * @brief Swapping outwards, switching leds outwards from the center
 * @param speed Speed of switching, 100 - 3000
 */
void seq_swap()
{
	switch(cur_state)
	{
	case 0:
		set_led(LED_1, OFF);
		set_led(LED_3, OFF);
		set_led(LED_7, OFF);
		set_led(LED_9, OFF);
    
		set_led(LED_5, ON);
		cur_state = 1;
		break;
	case 1:
		set_led(LED_5, OFF);

		set_led(LED_2, ON);
		set_led(LED_4, ON);
		set_led(LED_6, ON);
		set_led(LED_8, ON);
		cur_state = 2;
		break;
	case 2:
		set_led(LED_2, OFF);
		set_led(LED_4, OFF);
		set_led(LED_6, OFF);
		set_led(LED_8, OFF);

		set_led(LED_1, ON);
		set_led(LED_3, ON);
		set_led(LED_7, ON);
		set_led(LED_9, ON);
		cur_state = 0;
	}
}

/**
 * @brief Imitates an arrow on 3x3 matrix -->
 * @param speed Speed of switching, 100 - 3000
 * @param pos Direction which it starts from
 */
void seq_arrow()
{	
	if (LED_cnt != cur_pos)	{ 	/* Position has changed */
		led_reset();
		LED_cnt = cur_pos; 		/* save previous position */
	}

	uint8_t arw_leds[] = {	LED_2, 
							LED_1, LED_3, LED_5, 
						 	LED_4, LED_6, LED_8,
						 	LED_7, LED_9		};

	switch(cur_pos)				/* Adjust the array */
	{
	case POS_LEFT: 
		arw_leds[0] = LED_4; arw_leds[2] = LED_7; 
		arw_leds[4] = LED_2; arw_leds[5] = LED_8;
		arw_leds[6] = LED_6; arw_leds[7] = LED_3;
		break;	
	case POS_RIGHT: 
		arw_leds[0] = LED_6; arw_leds[1] = LED_9;
		arw_leds[4] = LED_8; arw_leds[5] = LED_2;
		arw_leds[6] = LED_4; arw_leds[8] = LED_1;
		break;
	case POS_BOTTOM:
		arw_leds[0] = LED_8; arw_leds[1] = LED_9;
		arw_leds[2] = LED_7; arw_leds[6] = LED_2;
		arw_leds[7] = LED_1; arw_leds[8] = LED_3;
	}

	switch(cur_state)
	{
	case 0:
		set_led(arw_leds[0], ON);
		cur_state = 1;
		break;
	case 1:
		set_led(arw_leds[1], ON);
		set_led(arw_leds[2], ON);
		set_led(arw_leds[3], ON);
		cur_state = 2;
		break;
	case 2:
		set_led(arw_leds[1], OFF);
		set_led(arw_leds[2], OFF);
		set_led(arw_leds[4], ON);
		set_led(arw_leds[5], ON);
		set_led(arw_leds[6], ON);
		cur_state = 3;
		break;
	case 3:
		set_led(arw_leds[4], OFF);
		set_led(arw_leds[5], OFF);
		set_led(arw_leds[7], ON);
		set_led(arw_leds[8], ON);
		cur_state = 4;
		break;
	case 4:
		set_led(arw_leds[0], OFF);
		set_led(arw_leds[7], OFF);
		set_led(arw_leds[8], OFF);
		cur_state = 5;
		break;
	case 5:
		set_led(arw_leds[3], OFF);
		cur_state = 6;
		break;
	case 6:
		set_led(arw_leds[6], OFF);
		cur_state = 0;
	}
}
