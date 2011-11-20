/*
 * Copyright 2011 Fabio Baltieri (fabio.baltieri@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 */

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
