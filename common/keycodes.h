

#include "platform.h"

#ifdef PLATFORM_WIN32
#define KEYCODE_0   '0'
#define KEYCODE_1   '1'
#define KEYCODE_2   '2'
#define KEYCODE_3   '3'
#define KEYCODE_4   '4'
#define KEYCODE_5   '5'
#define KEYCODE_6   '6'
#define KEYCODE_7   '7'
#define KEYCODE_8   '8'
#define KEYCODE_9   '9'
#define KEYCODE_A   'A'
#define KEYCODE_B   'B'
#define KEYCODE_C   'C'
#define KEYCODE_D   'D'
#define KEYCODE_E   'E'
#define KEYCODE_F   'F'
#define KEYCODE_G   'G'
#define KEYCODE_H   'H'
#define KEYCODE_I   'I'
#define KEYCODE_J   'J'
#define KEYCODE_K   'K'
#define KEYCODE_L   'L'
#define KEYCODE_M   'M'
#define KEYCODE_N   'N'
#define KEYCODE_O   'O'
#define KEYCODE_P   'P'
#define KEYCODE_Q   'Q'
#define KEYCODE_R   'R'
#define KEYCODE_S   'S'
#define KEYCODE_T   'T'
#define KEYCODE_U   'U'
#define KEYCODE_V   'V'
#define KEYCODE_W   'W'
#define KEYCODE_X   'X'
#define KEYCODE_Y   'Y'
#define KEYCODE_Z   'Z'
#define KEYCODE_BACKSLASH    	VK_OEM_102
/*
#define KEYCODE_CAPSLOCK        SDLK_CAPSLOCK
#define KEYCODE_COMMA           SDLK_COMMA
#define KEYCODE_COPY            SDLK_COPY
#define KEYCODE_TILDE           SDLK_BACKQUOTE
#define KEYCODE_ESCAPE          SDLK_ESCAPE
#define KEYCODE_DELETE          SDLK_DELETE
#define KEYCODE_PASTE           SDLK_PASTE
#define KEYCODE_PERIOD          SDLK_PERIOD
#define KEYCODE_MINUS           SDLK_MINUS
#define KEYCODE_RETURN          SDLK_RETURN
#define KEYCODE_UP              SDLK_UP
#define KEYCODE_LEFT            SDLK_LEFT
#define KEYCODE_RIGHT           SDLK_RIGHT
#define KEYCODE_DOWN            SDLK_DOWN
#define KEYCODE_SEMICOLON       SDLK_SEMICOLON
#define KEYCODE_SLASH           SDLK_SLASH
#define KEYCODE_RIGHTBRACKET    SDLK_RIGHTBRACKET
#define KEYCODE_LEFTBRACKET     SDLK_LEFTBRACKET
#define KEYCODE_EQUALS          SDLK_EQUALS
*/
#define KEYCODE_CONTROL			VK_CONTROL
#define KEYCODE_SHIFT			VK_SHIFT
#define FINISHED_KEYCODES	1
#endif

#if defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
#define KEYCODE_0   SDLK_0
#define KEYCODE_1   SDLK_1
#define KEYCODE_2   SDLK_2
#define KEYCODE_3   SDLK_3
#define KEYCODE_4   SDLK_4
#define KEYCODE_5   SDLK_5
#define KEYCODE_6   SDLK_6
#define KEYCODE_7   SDLK_7
#define KEYCODE_8   SDLK_8
#define KEYCODE_9   SDLK_9
#define KEYCODE_A   SDLK_a
#define KEYCODE_B   SDLK_b
#define KEYCODE_C   SDLK_c
#define KEYCODE_D   SDLK_d
#define KEYCODE_E   SDLK_e
#define KEYCODE_F   SDLK_f
#define KEYCODE_G   SDLK_g
#define KEYCODE_H   SDLK_h
#define KEYCODE_I   SDLK_i
#define KEYCODE_J   SDLK_j
#define KEYCODE_K   SDLK_k
#define KEYCODE_L   SDLK_l
#define KEYCODE_M   SDLK_m
#define KEYCODE_N   SDLK_n
#define KEYCODE_O   SDLK_o
#define KEYCODE_P   SDLK_p
#define KEYCODE_Q   SDLK_q
#define KEYCODE_R   SDLK_r
#define KEYCODE_S   SDLK_s
#define KEYCODE_T   SDLK_t
#define KEYCODE_U   SDLK_u
#define KEYCODE_V   SDLK_v
#define KEYCODE_W   SDLK_w
#define KEYCODE_X   SDLK_x
#define KEYCODE_Y   SDLK_y
#define KEYCODE_Z   SDLK_z
#define KEYCODE_BACKSLASH    	SDLK_BACKSLASH
#define KEYCODE_CAPSLOCK        SDLK_CAPSLOCK
#define KEYCODE_COMMA           SDLK_COMMA
#define KEYCODE_COPY            SDLK_COPY
#define KEYCODE_TILDE           SDLK_BACKQUOTE
#define KEYCODE_ESCAPE          SDLK_ESCAPE
#define KEYCODE_DELETE          SDLK_DELETE
#define KEYCODE_PASTE           SDLK_PASTE
#define KEYCODE_PERIOD          SDLK_PERIOD
#define KEYCODE_MINUS           SDLK_MINUS
#define KEYCODE_RETURN          SDLK_RETURN
#define KEYCODE_UP              SDLK_UP
#define KEYCODE_LEFT            SDLK_LEFT
#define KEYCODE_RIGHT           SDLK_RIGHT
#define KEYCODE_DOWN            SDLK_DOWN
#define KEYCODE_SEMICOLON       SDLK_SEMICOLON
#define KEYCODE_SLASH           SDLK_SLASH
#define KEYCODE_RIGHTBRACKET    SDLK_RIGHTBRACKET
#define KEYCODE_LEFTBRACKET     SDLK_LEFTBRACKET
#define KEYCODE_EQUALS          SDLK_EQUALS
#define KEYCODE_CONTROL			SDLK_CONTROL
#define KEYCODE_SHIFT			SDLK_SHIFT
#endif