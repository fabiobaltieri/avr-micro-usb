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

/* This header is shared between the firmware and the host software. It
 * defines the USB request numbers (and optionally data types) used to
 * communicate between the host and the device.
 */

#ifndef __REQUESTS_H_INCLUDED__
#define __REQUESTS_H_INCLUDED__

#define CUSTOM_RQ_ECHO          0
/* Request that the device sends back wValue and wIndex. This is used with
 * random data to test the reliability of the communication.
 */
#define CUSTOM_RQ_ADC0    1
#define CUSTOM_RQ_ADC1    2

#endif /* __REQUESTS_H_INCLUDED__ */
