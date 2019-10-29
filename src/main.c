#define ENABLE_SERIAL

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <string.h>

#include "main.h"
#include "font.c"
#ifdef ENABLE_SERIAL
#include "usb_serial.h"
#else
#define usb_init() ;
#define usb_serial_putchar(...) ;
#define usb_serial_getchar() -1
#endif

// #define WELCOME_MESSAGE "Laser display v0.9"
#define WELCOME_MESSAGE "skomplikowany system luster"


#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))


#define DELAY_DOT_ON   60
#define DELAY_DOT_OFF  (70 - DELAY_DOT_ON) // 20

#define PIN_LASER  (1 << 0)
#define PIN_BUTTON (1 << 1)

#define laser(v) \
	if (v) \
		PORTF |=  PIN_LASER; \
	else \
		PORTF &= ~PIN_LASER

// rewrite to asm?
#define pixel(v) \
		do { \
			laser((v)); \
			_delay_us(DELAY_DOT_ON); \
			laser(0); \
			_delay_us(DELAY_DOT_OFF); \
		} while(0)

#define toggle_scrolling() \
	do { \
		if (++scrolling >= 3) \
			scrolling = 0; \
		if (!scrolling) \
			view_offset = NUM_SCREEN_COLS; \
	} while (0)

#define NUM_SCREEN_COLS  32
#define NUM_SCREEN_ROWS  4

// 48   32 + 240
#define NUM_BUFFER_COLS  (NUM_SCREEN_COLS * 10)
#define NUM_BUFFER_ROWS   NUM_SCREEN_ROWS

#define BUFFER_SIZE  (NUM_BUFFER_COLS * NUM_BUFFER_ROWS / 8)

union _buffer {
	uint8_t  bytes[BUFFER_SIZE];
	uint8_t  bytes2d[NUM_BUFFER_ROWS][NUM_BUFFER_COLS / 8];
	// uint32_t lines[NUM_BUFFER_ROWS];
};

static union _buffer buffer;
static uint16_t scroll_offset = 0;
static uint16_t write_offset  = 0;
static uint16_t view_offset   = 0;
static uint8_t view_offset_m  = 0;

#define DATA_SIZE (NUM_SCREEN_COLS * NUM_SCREEN_ROWS / 8)

// const uint8_t PROGMEM data[DATA_SIZE] = {
// 	// "ELO!:DDD"
// 	0xc8, 0xa0, 0x4a, 0xaa, // line 2
// 	0xee, 0xe6, 0xac, 0xcc, // line 3
// 	0xe8, 0xe6, 0x0c, 0xcc, // line 0
// 	0x88, 0xa6, 0xaa, 0xaa, // line 1
// };


// #define TEXT_SIZE 200
// uint8_t text_buffer[TEXT_SIZE];

// volatile uint8_t *d = buffer.bytes;
volatile uint8_t const *d = NULL;

// should be computed based on how much time the last rev took?
// or add some button to calibrate for the time it's pressed to calibrate (for dev)
// and in the future calibrate for a couple of seconds?
// http://www.avrfreaks.net/forum/how-measure-time-between-two-interrupts
// #define PADDING_LEFT 180
#define PADDING_LEFT 180
#define PADDING_LINE 3400

// 16mhz
// 16 nops = 1µs
//  1 nop  = 62.5ns
uint8_t measure = 0;
static uint16_t delay[5] = {0,
	PADDING_LEFT,
	PADDING_LEFT + PADDING_LINE + 530 - 1120 - 58,
	PADDING_LEFT + PADDING_LINE + 300 - 1030 + 335,
	PADDING_LEFT + PADDING_LINE + 180 - 1030 + 375,
};
uint16_t *du;

// 9ms ~= 1 line

void delay_us(uint16_t n) {
	while (n--) {
		_delay_us(1);
	}
}

// another revolution
ISR(INT0_vect) {
	static volatile uint8_t row;
	static volatile uint8_t col;
	static volatile uint8_t b;

#define MEASURE_RESOLUTION 3 // 0..5 for TCCR1B
// 0x01..0x05 => clock = 1 / (1, 8, 64, 256, 1024)
// 0x01  (1 /    1)  16000000 Hz
// 0x02  (1 /    8)   2000000 Hz
// 0x03  (1 /   64)    250000 Hz
// 0x04  (1 /  256)     62500 Hz
// 0x05  (1 / 1024)     15625 Hz
// #define _MEASURE_DIVISOR(v) ((v < 3)? (2000000.0 * (3 - v)): (250000 >> (2 * (5 - v))))
#define MEASURE_DIVISOR(v) \
	(((v == 1)? 16000000: \
	  ((v == 2)? 20000000: \
	   ((v == 3)? 250000: \
		((v == 4)? 62500: \
		 15625)))))
// #define MEASURE_DIVISOR _MEASURE_DIVISOR(MEASURE_RESOLUTION)

	switch (measure) {
	case 1:
		TCNT1 = 0;
		// TCCR1B = 0;
		// TCCR1B = 0x01; // too fast for current motor
		// 0x02 => 2MHz timer increment
		// delay = 50633 => 101266 (2MHz)
		// 101266 / 2000000 = sth (frame time in seconds)
		// rpm = 60 * 1 / sth = sthsth
		TCCR1B = MEASURE_RESOLUTION;
		// TCCR1B = 0x05; // not dokładne enough
		measure++;
		return;
		break;

	case 2:
		delay[0] = TCNT1;
		TCCR1B = 0x00;
		measure = 0;

		{
			uint16_t s = 1000.0 *  1.0 /
				((double)delay[0] / MEASURE_DIVISOR(MEASURE_RESOLUTION));
				 // * 60 to get rpm

			usb_serial_putchar(':');
			usb_serial_putchar('0' + ((s / 10000) % 10));
			usb_serial_putchar('0' + ((s / 1000) % 10));
			usb_serial_putchar('.');
			usb_serial_putchar('0' + ((s / 100) % 10));
			usb_serial_putchar('0' + ((s / 10) % 10));
			usb_serial_putchar('0' +  (s      % 10));
			usb_serial_putchar(' ');
			// usb_serial_putchar( & (1 << TOV1)? '1': '0'); // overflow
			usb_serial_putchar('0' + MEASURE_RESOLUTION);
			usb_serial_putchar('\r');
			usb_serial_putchar('\n');

			delay[1] = 180;
			delay[2] = 180 + 3400 + 120 - 1120 - 58  + s / 96.878049; // 530 - 120; 410
			delay[3] = 180 + 3400 + 40  - 1030 + 335 + s / 147.111111; // 300 - 030; 270
			delay[4] = 180 + 3400 + 30  - 1030 + 375 + s / 261.8; // 180 - 030; 150
		}
		return;
		break;
	}

	view_offset_m = view_offset % 8;
	d = buffer.bytes + view_offset / 8; // make it nice

#define DELAY_DYNAMIC

// #define DELAY_COMPENSATE 0
// #define __delay_us(v) _delay_us(v)


#ifdef DELAY_DYNAMIC
	// du = delay[1];
#endif

	for (row = 0; row < NUM_SCREEN_ROWS; row++) {
#ifndef DELAY_DYNAMIC
		switch (row) {
		// screen padding
#define DELAY_COMPENSATE 0
#define __delay_us(v) delay_us(v)
		case 0: __delay_us(PADDING_LEFT - DELAY_COMPENSATE); break;
		// should be the same for the rest, but hw is kinda lame for now ;P
#define DELAY_COMPENSATE 1120
		case 1: __delay_us(PADDING_LEFT + PADDING_LINE + 530 - DELAY_COMPENSATE); break;
#define DELAY_COMPENSATE 1030
		case 2: __delay_us(PADDING_LEFT + PADDING_LINE + 300 - DELAY_COMPENSATE); break;
		case 3: __delay_us(PADDING_LEFT + PADDING_LINE + 180 - DELAY_COMPENSATE); break;
		}
#else
		// delay_us(*du++);
		delay_us(delay[1 + row]);
#endif

		// TODO rewrite to spi?
		if (view_offset_m) {
			for (b = view_offset_m; b < 8; b++)
				pixel(*d & (0x80 >> b));

			d++;
		}

		for (col = 0; col < NUM_SCREEN_COLS / 8 - (view_offset_m && 1); col++) {
			for (b = 0; b < 8; b++)
				pixel(*d & (0x80 >> b));

			d++;
		}

		if (view_offset_m) {
			for (b = 0; b < view_offset_m; b++)
				pixel(*d & (0x80 >> b));
		}

		// jump to the next line on the same column
		d += NUM_BUFFER_COLS / 8 - NUM_SCREEN_COLS / 8;
	}
}

void draw_pixel(uint16_t x, uint16_t y, uint8_t color) {
	if (x > NUM_BUFFER_COLS || y > NUM_BUFFER_ROWS)
		return;

	// temp
	switch (y) {
	case 0: case 1: y += 2; break;
	case 2: case 3: y -= 2; break;
	}

	if (color)
		buffer.bytes2d[y][x >> 3] |=   0x80 >> (x & 0x07);
	else
		buffer.bytes2d[y][x >> 3] &= ~(0x80 >> (x & 0x07));
}

void draw_letter(uint8_t c) {
	if (c < 32 || c > 127)
		return;

	// usb_serial_putchar(':');
	// usb_serial_putchar(c);

	static uint8_t i, y, yo, glyph;
	// static uint8_t i, n, yo, glyph;

	// move small letters 1px down
	// should also move ',' and ';', but it doesn't have 5 rows
	if (c >= 'a' && c <= 'z'
		&& c != 'j' && c != 'g' && c != 'p' && c != 'q' && c != 'y')
		yo = 1;
	else
		yo = 0;

	// erase line above the small letter
	if (yo)
		for (i = 0; i < 4; i++)
			draw_pixel(write_offset + i, 0, 0);

	for (i = 0; i < 4; i++) {
		// usb_serial_putchar('\r');
		// usb_serial_putchar('\n');

		// TODO just read next bytes here
		// pgm_read_dword?
		glyph = pgm_read_byte(TINYFONT_SPRITE + ((c - 32) / 2) * 4 + i)
			>> (c & 1) * 4;

		// usb_serial_putchar(' ');

		for (y = 0; y < 4; y++) {
			draw_pixel(write_offset + i, y + yo, glyph & (1 << y));
			// usb_serial_putchar(glyph & (0x08 >> y)? '1': '0');
		}
	}

	// for (n = 0; n < 2; n++) {
	// 	glyph = pgm_read_byte(TINYFONT_SPRITE + (c - 32) * 2 + n);
	// 	// glyph = pgm_read_word(TINYFONT_SPRITE + (c - 32) * 2 + n);

	// 	// usb_serial_putchar(' ');

	// 	for (i = 0; i < 8; i++) {
	// 		draw_pixel(write_offset + (i & 0x03), i / 4 + n - yo,
	// 				   glyph & (0x80 >> i));
	// 		// usb_serial_putchar(glyph & (0x80 >> i)? '1': '0');
	// 	}
	// }

	write_offset += FONT_WIDTH; // 4 for small letters and special chars?

	// and erase space after the letter
	for (i = 0; i < 4; i++) {
		draw_pixel(write_offset - 1, i, 0);
	}

	// usb_serial_putchar('\r');
	// usb_serial_putchar('\n');
}

int main() {
	CPU_PRESCALE(0); // fix delays

	// D0 - optiodiode
	DDRD  = 0x00;
	PORTD = 0x01;
	EICRA = 0x02; // falling edge
	EIMSK = 0x01;

	// F0 - laser
	DDRF  = PIN_LASER;
	// PORTF = 0x00;
	laser(0);

	// button
	DDRF |= PIN_BUTTON;

	// timer to adjust line delay
	// TCNT1  = 0;
	TCCR1B = 0; // 0x01..0x05 => clock = 1 / (1, 8, 64, 256, 1024)
	// OCR1A  = 0x02;
	// OCR1A  = 0x00;
	// XXX clear PRR0 to enable?
	// PRR0 = ;
	// TCCR0 = 0x01;
	// line_delay = measured_difference / mirrors_count +/- sth;

	// timer for scrolling
	// TCNT0 = ;
	TCCR0B = 0x05;

	// uart
	usb_init();

	sei();

	uint8_t scrolling = 0;
	view_offset = 0;

	buffer.bytes2d[0][4] = 0xc0;
	buffer.bytes2d[1][4] = 0xf0;
	buffer.bytes2d[2][4] = 0xfc;
	buffer.bytes2d[3][4] = 0xff;
	view_offset = 4 * 8;

	// leave some space for nice scrolling effect
	write_offset = NUM_SCREEN_COLS;
	scroll_offset = NUM_SCREEN_COLS;

	// char *m = WELCOME_MESSAGE;
	// while (*m)
	// 	draw_letter((uint8_t)*m++);
	// scroll_offset = write_offset;
	// write_offset = NUM_SCREEN_COLS;

	int16_t c = -1;

	for (;;) {
		// check usb_configured()?
		c = usb_serial_getchar();

		// write letter into display buffer
		// use special codes to toggle scrolling/changin font sizes and stuff
		if (c >= 0) {
			if (c >= 32 && c < 128) {
				draw_letter(c);
			} else {
				switch (c) {
				case 0:
				case '\r':
				case '\n':
					// just erase the next 32 columns since scrolling won't go
					// further then that
					for (uint8_t j = 0; j < NUM_SCREEN_ROWS; j++)
						for (uint8_t i = 0; i < NUM_SCREEN_COLS; i++)
							draw_pixel(write_offset + i, j, 0);

					// TODO write_offset = -NUM_SCREEN_COLS + 1 to show first
					// column and so on
					scroll_offset = write_offset;
					write_offset  = NUM_SCREEN_COLS;
					// view_offset   = NUM_SCREEN_COLS; // XXX leave it be?
					break;

				case 0x1b: // esc
					// get more chars
					break;

				case '\b':
				case 0x7f:
					if (write_offset >= FONT_WIDTH)
						write_offset -= FONT_WIDTH;
					// and erase?
					break;

				// toggle scrolling (and delay)
				case 0x12: // ^R
					toggle_scrolling();
					break;
				}
			}
		}

		// if (c && text_write - text_buffer < TEXT_BUFFER_SIZE - 1) {
		// 	*text_write++ = c;
		// 	*text_write = 0;
		// }

		if (PINF & PIN_BUTTON) {
			// toggle_scrolling();
			measure = 1;
			_delay_ms(60);

		}

		// TODO use timer
		if (scrolling) {
			view_offset++;
			// message size + buffer width
			// if (view_offset >= NUM_SCREEN_COLS + write_offset)
			if (view_offset >= scroll_offset)
				view_offset = 0;

			if (scrolling == 1)
				_delay_ms(20);
			else if (scrolling == 2)
				_delay_ms(7);
		}
	}
}
