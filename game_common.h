/**
 * @file        game_common.c
 * @brief       Header of tetris-like game's common functions
 * @author      (C) Peter Ivanov, 2013, 2014
 * 
 * Created:     2013-12-23 11:29:32
 * Last modify: 2014-12-30 09:49:24 ivanovp {Time-stamp}
 * Licence:     GPL
 */
#ifndef INCLUDE_GAME_H
#define INCLUDE_GAME_H

#include "common.h"

#define MAP_SIZE_X              12
#define MAP_SIZE_Y              18
#define MAP_IS_EMPTY(x,y)       (!MAP(x,y))         /* Cell is empty */
#define MAP_IS_NOT_EMPTY(x,y)   (MAP(x,y))          /* Cell is not empty */
#define MAP_IS_SELECTED(x,y)    (game.map[y][x] & 0x80)  /* Cell is selected for remove */
#define MAP_SELECT(x,y)         game.map[y][x] |= 0x80   /* Select cell for remove */
#define MAP(x,y)                (game.map[y][x] & 0x7F)  /* Read map */
#define MAPW(x,y)               game.map[y][x]           /* Write map */

#define MAX_BLOCK_TYPES         6   /* 0: no block, 1: diamond, 2: filled diamond, ... */
#define MIN_BLOCK_TYPES         3
#define RECORD_TYPES            (MAX_BLOCK_TYPES - MIN_BLOCK_TYPES + 1)
#define RECORD_TYPE(block_type) (block_type - MIN_BLOCK_TYPES)

#define SAME_BLOCK_NUM          3   /* Three same blocks will disappear */
#define SAME_BLOCK_VERT_FACTOR  1   /* Score multiplier */
#define SAME_BLOCK_HORIZ_FACTOR 1   /* Score multiplier */
#define SAME_BLOCK_DIAG_FACTOR  2   /* Score multiplier */

#define FIGURE_SIZE             3

#define RAND()                  myrand()
//#define RAND()                  rand() /* rand() is ineligible for this game! */

typedef struct
{
    uint8_t version;            /* Only for loading/saving game */
    uint8_t map[MAP_SIZE_Y][MAP_SIZE_X];
    uint8_t figure[FIGURE_SIZE];
    bool_t figure_is_vertical;
    uint8_t figure_x;
    uint8_t figure_y;
    uint32_t figure_counter;
    uint32_t score;
    uint8_t block_types;
    uint8_t level;
} game_t;

extern game_t game;

#ifdef __cplusplus
extern "C" {
#endif
void initMap (void);
bool_t canMoveFigureRight (void);
bool_t canMoveFigureLeft (void);
bool_t canMoveFigureDown (void);
bool_t canRotateFigure (uint8_t* new_x, uint8_t* new_y);
void rotateFigure (uint8_t new_x, uint8_t new_y);
void generateFigure (void);
void copyFigureToMap (void);
void shiftDownColumn (uint8_t x0, uint8_t y0);
void incScore (uint8_t same_cntr, uint8_t factor);
void collapseMap (void);
bool_t isGameOver (void);
#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GAME_H */
