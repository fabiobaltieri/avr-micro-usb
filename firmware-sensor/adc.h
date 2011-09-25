#define ADMUX_REF_VCC  ((0 << REFS2) | (0 << REFS1) | (0 << REFS0))
#define ADMUX_REF_1V1  ((0 << REFS2) | (1 << REFS1) | (0 << REFS0))
#define ADMUX_REF_2V56 ((1 << REFS2) | (1 << REFS1) | (0 << REFS0))

#define ADMUX_MUX_ADC3 0x3
#define ADMUX_MUX_TEMP 0xf

uint16_t adc_get (uint8_t cfg);
