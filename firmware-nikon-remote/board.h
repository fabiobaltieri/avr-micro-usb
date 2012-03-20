/* LEDs */

#define LED_PORT      PORTB
#define LED_DDR       DDRB
#define LED_IR        PB3
#define LED_R         PB0

#define led_ir_on() do {			\
		LED_PORT &= ~_BV(LED_IR);	\
		LED_PORT |=  _BV(LED_R);	\
	} while (0)
#define led_r_on() do {					\
		LED_PORT |=  _BV(LED_IR);		\
		LED_PORT &= ~_BV(LED_R);		\
	} while (0)

#define led_off() do {						\
		LED_PORT &= ~(_BV(LED_IR) | _BV(LED_R));	\
	} while (0)

#define led_init() do {					\
		LED_DDR |= _BV(LED_IR) | _BV(LED_R);	\
	} while (0);
