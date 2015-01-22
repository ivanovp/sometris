/**
 * @file        game_gfx.c
 * @brief       Tetris-like game's common functions, some functions were borrowed from Bricks example
 * @author      (C) Peter Ivanov, 2013, 2014 and Bricks example authors
 * 
 * Created:     2013-12-23 11:29:32
 * Last modify: 2014-12-30 10:02:14 ivanovp {Time-stamp}
 * Licence:     GPL
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"
#include "game_common.h"
#include "game_gfx.h"

unsigned char save[8*8] = {0};
const char hexNumbers[17] = "0123456789ABCDEF";

#define SPR1     COLOR_BLUE     // blue square
#define SPR2     COLOR_RED      // red diamond
#define SPR3     COLOR_CYAN     // cyan hour glass
#define SPR4     COLOR_YELLOW   // yellow square
#define SPR5     COLOR_GREEN    // green circle
#define SPR6     COLOR_WHITE    // white plus sign

const uint8_t sprites[MAX_BLOCK_TYPES + 1][BLOCK_SIZE_Y_PX][BLOCK_SIZE_X_PX] =
{
  // no sprite
  {
   { 0, 0, 0, 0, 0, 0 }, 
   { 0, 0, 0, 0, 0, 0 }, 
   { 0, 0, 0, 0, 0, 0 }, 
   { 0, 0, 0, 0, 0, 0 }, 
   { 0, 0, 0, 0, 0, 0 }, 
   { 0, 0, 0, 0, 0, 0 }, 
  },
  // blue square (filled)
  {
   { SPR1, SPR1, SPR1, SPR1, SPR1, 0x00 }, 
   { SPR1, SPR1, SPR1, SPR1, SPR1, 0x00 }, 
   { SPR1, SPR1, SPR1, SPR1, SPR1, 0x00 }, 
   { SPR1, SPR1, SPR1, SPR1, SPR1, 0x00 }, 
   { SPR1, SPR1, SPR1, SPR1, SPR1, 0x00 }, 
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
  },            
  // red diamond (filled)
  {                        
   { 0x00, 0x00, SPR2, 0x00, 0x00, 0x00 }, 
   { 0x00, SPR2, SPR2, SPR2, 0x00, 0x00 }, 
   { SPR2, SPR2, SPR2, SPR2, SPR2, 0x00 }, 
   { 0x00, SPR2, SPR2, SPR2, 0x00, 0x00 }, 
   { 0x00, 0x00, SPR2, 0x00, 0x00, 0x00 }, 
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
  },
  // cyan hour glass (filled)
  {
   { SPR3, SPR3, SPR3, SPR3, SPR3, 0x00 }, 
   { 0x00, SPR3, SPR3, SPR3, 0x00, 0x00 }, 
   { 0x00, 0x00, SPR3, 0x00, 0x00, 0x00 }, 
   { 0x00, SPR3, SPR3, SPR3, 0x00, 0x00 }, 
   { SPR3, SPR3, SPR3, SPR3, SPR3, 0x00 }, 
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
  },
  // yellow square (non-filled)
  {
   { SPR4, SPR4, SPR4, SPR4, SPR4, 0x00 }, 
   { SPR4, 0x00, 0x00, 0x00, SPR4, 0x00 }, 
   { SPR4, 0x00, 0x00, 0x00, SPR4, 0x00 }, 
   { SPR4, 0x00, 0x00, 0x00, SPR4, 0x00 }, 
   { SPR4, SPR4, SPR4, SPR4, SPR4, 0x00 }, 
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
  },
  // green circle (filled)  
  {
   { 0x00, SPR5, SPR5, SPR5, 0x00, 0x00 }, 
   { SPR5, SPR5, SPR5, SPR5, SPR5, 0x00 }, 
   { SPR5, SPR5, SPR5, SPR5, SPR5, 0x00 }, 
   { SPR5, SPR5, SPR5, SPR5, SPR5, 0x00 }, 
   { 0x00, SPR5, SPR5, SPR5, 0x00, 0x00 }, 
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
  },                       
  // white plus sign
  {
   { 0x00, 0x00, SPR6, 0x00, 0x00, 0x00 }, 
   { 0x00, 0x00, SPR6, 0x00, 0x00, 0x00 }, 
   { SPR6, SPR6, SPR6, SPR6, SPR6, 0x00 }, 
   { 0x00, 0x00, SPR6, 0x00, 0x00, 0x00 }, 
   { 0x00, 0x00, SPR6, 0x00, 0x00, 0x00 }, 
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
  },                       
};

// From Bricks example
void sprite_draw(uint8_t x, uint8_t y, uint8_t shape)
{
    int i = 0, j = 0;
    volatile unsigned int *p;
    volatile unsigned int *vmem = video_memory;

    for (j = 0; j < BLOCK_SIZE_Y_PX; j++) 
    {
       p = &vmem[(int)x + (y + j) * DISPLAY_SIZE_X];
       for (i = 0; i < BLOCK_SIZE_X_PX; i++) 
        {
            *p++ = sprites[shape][j][i];
        }
    }
}

// From Bricks example
void drawHLine (uint8_t x, uint8_t y, uint8_t length, unsigned int color)
{
    uint8_t i;
    volatile unsigned int *vmem = video_memory;
    vmem += y * DISPLAY_SIZE_X + x;

    while (length--)
    {
        *vmem = color;
        vmem++;
    }
}

// From Bricks example
void drawVLine (uint8_t x, uint8_t y, uint8_t length, unsigned int color)
{
    uint8_t i;
    volatile unsigned int *vmem = video_memory;
    vmem += y * DISPLAY_SIZE_X + x;

    while (length--)
    {
        *vmem = color;
        vmem += DISPLAY_SIZE_X;
    }
}

// From Bricks example
void printchar(unsigned int x, unsigned int y, unsigned int c, int fgcolor, int bgcolor)
{
    volatile unsigned int *vmem = video_memory;
    volatile unsigned int *vchar = chargen_memory;
    int i,j;
    unsigned int v;

    vchar += c << 3;

    //vmem += x + (y*DISPLAY_SIZE_X);
    for (i = 0; i < 8;i++) 
    {
        v = *vchar;
        for (j = 0; j < 8; j++) 
        {

            int rx,ry;
            rx = x + j;
            ry = (y + i);

            /* Check bounds */
            if (rx >= 0 && rx < DISPLAY_SIZE_X && ry >= 0 &&  ry < DISPLAY_SIZE_Y) 
            {
                rx = rx + ry * DISPLAY_SIZE_X;


                if (v & 0x80) 
                {
                    vmem[rx] = fgcolor;
                } 
                else 
                {
                    vmem[rx] = bgcolor;
                }
                //vmem++;
                v<<=1;
            }
        }
        vchar++;
    }
}

// From Bricks example
void printchar_trans(unsigned int x, unsigned int y, unsigned char c, int fgcolor)
{
    volatile unsigned int *vmem = video_memory;
    volatile unsigned int *vchar = chargen_memory;
    int i,j,v;

    vchar += c*8;

    vmem += x + (y * DISPLAY_SIZE_X);
    for (i = 8; i != 0; i--)
    {
        v = *vchar++;
        for (j = 8; j != 0;j--)
        {
            if (v & 0x80)
            {
                *vmem = fgcolor;
            } 
            vmem++;
            v<<=1;
        }
        vmem += DISPLAY_SIZE_X - 8;
    }
}

// From Bricks example
void save8x8(unsigned x, unsigned y)
{
    volatile unsigned int *vmem = video_memory;
    unsigned char *sptr=&save[0];
    unsigned w,h;
    vmem += x + (y*DISPLAY_SIZE_X);

    for (h=0;h<8;h++) {
        for (w=0;w<8;w++) {
            *sptr = *vmem & 0xff;
            //*vmem = 0;//*sptr;
            vmem++;
            sptr++;
        }
        vmem+=DISPLAY_SIZE_X-8;
    }
}

// From Bricks example
void restore8x8(unsigned x, unsigned y)
{
    volatile unsigned int *vmem = video_memory;
    unsigned char *sptr=&save[0];
    unsigned w,h;
    vmem += x + (y*DISPLAY_SIZE_X);

    for (h=0;h<8;h++)
    {
        for (w=0;w<8;w++)
        {
            *vmem++ = *sptr++;
        }
        vmem+=DISPLAY_SIZE_X-8;
    }
}


// From Bricks example
void clear_at(unsigned x, unsigned y, unsigned width, unsigned height, unsigned color)
{
    volatile unsigned int *vmem = video_memory;
    unsigned w,h;
    vmem += x + (y*DISPLAY_SIZE_X);

    for (h=0;h<height;h++)
    {
        for (w=0;w<width;w++)
        {
            *vmem++=color;
        }
        vmem+=DISPLAY_SIZE_X-width;
    }
}


// From Bricks example
void clear()
{
    volatile unsigned int *vmem = video_memory;
    unsigned i;
    for (i = DISPLAY_SIZE_X * DISPLAY_SIZE_Y; i !=0; i--) 
    {
        *vmem++ = 0;
    }
}

// From Bricks example
void printtext(unsigned x, unsigned y, const char *text, unsigned fg,unsigned bg)
{
    while (*text) 
    {
        printchar(x,y,*text,fg,bg);
        text++;
        x+=8;
    }
}

#if 0
// From Bricks example
void draw_shifting_text()
{
    const char *text = &scrtext[xoffset>>3];

    if (!*text) {
        xoffset=0;
        text = &scrtext[0];
    }

    int i,x,y=30;

    x = 0 - (xoffset & 0x7);
    for (i=0; i<=DISPLAY_SIZE_X/8; i++) {
        printchar(x,y,*text,0xff,0x3);
        if (*text)
            text++;
        x+=8;
    }

    xoffset++;
}
#endif

void printHexByte(unsigned x, unsigned y, uint8_t number, unsigned fg, unsigned bg)
{
    char n;
    n = hexNumbers[(number >> 4) & 0xF];
    printchar(x, y, n, fg, bg);
    x += FONT_SIZE_X_PX;
    n = hexNumbers[number & 0xF];
    printchar(x, y, n, fg, bg);  
}

void printHexWord(unsigned x, unsigned y, uint16_t number, unsigned fg, unsigned bg)
{
    uint8_t digits = 4;
    char n;

    x += (digits - 1) * FONT_SIZE_X_PX;
    
    while (digits--)
    {
        n = hexNumbers[number & 0xF];
        printchar(x, y, n, fg, bg);
        number >>= 4;
        x -= FONT_SIZE_X_PX;
    }  
}

void printDecByte(unsigned x, unsigned y, uint8_t number, unsigned fg, unsigned bg)
{
    uint8_t digits = 3;
    char n;

    x += (digits - 1) * FONT_SIZE_X_PX;

    while (digits--)
    {
        n = hexNumbers[number % 10];
        printchar(x, y, n, fg, bg);
        number /= 10;
        x -= FONT_SIZE_X_PX;
    }
}

void printDecWord(unsigned x, unsigned y, uint16_t number, unsigned fg, unsigned bg)
{
    uint8_t digits = 5;
    char n;

    x += (digits - 1) * FONT_SIZE_X_PX;

    while (digits--)
    {
        n = hexNumbers[number % 10];
        printchar(x, y, n, fg, bg);
        number /= 10;
        x -= FONT_SIZE_X_PX;
    }
}

void drawBlock (uint8_t x, uint8_t y, uint8_t shape)
{
  sprite_draw (x * BLOCK_SIZE_X_PX + MAP_SPACE_X_PX, y * BLOCK_SIZE_Y_PX, shape);
}

void printCommon (void)
{
    drawVLine (0, 0, MAP_SIZE_Y_PX, INFO_COLOR);
    drawVLine (MAP_SIZE_X_PX + 1, 0, MAP_SIZE_Y_PX, INFO_COLOR);
    drawHLine (0, MAP_SIZE_Y_PX, MAP_SIZE_X_PX + 2, INFO_COLOR);
    printtext    (MAP_SIZE_X_PX + INFO_SPACE_X_PX, 0 * FONT_SIZE_Y_PX, "Score:", INFO_COLOR, INFO_BG_COLOR);
    printDecWord (MAP_SIZE_X_PX + INFO_SPACE_X_PX, 1 * FONT_SIZE_Y_PX, game.score, INFO_COLOR, INFO_BG_COLOR);
    printtext    (MAP_SIZE_X_PX + INFO_SPACE_X_PX, 3 * FONT_SIZE_Y_PX, "Level:", INFO_COLOR, INFO_BG_COLOR);
    printDecByte (MAP_SIZE_X_PX + INFO_SPACE_X_PX, 4 * FONT_SIZE_Y_PX, game.level, INFO_COLOR, INFO_BG_COLOR);
    printtext    (MAP_SIZE_X_PX + INFO_SPACE_X_PX, 6 * FONT_SIZE_Y_PX, "Difficulty:", INFO_COLOR, INFO_BG_COLOR);
    printDecByte (MAP_SIZE_X_PX + INFO_SPACE_X_PX, 7 * FONT_SIZE_Y_PX, game.block_types, INFO_COLOR, INFO_BG_COLOR);
    printtext    (MAP_SIZE_X_PX + INFO_SPACE_X_PX, 9 * FONT_SIZE_Y_PX, "Counter:", INFO_COLOR, INFO_BG_COLOR);
    printDecByte (MAP_SIZE_X_PX + INFO_SPACE_X_PX, 10 * FONT_SIZE_Y_PX, game.figure_counter, INFO_COLOR, INFO_BG_COLOR);
}

void drawGameScreen (void)
{
    if (GAME_IS_PAUSED())
    {
        printtext (MAP_SIZE_X_PX / 2 - 3 * FONT_SIZE_X_PX, MAP_SIZE_Y_PX / 2 - FONT_SIZE_Y_PX / 2, "PAUSED", COLOR_RED, COLOR_BLACK);
    }
    else if (GAME_IS_OVER())
    {
        printtext (MAP_SIZE_X_PX / 2 - 4 * FONT_SIZE_X_PX, MAP_SIZE_Y_PX / 2 - FONT_SIZE_Y_PX / 2, "GAME OVER", COLOR_RED, COLOR_BLACK);
    }
    else
    {
        printCommon ();
        drawMap (TRUE);
        //drawFigure ();
    }
}

/**
 * @brief blinkMap
 * FIXME A non-blocking method should be implemented!
 *
 * @param blinkNum Number of blinks.
 */
void blinkMap (uint8_t blinkNum)
{
    uint8_t i;
    uint8_t x, y;

    /* Blink same blocks */
    for (i = 0; i < blinkNum * 2; i++)
    {
        printCommon ();

        for (x = 0; x < MAP_SIZE_X; x++)
        {
            for (y = 0; y < MAP_SIZE_Y; y++)
            {
                if (!MAP_IS_SELECTED(x, y) || (i & 1))
                {
                    drawBlock (x, y, MAP(x,y));
                }
                else
                {
                    // clear block
                    drawBlock (x, y, 0);
                }
            }
        }
        delay_ms (200);
    }
}

/**
 * @brief drawMap
 * Draw game's map.
 */
void drawMap (bool_t drawFigure)
{
    uint8_t x, y;
    bool_t figureBlock = FALSE;

    for (x = 0; x < MAP_SIZE_X; x++)
    {
        for (y = 0; y < MAP_SIZE_Y; y++)
        {
            figureBlock = FALSE;
            if (drawFigure)
            {
                if (game.figure_is_vertical)
                {
                    // Fuggoleges
                    if (x == game.figure_x && y >= game.figure_y && y < (game.figure_y + FIGURE_SIZE))
                    {
                        drawBlock (x, y, game.figure[y - game.figure_y]);
                        figureBlock = TRUE;
                    }
                }
                else
                {
                    // Vizszintes
                    if (y == game.figure_y && x >= game.figure_x && x < (game.figure_x + FIGURE_SIZE))
                    {
                        drawBlock (x, y, game.figure[x - game.figure_x]);
                        figureBlock = TRUE;
                    }
                }
            }
            if (!figureBlock)
            {
                drawBlock (x, y, MAP(x,y));
            }
        }
    }
}

/**
 * @brief drawFigure
 * Draw the falling figure into map.
 */
void drawFigure (void)
{
    uint8_t i;

    if (game.figure_is_vertical)
    {
        /* VERTICAL, Fuggoleges */
        for (i = 0; i < FIGURE_SIZE; i++)
        {
            drawBlock (game.figure_x, game.figure_y + i, game.figure[i]);
        }
    }
    else
    {
        /* HORIZONTAL, Vizszintes */
        for (i = 0; i < FIGURE_SIZE; i++)
        {
            drawBlock (game.figure_x + i, game.figure_y, game.figure[i]);
        }
    }
}

/**
 * @brief clearFigure
 * Clear the figure from the map.
 * This is only useful if redrawing of screen is costly.
 */
void clearFigure (void)
{
    uint8_t i;

    if (game.figure_is_vertical)
    {
        /* VERTICAL, Fuggoleges */
        for (i = 0; i < FIGURE_SIZE; i++)
        {
            drawBlock (game.figure_x, game.figure_y + i, 0);
        }
    }
    else
    {
        /* HORIZONTAL, Vizszintes */
        for (i = 0; i < FIGURE_SIZE; i++)
        {
            drawBlock (game.figure_x + i, game.figure_y, 0);
        }
    }
}

/**
 * @brief shiftDownColumn
 * Remove one block from map and move down the remaining blocks of column.
 *
 * @param x0 Coordinate X of block that shall be removed.
 * @param y0 Coordinate Y of block that shall be removed.
 */
void shiftDownColumn (uint8_t x0, uint8_t y0)
{
    uint8_t y;

    for (y = y0; y > 0; y--)
    {
        uint8_t block = MAP(x0, y - 1);
        MAPW(x0, y) = block;
    }
    MAPW(x0, 0) = 0;
}
