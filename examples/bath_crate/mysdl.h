#pragma once

//#include "defs.h"
#include <SDL\SDL.h>



#define DT_SECS   0.01666f
#define DT_MS     (DT_SECS*1000.)

typedef struct MySDL_KeyVal_s{
  SDL_bool down;
  SDL_bool press;
}MySDL_KeyVal;


#define MYSDL_HAT_U  0
#define MYSDL_HAT_D  1
#define MYSDL_HAT_R  2
#define MYSDL_HAT_L  3

typedef struct MySDL_Hat_s{
  MySDL_KeyVal keys[ 4 ];
}MySDL_Hat;


typedef struct MySDL_Mouse_s{
  MySDL_KeyVal buttons[ 8 ];
  Sint32        x, y;
}MySDL_Mouse;


typedef struct MySDL_Keyboard_s{
  Uint32       numkeys;
  MySDL_KeyVal keys[ 512 ];
}MySDL_Keyboard;


#define MYSDL_MAXJOYS         8
#define MYSDL_MAXJOYAXES      16
#define MYSDL_MAXJOYHATS      4
#define MYSDL_MAXJOYBUTTONS   32

typedef struct MySDL_Joystick_s{
  float          axes   [ MYSDL_MAXJOYAXES    ]; //up to 16 axes
  MySDL_KeyVal   buttons[ MYSDL_MAXJOYBUTTONS ]; //up to 32 buttons
  MySDL_Hat      hats   [ MYSDL_MAXJOYHATS    ]; //up to 4 hats

}MySDL_Joystick;

#define  MYSDL_TIME_FORMAT_MS   0
#define  MYSDL_TIME_FORMAT_SECS 1

extern const MySDL_Keyboard *MySDL_keyboard();
extern const MySDL_Mouse    *MySDL_mouse();
extern const MySDL_Joystick *MySDL_joysticks( Uint32 no );

#define MySDL_keyDown(k)  ( MySDL_keyboard()->keys[ (k) & 511 ].down  )
#define MySDL_KeyPress(k) ( MySDL_keyboard()->keys[ (k) & 511 ].press )

#define MySDL_mouseButtonDown( b )  ( MySDL_mouse()->buttons[ (b) & 7 ].down )
#define MySDL_mouseButtonPress( b ) ( MySDL_mouse()->buttons[ (b) & 7 ].press )

extern float  MySDL_time( Uint32 time_fmt );
extern Uint32 MySDL_ticks();

extern SDL_bool  MySDL_gl_init( const char *title, Uint32 w, Uint32 h, SDL_bool center, SDL_bool full, SDL_bool joy, SDL_bool sound );
extern SDL_bool  MySDL_soft_init( const char *title, Uint32 w, Uint32 h, SDL_bool center, SDL_bool full, SDL_bool joy, SDL_bool sound );

extern void  MySDL_pump();
extern SDL_bool  MySDL_focused( SDL_bool input, SDL_bool mouse );

extern SDL_bool  MySDL_running();
extern SDL_bool  MySDL_resumed();
extern SDL_bool  MySDL_paused();
extern void  MySDL_unpause();

extern SDL_bool  MySDL_init();
extern void  MySDL_step();
extern void  MySDL_draw();
extern void  MySDL_term();
extern void  MySDL_run ();

