/* LEDs */

#define LED_A_PORT    PORTB
#define LED_A_DDR     DDRB
#define LED_A         PB0

#define led_a_on()     LED_A_PORT &= ~_BV(LED_A);
#define led_a_off()    LED_A_PORT |=  _BV(LED_A);
#define led_a_toggle() LED_A_PORT ^=  _BV(LED_A);

#define led_init()				\
	do {					\
		LED_A_DDR |= _BV(LED_A);	\
	} while (0);
