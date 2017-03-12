/**
 * @file        sometris.ino
 * @brief       Tetris-like game
 * @author      (C) Peter Ivanov, 2013, 2014, 2015, 2017
 * 
 * Created:     2013-12-23 11:29:32
 * Last modify: 2017-03-11 14:51:29 ivanovp {Time-stamp}
 * Licence:     GPL
 */
#define circuit Arcade_MegaWing

#include "common.h"
#include "game_gfx.h"
#include "game_common.h"
#include <Timer.h>

#define ZPU_VER             2

#define VGABASE             IO_SLOT(9)
#define CHARRAMBASE         IO_SLOT(10)

#define VGAPTR              REGISTER(VGABASE,0);
#define CHARRAMPTR          REGISTER(CHARRAMBASE,0);

#define USE_JOYSTICK_A      1
#define USE_JOYSTICK_B      1
#define DEBUG_JOYSTICK      0
#define DEBUG_STATEMACHINE  0
#define DEBUG_TICK          0
#define ENABLE_COLOR_TEST   0

/* Left, right, up, down and trigger names refer to joystick A/B */
uint8_t prevLeft, prevRight, prevUp, prevDown, prevTrigger;
uint8_t actLeft, actRight, actUp, actDown, actTrigger;
bool leftPressed, rightPressed, upPressed, downPressed, triggerPressed;
bool left, right, up, down, trigger;
volatile uint32_t timer_cntr = 0;
volatile uint32_t tick_cntr = 0;
uint32_t gameTimer = 0;
main_state_machine_t main_state_machine; /* Game state machine. @see handleMainStateMachine */


/* Random related */
uint8_t      key_delta[KEY_DELTA_SIZE];
uint16_t     key_delta_widx = 0;
uint16_t     key_delta_ridx = 0;

const volatile unsigned int* video_memory = &VGAPTR;
const volatile unsigned int* chargen_memory = &CHARRAMPTR;

void init ();

#if ZPU_VER == 2
bool timer (void*)
{
  tick_cntr++;
  if (timer_cntr)
  {
    timer_cntr--;
  }
  
  return true;
}
#elif ZPU_VER == 1
void _zpu_interrupt ()
{
  static uint8_t tick_prescaler = 10;

  if ( TMR0CTL & (1 << TCTLIF))
  {
    /* Interrupt comes from timer 0. */

    tick_prescaler--;
    if (tick_prescaler == 0)
    {
      tick_cntr++;
      tick_prescaler = 10;
      if (timer_cntr)
      {
        timer_cntr--;
      }
    }
 
    /* Clear the interrupt flag on timer register */
    TMR0CTL &= ~ (1 << TCTLIF );
  }
}
#else
#error Unknown ZPU version!
#endif

/**
 * Waste time.
 *
 * @param[in] ms Milliseconds to delay.
 */ 
void delay_ms (uint16_t ms)
{
    timer_cntr = ms;
    while (timer_cntr)
    {
    }
}

void setup()
{
  Serial.begin(9600);

  // Buttons on Arcade MegaWing
  pinMode (WING_AH3, INPUT);
  pinMode (WING_BH2, INPUT);
  pinMode (WING_BH0, INPUT);
  pinMode (WING_BH3, INPUT);
  pinMode (WING_BH1, INPUT);

#if USE_JOYSTICK_A == 1
  // Atari Joystick on DSUB9 connector
  pinMode (WING_CH4, OUTPUT);    // GND
  digitalWrite (WING_CH4, LOW);  // set low
  pinMode (WING_CH0, INPUT);
  pinMode (WING_CH2, INPUT);
  pinMode (WING_CH3, INPUT);
  pinMode (WING_CH5, INPUT);
  pinMode (WING_CH1, INPUT);
#endif
  
#if USE_JOYSTICK_B == 1
  // Atari Joystick on DSUB9 connector
  pinMode (WING_AL0, OUTPUT);    // GND
  digitalWrite (WING_AL0, LOW);  // set low
  pinMode (WING_BH4, INPUT);
  pinMode (WING_BH6, INPUT);
  pinMode (WING_BH7, INPUT);
  pinMode (WING_AL1, INPUT);
  pinMode (WING_BH5, INPUT);
#endif
  
  clear();

#if ZPU_VER == 2
  Timers.begin();
  int r = Timers.periodicHz(TICKS_PER_SEC, timer, 0, 1);
  if (r < 0) 
  {
      Serial.println("Fatal error!");
  }
#elif ZPU_VER == 1
  // Clear timer counter.
  TMR0CNT = 0;
 
  // Set up timer , no prescaler.
  TMR0CMP = (CLK_FREQ / TIMER_FREQEUNCY_HZ) - 1;
  TMR0CTL = (1 << TCTLENA)| (1 << TCTLCCM)| (1 << TCTLDIR)| (1 << TCTLIEN);
 
  // Enable timer 0 interrupt on mask.
  INTRMASK = (1 << INTRLINE_TIMER0);
 
  // Globally enable interrupts.
  INTRCTL = (1 << 0);
#else
#endif
  init ();
}  

/**
 * Button handler.
 */
void check_joystick()
{  
#if DEBUG_JOYSTICK == 1
  char s[6] = "-----";
#endif
  // Internal joystick
  actUp = digitalRead (WING_AH3);       // reset button
  actDown = digitalRead (WING_BH2);
  actLeft = digitalRead (WING_BH0);
  actRight = digitalRead (WING_BH3);
  actTrigger = digitalRead (WING_BH1);   // up button

#if USE_JOYSTICK_A == 1
  // Joystick A
  actUp |= digitalRead (WING_CH0) == HIGH ? LOW : HIGH;
  actDown |= digitalRead (WING_CH2) == HIGH ? LOW : HIGH;
  actLeft |= digitalRead (WING_CH3) == HIGH ? LOW : HIGH;
  actRight |= digitalRead (WING_CH5) == HIGH ? LOW : HIGH;
  actTrigger |= digitalRead (WING_CH1) == HIGH ? LOW : HIGH;
#endif

#if USE_JOYSTICK_B == 1
  // Joystick B
  actUp |= digitalRead (WING_BH4) == HIGH ? LOW : HIGH;
  actDown |= digitalRead (WING_BH6) == HIGH ? LOW : HIGH;
  actLeft |= digitalRead (WING_BH7) == HIGH ? LOW : HIGH;
  actRight |= digitalRead (WING_AL1) == HIGH ? LOW : HIGH;
  actTrigger |= digitalRead (WING_BH5) == HIGH ? LOW : HIGH;
#endif

  upPressed = false;
  downPressed = false;
  leftPressed = false;
  rightPressed = false;
  triggerPressed = false;
  up = (actUp == HIGH);
  down = (actDown == HIGH);
  left = (actLeft == HIGH);
  right = (actRight == HIGH);
  trigger = (actTrigger == HIGH);
  if (actUp == HIGH)
  {
    if (prevUp == LOW)
    {
      upPressed = true;
    }
#if DEBUG_JOYSTICK == 1
    s[0] = 'U';
#endif
  }
  if (actDown == HIGH)
  {
    if (prevDown == LOW)
    {
      downPressed = true;
    }
#if DEBUG_JOYSTICK == 1
    s[1] = 'D';
#endif
  }
  if (actLeft == HIGH)
  {
    if (prevLeft == LOW)
    {
      leftPressed = true;
    }
#if DEBUG_JOYSTICK == 1
    s[2] = 'L';
#endif
  }
  if (actRight == HIGH)
  {
    if (prevRight == LOW)
    {
      rightPressed = true;
    }
#if DEBUG_JOYSTICK == 1
    s[3] = 'R';
#endif
  }
  if (actTrigger == HIGH)
  {
    if (prevTrigger == LOW)
    {
      triggerPressed = true;
    }
#if DEBUG_JOYSTICK == 1
    s[4] = 'T';
#endif
  }
#if DEBUG_JOYSTICK == 1
  printtext (MAP_SIZE_X_PX + INFO_SPACE_X_PX, FONT_SIZE_Y_PX * 11, s, 0xff, 0x3);
#endif
  prevUp = actUp;
  prevDown = actDown;
  prevLeft = actLeft;
  prevRight = actRight;
  prevTrigger = actTrigger;
}

/**
 * Initialize game.
 */
void init (void)
{
    uint16_t i;

    srand (TMR0CNT);

    /* Fill random buffer with the library's values.
     * (Before generating real random values.)
     */
    for (i = 0; i < KEY_DELTA_SIZE; i++)
    {
        key_delta[i] = rand();
    }
}

/**
 * Time between user's key press is a real random number, so
 * we collect all.
 */
void collectRandomNumbers (void)
{
    uint32_t key_time;
    static uint32_t prev_key_time = 0;

    /* Get time between key presses to get better random numbers */
    if (upPressed || downPressed || leftPressed || rightPressed || triggerPressed)
    {
        key_time = tick_cntr;
        key_delta[key_delta_widx & KEY_DELTA_IDX_MASK] = key_time - prev_key_time;
        key_delta_widx++;
        key_delta_widx &= KEY_DELTA_IDX_MASK;
        prev_key_time = key_time;
    }
}

/**
 * Handle button presses and move figure according to that.
 */
void handleMovement (void)
{
    static uint8_t divider = 0; /* To slow down movement of figure */

    divider++;
    if (divider == 8)
    {
        divider = 0;
        if (right)
        {
            /* Right */
            if (canMoveFigureRight ())
            {
                game.figure_x++;
            }
        }
        else if (left)
        {
            /* Left */
            if (canMoveFigureLeft ())
            {
                game.figure_x--;
            }
        }
        if (down)
        {
            /* Down */
            if (canMoveFigureDown ())
            {
                game.figure_y++;
            }
        }
    }
    if (triggerPressed)
    {
        uint8_t new_x = 0, new_y = 0;
        /* Rotate */
        if (canRotateFigure (&new_x, &new_y))
        {
            rotateFigure (new_x, new_y);
        }
    }
#ifndef TEST_MOVEMENT
    if (tick_cntr >= gameTimer)
    {
        /* Automatic fall */
        int16_t ticks;

        if (canMoveFigureDown ())
        {
            game.figure_y++;
        }
        else
        {
            copyFigureToMap ();
            collapseMap ();
            generateFigure ();
        }

        /* Delay time = 0.8 sec - level * 0.1 sec */
        ticks = TICKS_PER_SEC * 8 / 10 - game.level * TICKS_PER_SEC / 10;
        if (ticks < TICKS_PER_SEC / 10) /* less than 0.1 sec */
        {
            ticks = TICKS_PER_SEC / 10;
        }
        gameTimer = tick_cntr + ticks;
    }
#endif
}

/**
 * Check inputs and change state machine if it is necessary.
 * @return TRUE: if user plays game again.
 */
bool_t handleMainStateMachine (void)
{
    bool_t replay = FALSE;
#if DEBUG_STATEMACHINE == 1
    static uint16_t sm_cntr = 0;

    printHexWord(0, 110, sm_cntr, 0xf0, 0x00);
    sm_cntr++;
#endif
    switch (main_state_machine)
    {
        case STATE_difficulty_selection:
#if ENABLE_COLOR_TEST == 0
            printtext (0, 0 * FONT_SIZE_Y_PX, "Sometris v1.2.1", INFO_COLOR, INFO_BG_COLOR);
            printtext (0, 1 * FONT_SIZE_Y_PX, "Copyright (C)", INFO_COLOR, INFO_BG_COLOR);
            printtext (0, 2 * FONT_SIZE_Y_PX, " Peter Ivanov,", INFO_COLOR, INFO_BG_COLOR);
            printtext (0, 3 * FONT_SIZE_Y_PX, "   2014-2015", INFO_COLOR, INFO_BG_COLOR);
            printtext (0, 4 * FONT_SIZE_Y_PX, "License: GPL", INFO_COLOR, INFO_BG_COLOR);

            if (downPressed && (game.block_types > MIN_BLOCK_TYPES))
            {
                game.block_types--;
            }
            if (upPressed && (game.block_types < MAX_BLOCK_TYPES))
            {
                game.block_types++;
            }
            printtext(8, 7 * FONT_SIZE_Y_PX, "Difficulty:", INFO_COLOR, INFO_BG_COLOR);
            printHexByte(8 + 12 * FONT_SIZE_X_PX, 7 * FONT_SIZE_Y_PX, game.block_types, INFO_COLOR, INFO_BG_COLOR);
#else
            { 
                static uint16_t color = 0x7F;
                static uint8_t prescaler = 0;
                if (downPressed || (actDown && !prescaler))
                {
                    color--;
                }
                if (upPressed || (actUp && !prescaler))
                {
                    color++;
                }
                printtext(8, 60, "Color:", 0xf0, 0x00);
                printHexWord(8 + 12 * FONT_SIZE_X_PX, 60, color, 0xf0, 0x00);
                printtext(0, 0, "COLOR TEST", color, 0x00);
                printtext(FONT_SIZE_X_PX * 0,  FONT_SIZE_Y_PX * 1, "White", COLOR_WHITE, 0x00);
                printtext(FONT_SIZE_X_PX * 6,  FONT_SIZE_Y_PX * 1, "Black", COLOR_BLACK, COLOR_WHITE);
                printtext(FONT_SIZE_X_PX * 12,  FONT_SIZE_Y_PX * 1, "Yellow", COLOR_YELLOW, 0x00);
                printtext(FONT_SIZE_X_PX * 0,  FONT_SIZE_Y_PX * 2, "Red", COLOR_RED, 0x00);
                printtext(FONT_SIZE_X_PX * 4,  FONT_SIZE_Y_PX * 2, "Green", COLOR_GREEN, 0x00);
                printtext(FONT_SIZE_X_PX * 10, FONT_SIZE_Y_PX * 2, "Blue", COLOR_BLUE, 0x00);
                printtext(FONT_SIZE_X_PX * 0,  FONT_SIZE_Y_PX * 3, "Magenta", COLOR_MAGENTA, 0x00);
                printtext(FONT_SIZE_X_PX * 8,  FONT_SIZE_Y_PX * 3, "Cyan", COLOR_CYAN, 0x00);
                
                prescaler++;
                if (prescaler == 80)
                {
                    prescaler = 0;
                }
            }
#endif
            if (triggerPressed || leftPressed || rightPressed)
            {
                clear();
                main_state_machine = STATE_running;
            }
            break;
        case STATE_running:
            handleMovement ();
            if (upPressed)
            {
                main_state_machine = STATE_paused;
            }
            if (isGameOver ())
            {
                main_state_machine = STATE_game_over;
            }
            else
            {
                drawGameScreen ();
            }
            break;
        case STATE_paused:
            if (triggerPressed || leftPressed || rightPressed || upPressed || downPressed)
            {
                main_state_machine = STATE_running;
            }
            drawGameScreen ();
            break;
        case STATE_game_over:
            if (triggerPressed || leftPressed || rightPressed || upPressed || downPressed)
            {
                /* Restart game */
                replay = TRUE;
            }
            drawGameScreen ();
            break;
        default:
        case STATE_undefined:
            /* This should not happen */
            break;
    }

    return replay;
}

/**
 * The user generates random numbers by pressing buttons: we store the time
 * difference between keystrokes.
 * rand() is not good enough for this game.
 * @return A random number.
 */
uint8_t myrand (void)
{
    uint8_t rnd;

    rnd = key_delta[key_delta_ridx];
    key_delta_ridx++;
    key_delta_ridx &= KEY_DELTA_IDX_MASK;
    return rnd;
}

/**
 * Play game.
 */
void loop()
{
    bool_t   do_replay   = FALSE;

replay:
    clear();
    main_state_machine = STATE_difficulty_selection;
    game.score = 0;
    game.level = 1;
    game.figure_counter = 0;
#ifndef TEST_MAP
    initMap ();
#endif
    generateFigure ();

    while (1)
    {
#if DEBUG_TICK != 0
        printHexWord(96, 110, tick_cntr >> 16, 0xf0, 0x00);
        printHexWord(128, 110, tick_cntr, 0xf0, 0x00);
#endif
        collectRandomNumbers ();

        do_replay = handleMainStateMachine ();
        if (do_replay)
        {
            goto replay; /* Shh! Bad thing! */
        }

        delay(5);
        check_joystick();
    }
}

// vim:sw=4:et:
