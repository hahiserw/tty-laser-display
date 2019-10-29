#ifndef TINYFONT_SPRITE_H
#define TINYFONT_SPRITE_H

// https://github.com/yinkou/Arduboy-TinyFont

#include <avr/io.h>
#include <avr/pgmspace.h>

#define FONT_WIDTH 5
#define FONT_HEIGHT 5

const unsigned char PROGMEM TINYFONT_SPRITE[] =
//*
{
  // FRAME 00
  0x00, 0xb0, 0x00, 0x00,
  0xa1, 0x70, 0xe1, 0x50,
  0x96, 0x4f, 0x26, 0x90,
  0x0f, 0x1d, 0x07, 0x0c,
  0x00, 0x96, 0x69, 0x00,
  0x4a, 0xe4, 0x4a, 0x00,
  0x48, 0x44, 0x40, 0x00,
  0x80, 0x68, 0x10, 0x00,
  0x0f, 0x99, 0xfb, 0x8f,
  0x99, 0xbd, 0xbb, 0xf9,
  0x77, 0xd4, 0xd4, 0xdf,
  0x1f, 0x1a, 0x1a, 0xfe,
  0x7f, 0x5d, 0x5d, 0xff,
  0x80, 0x5a, 0x00, 0x00,
  0xa0, 0xa4, 0xaa, 0x00,
  0x10, 0xba, 0x34, 0x00,
  0xff, 0x59, 0x53, 0xf3,
  0xff, 0x9b, 0x9b, 0x9e,
  0xff, 0xb9, 0xb9, 0x96,
  0xff, 0x95, 0x95, 0xd1,
  0x9f, 0xf4, 0x94, 0x0f,
  0xfc, 0x29, 0x5f, 0x91,
  0xff, 0x18, 0x38, 0xf8,
  0xff, 0x92, 0x94, 0xff,
  0xff, 0x95, 0xd5, 0xf7,
  0x7f, 0xd5, 0xdd, 0xd7,
  0xf1, 0x8f, 0x81, 0xf1,
  0xf7, 0x88, 0xc8, 0xf7,
  0x79, 0xc6, 0x46, 0x79,
  0x09, 0xfd, 0x9b, 0x09,
  0x01, 0x96, 0xf8, 0x00,
  0x82, 0x81, 0x82, 0x80,
  0x50, 0x71, 0x62, 0x00,
  0x77, 0x56, 0x56, 0x00,
  0x76, 0x76, 0x37, 0x00,
  0xa2, 0xb7, 0x73, 0x01,
  0x07, 0x72, 0x06, 0x00,
  0x78, 0x27, 0x50, 0x00,
  0x73, 0x34, 0x74, 0x00,
  0x77, 0x51, 0x76, 0x00,
  0x7f, 0x55, 0xf7, 0x00,
  0x47, 0x71, 0x10, 0x00,
  0x32, 0x47, 0x72, 0x00,
  0x73, 0x64, 0x73, 0x00,
  0x15, 0xa2, 0x75, 0x00,
  0x61, 0x67, 0x94, 0x00,
  0x00, 0x9f, 0x60, 0x60,
  0xf4, 0xf2, 0xf6, 0xf2,
};

/*/
{
// TODO swap nibbles: 0xa1, 0xb2, 0xc3, 0xd4, => 0xab, 0xcd, 0x12, 0x34,
  // FRAME 00
  0x0b, 0x00, 0x00, 0x00,
  0xa7, 0xe5, 0x10, 0x10,
  0x94, 0x29, 0x6f, 0x60,
  0x01, 0x00, 0xfd, 0x7c,
  0x09, 0x60, 0x06, 0x90,
  0x4e, 0x40, 0xa4, 0xa0,
  0x44, 0x40, 0x84, 0x00,
  0x86, 0x10, 0x08, 0x00,
  0x09, 0xf8, 0xf9, 0xbf,
  0x9b, 0xbf, 0x9d, 0xb9,
  0x7d, 0xdd, 0x74, 0x4f,
  0x11, 0x1f, 0xfa, 0xae,
  0x75, 0x5f, 0xfd, 0xdf,
  0x85, 0x00, 0x0a, 0x00,
  0xaa, 0xa0, 0x04, 0xa0,
  0x1b, 0x30, 0x0a, 0x40,
  0xf5, 0x5f, 0xf9, 0x33,
  0xf9, 0x99, 0xfb, 0xbe,
  0xfb, 0xb9, 0xf9, 0x96,
  0xf9, 0x9d, 0xf5, 0x51,
  0x9f, 0x90, 0xf4, 0x4f,
  0xf2, 0x59, 0xc9, 0xf1,
  0xf1, 0x3f, 0xf8, 0x88,
  0xf9, 0x9f, 0xf2, 0x4f,
  0xf9, 0xdf, 0xf5, 0x57,
  0x7d, 0xdd, 0xf5, 0xd7,
  0xf8, 0x8f, 0x1f, 0x11,
  0xf8, 0xcf, 0x78, 0x87,
  0x7c, 0x47, 0x96, 0x69,
  0x0f, 0x90, 0x9d, 0xb9,
  0x09, 0xf0, 0x16, 0x80,
  0x88, 0x88, 0x21, 0x20,
  0x57, 0x60, 0x01, 0x20,
  0x75, 0x50, 0x76, 0x60,
  0x77, 0x30, 0x66, 0x70,
  0xab, 0x70, 0x27, 0x31,
  0x07, 0x00, 0x72, 0x60,
  0x72, 0x50, 0x87, 0x00,
  0x73, 0x70, 0x34, 0x40,
  0x75, 0x70, 0x71, 0x60,
  0x75, 0xf0, 0xf5, 0x70,
  0x47, 0x10, 0x71, 0x00,
  0x34, 0x70, 0x27, 0x20,
  0x76, 0x70, 0x34, 0x30,
  0x1a, 0x70, 0x52, 0x50,
  0x66, 0x90, 0x17, 0x40,
  0x09, 0x66, 0x0f, 0x00,
  0xff, 0xff, 0x42, 0x62,
};
//*/

#endif
