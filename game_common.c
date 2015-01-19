/**
 * @file        game_common.c
 * @brief       Tetris-like game's common functions
 * @author      (C) Peter Ivanov, 2013, 2014
 * 
 * Created:     2013-12-23 11:29:32
 * Last modify: 2014-12-29 09:46:46 ivanovp {Time-stamp}
 * Licence:     GPL
 */
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
#include "game_common.h"
#include "game_gfx.h"

game_t game =
{
    .version = 1,
    .map =
#ifdef TEST_MAP
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 2, 1, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 3, 0, 0, 0, 0, 2, 0, 0 },
        { 0, 0, 0, 3, 0, 0, 0, 0, 2, 0, 0 },
        { 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0 },
        { 0, 1, 1, 1, 0, 0, 0, 3, 1, 0, 0 },
        //{ 0, 1, 2, 2, 0, 0, 0, 3, 1, 0, 0 }
    },
#else
    { { 0 } },
#endif
    .figure = { 0 },
    .figure_is_vertical = FALSE,
    .figure_x = 0,
    .figure_y = 0,
    .figure_counter = 0,
    .score = 0, /* Earned points */
    .block_types = (MIN_BLOCK_TYPES + MAX_BLOCK_TYPES) / 2, /* Game difficulty */
    .level = 1
};

/**
 * Clear map.
 */
void initMap (void)
{
    uint8_t x, y;

    for (x = 0; x < MAP_SIZE_X; x++)
    {
        for (y = 0; y < MAP_SIZE_Y; y++)
        {
            MAPW(x, y) = 0;
        }
    }
}

/**
 * Check whether figure can move right (X positive)
 */
bool_t canMoveFigureRight (void)
{
    bool_t canMove = FALSE;
    uint8_t i;

    if (game.figure_is_vertical)
    {
        /* VERTICAL, Fuggoleges */
        if (game.figure_x < MAP_SIZE_X - 1)
        {
            canMove = TRUE;
            for (i = 0; i < FIGURE_SIZE; i++)
            {
                if (MAP_IS_NOT_EMPTY(game.figure_x + 1, game.figure_y + i))
                {
                    canMove = FALSE;
                    break;
                }
            }
        }
    }
    else
    {
        if (game.figure_x < MAP_SIZE_X - FIGURE_SIZE)
        {
            /* HORIZONTAL, Vizszintes */
            if (MAP_IS_EMPTY(game.figure_x + FIGURE_SIZE, game.figure_y))
            {
                canMove = TRUE;
            }
        }
    }

    return canMove;
}

/**
 * Check whether figure can move left (X negative)
 */
bool_t canMoveFigureLeft (void)
{
    bool_t canMove = FALSE;
    uint8_t i;

    if (game.figure_x > 0)
    {
        if (game.figure_is_vertical)
        {
            /* VERTICAL, Fuggoleges */
            canMove = TRUE;
            for (i = 0; i < FIGURE_SIZE; i++)
            {
                if (MAP_IS_NOT_EMPTY(game.figure_x - 1, game.figure_y + i))
                {
                    canMove = FALSE;
                    break;
                }
            }
        }
        else
        {
            /* HORIZONTAL, Vizszintes */
            if (MAP_IS_EMPTY(game.figure_x - 1, game.figure_y))
            {
                canMove = TRUE;
            }
        }
    }

    return canMove;
}

/**
 * Check whether figure can move down (Y positive)
 */
bool_t canMoveFigureDown (void)
{
    bool_t canMove = FALSE;
    uint8_t i;

    if (game.figure_is_vertical)
    {
        /* VERTICAL, Fuggoleges */
        if (game.figure_y < MAP_SIZE_Y - FIGURE_SIZE)
        {
            if (MAP_IS_EMPTY(game.figure_x, game.figure_y + FIGURE_SIZE))
            {
                canMove = TRUE;
            }
        }
    }
    else
    {
        /* HORIZONTAL, Vizszintes */
        if (game.figure_y < MAP_SIZE_Y - 1)
        {
            canMove = TRUE;
            for (i = 0; i < FIGURE_SIZE; i++)
            {
                if (MAP_IS_NOT_EMPTY(game.figure_x + i, game.figure_y + 1))
                {
                    canMove = FALSE;
                    break;
                }
            }
        }
    }

    return canMove;
}

/**
 * Check if there is enough space to rotate the figure.
 * @param[out] new_x Figure's new X coordinate.
 * @param[out] new_y Figure's new Y coordinate.
 *
 * @return TRUE: if figure can be rotated.
 */
bool_t canRotateFigure (uint8_t* new_x, uint8_t* new_y)
{
    bool_t canRotate = FALSE;
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t i;

    if (game.figure_is_vertical)
    {
        /* VERTICAL, Fuggoleges */
        /* .#.
         * .#.
         * .#.
         */
        if ((game.figure_x >= FIGURE_SIZE / 2) && (game.figure_x < MAP_SIZE_X - FIGURE_SIZE / 2)
                && (game.figure_y < MAP_SIZE_Y))
        {

            canRotate = TRUE;
            for (i = 0; i < FIGURE_SIZE; i++)
            {
                if (MAP_IS_NOT_EMPTY(game.figure_x - 1, game.figure_y + i)
                        || MAP_IS_NOT_EMPTY(game.figure_x + 1, game.figure_y + i))
                {
                    canRotate = FALSE;
                    break;
                }
            }
            if (canRotate)
            {
                x = game.figure_x - FIGURE_SIZE / 2;
                y = game.figure_y + FIGURE_SIZE / 2;
            }
        }
    }
    else
    {
        /* HORIZONTAL, Vizszintes */
        /* ...
         * ###
         * ...
         */
        if ((game.figure_y >= FIGURE_SIZE / 2) && (game.figure_y < MAP_SIZE_Y - FIGURE_SIZE / 2)
                && (game.figure_x < MAP_SIZE_X))
        {

            canRotate = TRUE;
            for (i = 0; i < FIGURE_SIZE; i++)
            {
                if (MAP_IS_NOT_EMPTY(game.figure_x + i, game.figure_y - 1)
                        || MAP_IS_NOT_EMPTY(game.figure_x + i, game.figure_y + 1))
                {
                    canRotate = FALSE;
                    break;
                }
            }
            if (canRotate)
            {
                x = game.figure_x + FIGURE_SIZE / 2;
                y = game.figure_y - FIGURE_SIZE / 2;
            }
        }
    }

    if (canRotate && new_x && new_y)
    {
        *new_x = x;
        *new_y = y;
    }

    return canRotate;
}

void rotateFigure (uint8_t new_x, uint8_t new_y)
{
    if (!game.figure_is_vertical)
    {
#if FIGURE_SIZE != 3
#error Reversing figure should be adapted to different figure size!
#endif
        uint8_t tmp;
        tmp = game.figure[0];
        game.figure[0] = game.figure[2];
        game.figure[2] = tmp;
    }
    game.figure_is_vertical = !game.figure_is_vertical;
    game.figure_x = new_x;
    game.figure_y = new_y;

}

/**
 * Create random figure and put to top of map.
 */
void generateFigure (void)
{
    uint8_t i;

    game.figure_x = MAP_SIZE_X / 2;
    game.figure_y = 0;
    game.figure_is_vertical = TRUE;
    for (i = 0; i < FIGURE_SIZE; i++)
    {
        /* Generate random block, except type 0 (empty)! */
        game.figure[i] = (RAND() % game.block_types) + 1;
    }

    game.figure_counter++;

    if ((game.figure_counter % 250) == 0 && (game.level < 6))
    {
        game.level++;
    }
}

/**
 * Finalize figure's position: copy blocks to map.
 */
void copyFigureToMap (void)
{
    uint8_t i;

    if (game.figure_is_vertical)
    {
        /* VERTICAL, Fuggoleges */
        for (i = 0; i < FIGURE_SIZE; i++)
        {
            MAPW(game.figure_x, game.figure_y + i) = game.figure[i];
        }
    }
    else
    {
        /* HORIZONTAL, Vizszintes */
        for (i = 0; i < FIGURE_SIZE; i++)
        {
            MAPW(game.figure_x + i, game.figure_y) = game.figure[i];
        }
    }
}

/**
 * Increase score.
 */
void incScore (uint8_t same_cntr, uint8_t factor)
{
    if (same_cntr >= SAME_BLOCK_NUM)
    {
        game.score += (1u << (same_cntr - SAME_BLOCK_NUM)) * factor * game.level;
    }
}

/**
 * Search same blocks in a row/column/diagonal and remove them.
 */
void collapseMap (void)
{
    uint8_t x, y;
    const uint8_t start_x = 0, start_y = 0;
    const uint8_t end_x = MAP_SIZE_X, end_y = MAP_SIZE_Y;
    uint8_t x1, y1;
    uint8_t x2, y2;
    uint8_t same_cntr = 0;
    uint8_t same_start_x = 0;
    uint8_t same_start_y = 0;
    bool_t collapsed;
    uint8_t round = 0;

    do
    {
        /* VERTICAL, Fuggoleges */
        collapsed = FALSE;
        for (x = start_x; x < end_x; x++)
        {
            same_cntr = 0;
            for (y = start_y; y < end_y; y++)
            {
                if (MAP_IS_NOT_EMPTY(x, y))
                {
                    if ((y < end_y - 1) && (MAP(x, y) == MAP(x, y + 1)))
                    {
                        if (!same_cntr)
                        {
                            same_start_y = y;
                            same_cntr = 2;
                        }
                        else
                        {
                            same_cntr++;
                        }
                    }
                    else
                    {
                        if (same_cntr >= SAME_BLOCK_NUM)
                        {
                            round++;
                            incScore (same_cntr, SAME_BLOCK_VERT_FACTOR * round);
                            for (y2 = same_start_y; y2 <= y; y2++)
                            {
#ifdef DEBUG
                                printf("VERTICAL shift down x:%i y:%i\r\n", x, y2);
#endif
                                MAP_SELECT(x, y2);
                            }
#ifdef DEBUG
                            drawMap(FALSE);
#endif
                            collapsed = TRUE;
                        }
                        same_cntr = 0;
                    }
                }
                else
                {
                    same_cntr = 0;
                }
            }
        }
        /* HORIZONTAL, Vizszintes */
        for (y = start_y; y < end_y; y++)
        {
            same_cntr = 0;
            for (x = start_x; x < end_x; x++)
            {
                if (MAP_IS_NOT_EMPTY(x, y))
                {
                    if ((x < end_x - 1) && (MAP(x, y) == MAP(x + 1, y)))
                    {
                        if (!same_cntr)
                        {
                            same_start_x = x;
                            same_cntr = 2;
                        }
                        else
                        {
                            same_cntr++;
                        }
                    }
                    else
                    {
                        if (same_cntr >= SAME_BLOCK_NUM)
                        {
                            round++;
                            incScore (same_cntr, SAME_BLOCK_HORIZ_FACTOR * round);
                            for (x2 = same_start_x; x2 <= x; x2++)
                            {
#ifdef DEBUG
                                printf("HORIZONTAL shift down x:%i y:%i\r\n", x2, y);
#endif
                                MAP_SELECT(x2, y);
                            }
#ifdef DEBUG
                            drawMap(FALSE);
#endif
                            collapsed = TRUE;
                        }
                        same_cntr = 0;
                    }
                }
                else
                {
                    same_cntr = 0;
                }
            }
        }
        /********* DIAGONAL *********/
        for (y = start_y; y < end_y; y++)
        {
            same_cntr = 0;
            /* DIAGONAL RIGHT on Y, Atlos jobbra lejt Y */
            for (x = start_x, y1 = y; x < end_x && y1 < end_y; x++, y1++)
            {
                if (MAP_IS_NOT_EMPTY(x, y1))
                {
                    if ((x < end_x - 1) && (y1 < end_y - 1)
                            && MAP(x, y1) == MAP(x + 1, y1 + 1))
                    {
                        if (!same_cntr)
                        {
                            same_start_x = x;
                            same_start_y = y1;
                            same_cntr = 2;
                        }
                        else
                        {
                            same_cntr++;
                        }
                    }
                    else
                    {
                        if (same_cntr >= SAME_BLOCK_NUM)
                        {
                            round++;
                            incScore (same_cntr, SAME_BLOCK_DIAG_FACTOR * round);
                            for (x2 = same_start_x, y2 = same_start_y; x2 <= x && y2 <= y1; x2++, y2++)
                            {
#ifdef DEBUG
                                printf("DIAGONAL RIGHT Y shift down x:%i y:%i\r\n", x2, y2);
#endif
                                MAP_SELECT(x2, y2);
                            }
#ifdef DEBUG
                            drawMap(FALSE);
#endif
                            collapsed = TRUE;
                        }
                        same_cntr = 0;
                    }
                }
                else
                {
                    same_cntr = 0;
                }
            }
            /* DIAGONAL LEFT on Y, Atlos balra lejt Y */
            for (x = start_x, y1 = y; x < end_x && y1 > 0; x++, y1--)
            {
                if (MAP_IS_NOT_EMPTY(x, y1))
                {
                    if ((x < end_x - 1) && MAP(x, y1) == MAP(x + 1, y1 - 1))
                    {
                        if (!same_cntr)
                        {
                            same_start_x = x;
                            same_start_y = y1;
                            same_cntr = 2;
                        }
                        else
                        {
                            same_cntr++;
                        }
                    }
                    else
                    {
                        if (same_cntr >= SAME_BLOCK_NUM)
                        {
                            round++;
                            incScore (same_cntr, SAME_BLOCK_DIAG_FACTOR * round);
                            for (x2 = same_start_x, y2 = same_start_y; x2 <= x && y2 >= y1; x2++, y2--)
                            {
#ifdef DEBUG
                                printf("DIAGONAL LEFT Y shift down x:%i y:%i\r\n", x2, y2);
#endif
                                MAP_SELECT(x2, y2);
                            }
#ifdef DEBUG
                            drawMap(FALSE);
#endif
                            collapsed = TRUE;
                        }
                        same_cntr = 0;
                    }
                }
                else
                {
                    same_cntr = 0;
                }
            }
        }
        for (x = start_x; x < end_x; x++)
        {
            same_cntr = 0;
            /* DIAGONAL RIGHT on X, Atlos jobbra lejt X */
            for (x1 = x, y = start_y; x1 < end_x && y < end_y; x1++, y++)
            {
                if (MAP_IS_NOT_EMPTY(x1, y))
                {
                    if ((x1 < end_x - 1) && (y < end_y - 1)
                            && MAP(x1, y) == MAP(x1 + 1, y + 1))
                    {
                        if (!same_cntr)
                        {
                            same_start_x = x1;
                            same_start_y = y;
                            same_cntr = 2;
                        }
                        else
                        {
                            same_cntr++;
                        }
                    }
                    else
                    {
                        if (same_cntr >= SAME_BLOCK_NUM)
                        {
                            round++;
                            incScore (same_cntr, SAME_BLOCK_DIAG_FACTOR * round);
                            for (x2 = same_start_x, y2 = same_start_y; x2 <= x1 && y2 <= y; x2++, y2++)
                            {
#ifdef DEBUG
                                printf("DIAGONAL RIGHT X shift down x:%i y:%i\r\n", x2, y2);
#endif
                                MAP_SELECT(x2, y2);
                            }
#ifdef DEBUG
                            drawMap(FALSE);
#endif
                            collapsed = TRUE;
                        }
                        same_cntr = 0;
                    }
                }
                else
                {
                    same_cntr = 0;
                }
            }
            /* DIAGONAL LEFT on X, Atlos balra lejt X */
            for (x1 = x, y = end_y - 1; x1 < end_x && y > 0; x1++, y--)
            {
                if (MAP_IS_NOT_EMPTY(x1, y))
                {
                    if ((x1 < end_x - 1) && MAP(x1, y) == MAP(x1 + 1, y - 1))
                    {
                        if (!same_cntr)
                        {
                            same_start_x = x1;
                            same_start_y = y;
                            same_cntr = 2;
                        }
                        else
                        {
                            same_cntr++;
                        }
                    }
                    else
                    {
                        if (same_cntr >= SAME_BLOCK_NUM)
                        {
                            round++;
                            incScore (same_cntr, SAME_BLOCK_DIAG_FACTOR * round);
                            for (x2 = same_start_x, y2 = same_start_y; x2 <= x1 && y2 >= y; x2++, y2--)
                            {
#ifdef DEBUG
                                printf("DIAGONAL LEFT X shift down x:%i y:%i\r\n", x2, y2);
#endif
                                MAP_SELECT(x2, y2);
                            }
#ifdef DEBUG
                            drawMap(FALSE);
#endif
                            collapsed = TRUE;
                        }
                        same_cntr = 0;
                    }
                }
                else
                {
                    same_cntr = 0;
                }
            }
        }
        if (collapsed)
        {
            blinkMap (2);
            /* Delete same blocks */
            for (x = start_x; x < end_x; x++)
            {
                for (y = start_y; y < end_y; y++)
                {
                    if (MAP_IS_SELECTED(x, y))
                    {
                        shiftDownColumn (x, y);
                    }
                }
            }
        }
    } while (collapsed);
}

/**
 * Check if there is space for a new figure at top of map.
 *
 * @return TRUE: if there is no room, so game is over.
 */
bool_t isGameOver (void)
{
    uint8_t i;
    bool_t over = FALSE;

    for (i = 0; i < FIGURE_SIZE; i++)
    {
        if (MAP_IS_NOT_EMPTY(MAP_SIZE_X / 2, i))
        {
            over = TRUE;
        }
    }
    return over;
}

