/**
 * @file leds.ino
 * @date 24.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief TODO
 *
 */

#include "leds.h"
#include "main.h"

/* Global declarations */
extern uint8_t cur_seq;		/* current sequence TODO in HEADER?? */
extern uint8_t LED_cnt;		/* Auxilliary LED counter */
extern uint16_t cur_speed;	/* Current speed */
extern uint8_t cur_pos;		/* Current position */
extern uint8_t cur_len;		/* Current Number of LEDs */
extern uint8_t cur_direct;	/* Current direction */

uint8_t cur_state;
uint8_t last_len;
uint8_t seq_arr[] = {LED_1, LED_2, LED_3, 
					 LED_4, LED_5, LED_6, 
					 LED_7, LED_8, LED_9};

// TODO array of current lit leds - bool? or BITS
// TODO current param of seq
// TODO current speed

void ydelay(unsigned long ms)
{
    uint32_t start = micros();

    while (ms > 0) {
        yield();
        while ( ms > 0 && (micros() - start) >= 1000) {
            ms--;
            start += 1000;
        }
    }
}

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
 * @brief Performs LED reset and calls appropriate sequence function
 * TODO PARAMETERS
 * @param seq_id Uniquely identifies an LED sequence
 */
inline void change_seq(uint8_t seq_id)
{
	if (!seq_id) { return; }

	if (cur_seq != seq_id)
	{
		led_reset();
		cur_seq = seq_id;
		cur_state = 0;
		LED_cnt = 0;
		last_len = 0;
	}
}

/**
 * @brief TODO
 *
 */
void cur_seq_continue()
{
	switch(cur_seq)
	{
	/* case SEQ_INDIV: Is manual */
	case SEQ_ONE:
    Serial.println("Seq ONE");
		seq_one_by_one();
		break;
	case SEQ_ROW:
     Serial.println("Seq ROW");
		seq_row(cur_pos);
		break;
	case SEQ_COL:
     Serial.println("Seq COLUMN");
		seq_col(cur_pos);
		break;
	case SEQ_CIRC:
      Serial.println("Seq CIRCLE");
		seq_circle(cur_len, cur_direct);
		break;
	case SEQ_SWAP:
      Serial.println("Seq SWAP");
		seq_swap();
		break;
	case SEQ_ARROW:
     Serial.println("Seq ARROW");
		seq_arrow(cur_direct);
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
	if (state > 1 || state < 0) { return; }

	switch(led)
	{
	case 1:
		digitalWrite(LED_1, state);
		break;
	case 2:
		digitalWrite(LED_2, state);
		break;
	case 3:
		digitalWrite(LED_3, state);
		break;
	case 4:
		digitalWrite(LED_4, state);
		break;
	case 5:
		digitalWrite(LED_5, state);
		break;
	case 6:
		digitalWrite(LED_6, state);
		break;
	case 7:
		digitalWrite(LED_7, state);
		break;
	case 8:
		digitalWrite(LED_8, state);
		break;
	case 9:
		digitalWrite(LED_9, state);
		break;
	}
}

/**
 * @brief One by one LED (sweep)
 * @param speed Switching speed 3000 - 100
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
    cur_state = 1; // needed?
		break;
	}
}

/**
 * @brief Row by Row sequence
 * @param speed Switching speed
 * @param pos Starting position
 */
void seq_row(uint8_t pos)
{
	/* TODO local in loop() sufficient? */	
	uint8_t start_pos = pos;
  
  switch(cur_state)
  {
  case 0:
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
      pos = POS_BOTTOM;   /* setting next position */
      break;
    case POS_BOTTOM:
      pos = POS_TOP;
      break;
    }
    set_led(LED_4, OFF);
    set_led(LED_5, OFF);
    set_led(LED_6, OFF);  
    set_led(seq_arr[pos-1], ON);
    set_led(seq_arr[pos], ON);
    set_led(seq_arr[pos+1], ON); 
    LED_cnt = pos;
    cur_state = 3;
    break;
  case 3:
    set_led(seq_arr[LED_cnt-1], OFF);
    set_led(seq_arr[LED_cnt], OFF);
    set_led(seq_arr[LED_cnt+1], OFF);  
    cur_state = 0;
  }
}

/**
 * @brief Column by Column sequence
 * @param speed Switching speed
 * @param pos Starting position
 */
void seq_col(uint8_t pos)
{
	/* TODO local in loop() sufficient? 			
	uint8_t seq_arr[] = {LED_1, LED_2, LED_3, 
						 LED_4, LED_5, LED_6, 
						 LED_7, LED_8, LED_9}; */

  uint8_t wseq_arr[] = { LED_1,  LED_4, LED_7, // TODO
                        LED_2, LED_5, LED_8, 
                        LED_3, LED_6, LED_9};

	uint8_t start_pos = pos;

	switch(cur_state)
	{
	case 0:
		set_led(wseq_arr[start_pos-1], ON);
		set_led(wseq_arr[start_pos], ON);
		set_led(wseq_arr[start_pos+1], ON);
		LED_cnt = start_pos;
		cur_state = 1;
		break;
	case 1:
		set_led(wseq_arr[LED_cnt-1], OFF);
		set_led(wseq_arr[LED_cnt], OFF);
		set_led(wseq_arr[LED_cnt+1], OFF);
		set_led(LED_2, ON);
		set_led(LED_5, ON);
		set_led(LED_8, ON);	
		cur_state = 2;
		break;
	case 2:
    switch(start_pos)
    {
    case POS_TOP:
      pos = POS_BOTTOM;   /* setting next position */
      break;
    case POS_BOTTOM:
      pos = POS_TOP;
      break;
    }
		set_led(LED_2, OFF);
		set_led(LED_5, OFF);
		set_led(LED_8, OFF);	
		set_led(wseq_arr[pos-1], ON);
		set_led(wseq_arr[pos], ON);
		set_led(wseq_arr[pos+1], ON);	
		LED_cnt = pos;
		cur_state = 3;
		break;
	case 3:
		set_led(wseq_arr[LED_cnt-1], OFF);
		set_led(wseq_arr[LED_cnt], OFF);
		set_led(wseq_arr[LED_cnt+1], OFF);	
		cur_state = 0;
	}
}

/**
 * @brief Performs rotation, one by one in a circle fashion
 * @param speed Speed of switching, 3000 - 100
 * @param led_len Total LEDs switch at once 9 - 1
 * @param direct Direction - clockwise / Anticlockwise
 */
void seq_circle(uint8_t led_len, uint8_t direct)
{
  (void)direct;
	uint8_t cseq_arr[] = {LED_1, LED_2, LED_3, 
						  LED_6,		LED_9, 
						  LED_8, LED_7, LED_4};
	int8_t i = LED_cnt;		/* Current Led */
	int8_t y = LED_cnt;
	switch(cur_state)
	{
	case 0:
		for (uint8_t num = 0; num < led_len; num++)
		{	
			set_led(cseq_arr[i], ON);
			i = (i+1)%(TOTAL_LED-1);
		}
    LED_cnt = i;
		cur_state = 1;
		last_len = led_len;
		break;
	case 1:
    y = i-1;
    if (y < 0) { y = 7; }
		for (uint8_t num = 0; num < last_len; num++)
		{	
			set_led(cseq_arr[y], OFF);
			if (!y) { y = 8; }
			y = (y-1)%(TOTAL_LED-1);
		}
		cur_state = 0;
	}
}

/**
 * @brief 
 * @param speed Speed of switching, 3000 - 100
 */
void seq_swap()
{
	switch(cur_state)
	{
	case 0:
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
		cur_state = 3;
		break;
	case 3:
		set_led(LED_1, OFF);
		set_led(LED_3, OFF);
		set_led(LED_7, OFF);
		set_led(LED_9, OFF);
		cur_state = 0;
	}
}

/**
 * @brief 
 * @param speed Speed of switching, 3000 - 100
 * @param direct Direction from which it starts
 */
void seq_arrow(uint8_t direct)
{	

/*	switch(direct)
	{
	case POS_TOP: case POS_LEFT: case POS_RIGHT: case POS_BOTTOM:  TODO make more*/
 (void)direct;
	switch(cur_state)
	{
	case 0:
		set_led(LED_2, ON);
		cur_state = 1;
		break;
	case 1:
		set_led(LED_1, ON);
		set_led(LED_3, ON);
		set_led(LED_5, ON);
		cur_state = 2;
		break;
	case 2:
		set_led(LED_1, OFF);
		set_led(LED_3, OFF);
		set_led(LED_4, ON);
		set_led(LED_6, ON);
		set_led(LED_8, ON);
		cur_state = 3;
		break;
	case 3:
		set_led(LED_4, OFF);
		set_led(LED_6, OFF);
		set_led(LED_7, ON);
		set_led(LED_9, ON);
		cur_state = 4;
		break;
	case 4:
		set_led(LED_2, OFF);
		set_led(LED_7, OFF);
		set_led(LED_9, OFF);
		cur_state = 5;
		break;
	case 5:
		set_led(LED_5, OFF);
		cur_state = 6;
		break;
	case 6:
		set_led(LED_8, OFF);
		cur_state = 0;
	}	
}
