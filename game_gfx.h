/**
 * @file        game_gfx.c
 * @brief       Header of tetris-like game's common functions
 * @author      (C) Peter Ivanov, 2013, 2014
 * 
 * Created:     2013-12-23 11:29:32
 * Last modify: 2014-12-30 08:41:22 ivanovp {Time-stamp}
 * Licence:     GPL
 */
#ifndef INCLUDE_GAME_GFX_H
#define INCLUDE_GAME_GFX_H

#include "common.h"

#define DISPLAY_SIZE_X          (800/5)    /* 160 */
#define DISPLAY_SIZE_Y          (600/5)    /* 120 */

#define FONT_SIZE_X_PX          8
#define FONT_SIZE_Y_PX          8

#define DEFAULT_BG_COLOR        0x00    /* Black */

#define MAP_SIZE_X_PX           (MAP_SIZE_X * BLOCK_SIZE_X_PX)
#define MAP_SIZE_Y_PX           (MAP_SIZE_Y * BLOCK_SIZE_Y_PX)

#define BLOCK_SIZE_X_PX         6
#define BLOCK_SIZE_Y_PX         6

#define COLOR_WHITE             0xFF
#define COLOR_YELLOW            0xFC
#define COLOR_RED               0xE5
#define COLOR_GREEN             0x7C
#define COLOR_BLUE              0x03
#define COLOR_MAGENTA           0xEF
#define COLOR_CYAN              0x5F
#define COLOR_BLACK             0x00

#define MAP_SPACE_X_PX          1
#define INFO_SPACE_X_PX         3
#define INFO_COLOR              COLOR_WHITE
#define INFO_BG_COLOR           COLOR_BLACK

#if MAP_SIZE_X_PX > DISPLAY_SIZE_X
#error MAP_SIZE_X_PX greater than width of screen!
#endif
#if MAP_SIZE_Y_PX > DISPLAY_SIZE_Y
#error MAP_SIZE_Y_PX greater than height of screen!
#endif

extern const volatile unsigned int* video_memory;
extern const volatile unsigned int* chargen_memory;

#ifdef __cplusplus
extern "C" {
#endif
void sprite_draw (uint8_t x, uint8_t y, uint8_t shape);
void printchar(unsigned int x, unsigned int y, unsigned int c, int fgcolor, int bgcolor);
void printchar_trans(unsigned int x, unsigned int y, unsigned char c, int fgcolor);
void save8x8(unsigned x, unsigned y);
void restore8x8(unsigned x, unsigned y);
void clear_at(unsigned x, unsigned y, unsigned width, unsigned height, unsigned color);
void clear();
void printtext(unsigned x, unsigned y, const char *text, unsigned fg,unsigned bg);
void printHexByte(unsigned x, unsigned y, uint8_t number, unsigned fg, unsigned bg);
void printHexWord(unsigned x, unsigned y, uint16_t number, unsigned fg, unsigned bg);
void printDecByte(unsigned x, unsigned y, uint8_t number, unsigned fg, unsigned bg);
void printDecWord(unsigned x, unsigned y, uint16_t number, unsigned fg, unsigned bg);
void drawBlock (uint8_t x, uint8_t y, uint8_t shape);
void printCommon (void);
void drawGameScreen (void);
void blinkMap (uint8_t blinkNum);
void drawMap (bool_t drawFigure);
void drawFigure (void);
void clearFigure (void);
void shiftDownColumn (uint8_t x0, uint8_t y0);
#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GAME_GFX_H */
