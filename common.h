/**
 * @file        common.h
 * @brief       Common definitions
 * @author      Copyright (C) Peter Ivanov, 2011, 2012, 2013, 2014
 *
 * Created      2011-01-19 11:48:53
 * Last modify: 2014-12-30 09:56:10 ivanovp {Time-stamp}
 * Licence:     GPL
 */

#ifndef _INCLUDE_COMMON_H_
#define _INCLUDE_COMMON_H_

#define VGA_HSYNC           WC6
#define VGA_VSYNC           WC7
#define VGA_RED0            WC0
#define VGA_RED1            WC2
#define VGA_GREEN0          WC1
#define VGA_GREEN1          WC3
#define VGA_BLUE0           WC5
#define VGA_BLUE1           WC4

#define TIMER_FREQEUNCY_HZ  10000
#define TICKS_PER_SEC       (1000)

//#define TEST_MAP
//#define TEST_MOVEMENT
//#define TEST_RANDOM

#define VERSION_MAJOR    1
#define VERSION_MINOR    2
#define VERSION_REVISION 1

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

#ifndef FALSE
#define FALSE   (0)
#endif
#ifndef TRUE
#define TRUE    (1)
#endif

#define BIT0    0x01u
#define BIT1    0x02u
#define BIT2    0x04u
#define BIT3    0x08u
#define BIT4    0x10u
#define BIT5    0x20u
#define BIT6    0x40u
#define BIT7    0x80u

#define _BV(x)  (1u << (x))

#define GAME_IS_PAUSED()    (main_state_machine == STATE_paused)
#define GAME_IS_OVER()      (main_state_machine == STATE_game_over)

#define KEY_DELTA_SIZE      32  /* Must be power of 2! */
#define KEY_DELTA_IDX_MASK  (KEY_DELTA_SIZE - 1)

#define MAX_RECORD_NUM      10
#define PLAYER_NAME_LENGTH  9
#define MAX_PLAYERS         12

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned char bool_t;

typedef enum
{
    STATE_undefined,            /**< This shall not be used! */
    STATE_difficulty_selection, /**< Game difficulty should be selected first. */
    STATE_running,              /**< Game is played */
    STATE_paused,               /**< Game is paused. It can be played again. */
    STATE_game_over,            /**< Game is over, it can be played again. */
    STATE_size        /**< Not a real state. Only to count number of states. THIS SHOULD BE THE LAST ONE! */
} main_state_machine_t;

extern uint32_t     gameTimer;      /* Game timer (automatic shift down of figure) */
//extern uint32_t     gameTickRate;   /* 30 FPS */
extern bool_t       gameRunning;    /* TRUE: game is running, FALSE: game shall exit! */
extern main_state_machine_t main_state_machine; /* Game state machine. @see handleMainStateMachine */

/* Random related */
extern uint8_t      key_delta[KEY_DELTA_SIZE]; /**< Time difference between keystrokes. For random number generating. */
extern uint16_t     key_delta_widx; /**< Write index for key_delta */
extern uint16_t     key_delta_ridx; /**< Read index for key_delta */

#ifdef __cplusplus
extern "C" {
#endif
  uint8_t myrand (void);
  void delay_ms (uint16_t ms);
#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_COMMON_H_ */

