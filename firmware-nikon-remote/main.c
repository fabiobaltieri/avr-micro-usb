#include "board.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usbdrv.h"

#include "requests.h"

static void reset_cpu(void)
{
	wdt_disable();

	wdt_enable(WDTO_15MS);

	for (;;);
}

static void pulse(uint16_t us)
{
	uint16_t i;

	for (i = 0; i < (us) / 26; i++) {
		led_ir_on();
		_delay_us(13);
		led_off();
		_delay_us(13);
	}
}
static void space(uint16_t us)
{
	uint16_t i;

	for (i = 0; i < (us) / 26; i++)
		_delay_us(26);
}

usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
	struct usbRequest *rq = (void *)data;
	uint8_t i;

	switch (rq->bRequest) {
	case CUSTOM_RQ_SHOOT:
		led_r_on();
		_delay_ms(20);
		led_off();
		_delay_us(20);
	case CUSTOM_RQ_SHOOT_SILENT:
	cli();
		for (i = 0; i < 2; i++) {
			pulse(2000);
			space(27830);
			pulse(400);
			space(1580);
			pulse(400);
			space(3580);
			pulse(400);

			_delay_ms(63);
		}
	sei();
		break;
	case CUSTOM_RQ_IR_ON:
		led_ir_on();
		break;
	case CUSTOM_RQ_R_ON:
		led_r_on();
		break;
	case CUSTOM_RQ_R_BLINK:
		led_r_on();
		while (rq->wValue.word--)
			_delay_ms(1);
		led_off();
		break;
	case CUSTOM_RQ_OFF:
		led_off();
		break;
	case CUSTOM_RQ_RESET:
		reset_cpu();
		break;
	}

	return 0;
}

int __attribute__((noreturn)) main(void)
{
	uint8_t i;

	led_init();
	led_off();

	wdt_enable(WDTO_1S);

	usbInit();
	usbDeviceDisconnect();

	i = 0;
	while (--i) {
		wdt_reset();
		_delay_ms(1);
	}

	usbDeviceConnect();

	sei();
	for (;;) {
		wdt_reset();
		usbPoll();
	}
}
