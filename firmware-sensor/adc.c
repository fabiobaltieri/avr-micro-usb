#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "adc.h"

uint16_t adc_get (uint8_t cfg)
{
  ADCSRA = ( (1 << ADEN) | /* enable           */
             (0 << ADSC) | /* start conversion */
             (0 << ADATE) | /* free running     */
             (1 << ADIF) | /* clear interrupts */
             (0 << ADIE) | /* interrupt enable */
             (1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0) );

  ADMUX = cfg;

  /* wait Vref to stabilize */
  _delay_ms(1);

  ADCSRA |= _BV(ADSC);

  loop_until_bit_is_clear(ADCSRA, ADSC);

  ADCSRA = 0x00; /* ADC disable */

  return ADCW;
}
