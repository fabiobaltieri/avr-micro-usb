/*
 * Copyright 2011 Fabio Baltieri (fabio.baltieri@gmail.com)
 *
 * Based on the original V-USB code written by
 *  Christian Starkjohann, Copyright 2008, OBJECTIVE DEVELOPMENT Software GmbH
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include <avr/eeprom.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */
#include "requests.h"       /* The custom request numbers we use */

#include "adc.h"

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
  usbRequest_t    *rq = (void *)data;
  static unsigned char dataBuffer[4];
  static int16_t  val;  /* buffer must stay valid when usbFunctionSetup returns */

  if(rq->bRequest == CUSTOM_RQ_ECHO){ /* echo -- used for reliability tests */
    dataBuffer[0] = rq->wValue.bytes[0];
    dataBuffer[1] = rq->wValue.bytes[1];
    dataBuffer[2] = rq->wIndex.bytes[0];
    dataBuffer[3] = rq->wIndex.bytes[1];
    usbMsgPtr = dataBuffer;         /* tell the driver which data to return */
    return 4;
  }else if(rq->bRequest == CUSTOM_RQ_ADC0){
    val  = adc_get(ADMUX_REF_VCC | ADMUX_MUX_ADC3);
    val += adc_get(ADMUX_REF_VCC | ADMUX_MUX_ADC3);
    val += adc_get(ADMUX_REF_VCC | ADMUX_MUX_ADC3);
    val += adc_get(ADMUX_REF_VCC | ADMUX_MUX_ADC3);
    val /= 4;
    usbMsgPtr = (unsigned char *)&val;         /* tell the driver which data to return */
    return 2;
  }else if(rq->bRequest == CUSTOM_RQ_ADC1){
    val  = adc_get(ADMUX_REF_1V1 | ADMUX_MUX_TEMP);
    val += adc_get(ADMUX_REF_1V1 | ADMUX_MUX_TEMP);
    val += adc_get(ADMUX_REF_1V1 | ADMUX_MUX_TEMP);
    val += adc_get(ADMUX_REF_1V1 | ADMUX_MUX_TEMP);
    val /= 4;
    usbMsgPtr = (unsigned char *)&val;         /* tell the driver which data to return */
    return 2;
  }
  return 0;   /* default for not implemented requests: return no data back to host */
}

static void calibrateOscillator(void)
{
  uchar       step = 128;
  uchar       trialValue = 0, optimumValue;
  int         x, optimumDev, targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

  /* do a binary search: */
  do{
    OSCCAL = trialValue + step;
    x = usbMeasureFrameLength();    /* proportional to current real frequency */
    if(x < targetValue)             /* frequency still too low */
      trialValue += step;
    step >>= 1;
  }while(step > 0);
  /* We have a precision of +/- 1 for optimum OSCCAL here */
  /* now do a neighborhood search for optimum value */
  optimumValue = trialValue;
  optimumDev = x; /* this is certainly far away from optimum */
  for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
    x = usbMeasureFrameLength() - targetValue;
    if(x < 0)
      x = -x;
    if(x < optimumDev){
      optimumDev = x;
      optimumValue = OSCCAL;
    }
  }
  OSCCAL = optimumValue;
}

void    hadUsbReset(void)
{
  calibrateOscillator();
  eeprom_write_byte(0, OSCCAL);   /* store the calibrated value in EEPROM */
}


/* ------------------------------------------------------------------------- */

int __attribute__((noreturn)) main(void)
{
  uchar   i;
  uchar   calibrationValue;

  calibrationValue = eeprom_read_byte(0); /* calibration value from last time */
  if(calibrationValue != 0xff){
    OSCCAL = calibrationValue;
  }

  wdt_enable(WDTO_1S);
  /* Even if you don't use the watchdog, turn it off here. On newer devices,
   * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
   */
  /* RESET status: all port bits are inputs without pull-up.
   * That's the way we need D+ and D-. Therefore we don't need any
   * additional hardware initialization.
   */
  odDebugInit();
  DBG1(0x00, 0, 0);       /* debug output: main starts */
  usbInit();
  usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
  i = 0;
  while(--i){             /* fake USB disconnect for > 250 ms */
    wdt_reset();
    _delay_ms(1);
  }
  usbDeviceConnect();
  sei();
  DBG1(0x01, 0, 0);       /* debug output: main loop starts */
  for(;;){                /* main event loop */
    DBG1(0x02, 0, 0);   /* debug output: main loop iterates */
    wdt_reset();
    usbPoll();
  }
}

/* ------------------------------------------------------------------------- */
