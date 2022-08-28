#pragma once

#include "defines.h"

enum buttons {
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_MIDDLE,

  BUTTON_MAX_BUTTONS
};

enum keys {
  /* The backspace key. */
  KEY_BACKSPACE = 0x08,
  /* The enter key. */
  KEY_ENTER = 0x0D,
  /* The tab key. */
  KEY_TAB = 0x09,
  /* The shift key. */
  KEY_SHIFT = 0x10,
  /* The Control/Ctrl key. */
  KEY_CONTROL = 0x11,

  /* The pause key. */
  KEY_PAUSE = 0x13,
  /* The Caps Lock key. */
  KEY_CAPITAL = 0x14,

  /* The Escape key. */
  KEY_ESCAPE = 0x1B,

  KEY_CONVERT = 0x1C,
  KEY_NONCONVERT = 0x1D,
  KEY_ACCEPT = 0x1E,
  KEY_MODECHANGE = 0x1F,

  /* The spacebar key. */
  KEY_SPACE = 0x20,
  KEY_PRIOR = 0x21,
  KEY_NEXT = 0x22,
  /* The end key. */
  KEY_END = 0x23,
  /* The home key. */
  KEY_HOME = 0x24,
  /* The left arrow key. */
  KEY_LEFT = 0x25,
  /* The up arrow key. */
  KEY_UP = 0x26,
  /* The right arrow key. */
  KEY_RIGHT = 0x27,
  /* The down arrow key. */
  KEY_DOWN = 0x28,
  KEY_SELECT = 0x29,
  KEY_PRINT = 0x2A,
  KEY_EXECUTE = 0x2B,
  KEY_SNAPSHOT = 0x2C,
  /* The insert key. */
  KEY_INSERT = 0x2D,
  /* The delete key. */
  KEY_DELETE = 0x2E,
  KEY_HELP = 0x2F,

  /* The 0 key */
  KEY_0 = 0x30,
  /* The 1 key */
  KEY_1 = 0x31,
  /* The 2 key */
  KEY_2 = 0x32,
  /* The 3 key */
  KEY_3 = 0x33,
  /* The 4 key */
  KEY_4 = 0x34,
  /* The 5 key */
  KEY_5 = 0x35,
  /* The 6 key */
  KEY_6 = 0x36,
  /* The 7 key */
  KEY_7 = 0x37,
  /* The 8 key */
  KEY_8 = 0x38,
  /* The 9 key */
  KEY_9 = 0x39,

  /* The A key. */
  KEY_A = 0x41,
  /* The B key. */
  KEY_B = 0x42,
  /* The C key. */
  KEY_C = 0x43,
  /* The D key. */
  KEY_D = 0x44,
  /* The E key. */
  KEY_E = 0x45,
  /* The F key. */
  KEY_F = 0x46,
  /* The G key. */
  KEY_G = 0x47,
  /* The H key. */
  KEY_H = 0x48,
  /* The I key. */
  KEY_I = 0x49,
  /* The J key. */
  KEY_J = 0x4A,
  /* The K key. */
  KEY_K = 0x4B,
  /* The L key. */
  KEY_L = 0x4C,
  /* The M key. */
  KEY_M = 0x4D,
  /* The N key. */
  KEY_N = 0x4E,
  /* The O key. */
  KEY_O = 0x4F,
  /* The P key. */
  KEY_P = 0x50,
  /* The Q key. */
  KEY_Q = 0x51,
  /* The R key. */
  KEY_R = 0x52,
  /* The S key. */
  KEY_S = 0x53,
  /* The T key. */
  KEY_T = 0x54,
  /* The U key. */
  KEY_U = 0x55,
  /* The V key. */
  KEY_V = 0x56,
  /* The W key. */
  KEY_W = 0x57,
  /* The X key. */
  KEY_X = 0x58,
  /* The Y key. */
  KEY_Y = 0x59,
  /* The Z key. */
  KEY_Z = 0x5A,

  /* The left Windows/Super key. */
  KEY_LWIN = 0x5B,
  /* The right Windows/Super key. */
  KEY_RWIN = 0x5C,
  KEY_APPS = 0x5D,

  /* The sleep key. */
  KEY_SLEEP = 0x5F,

  /* The numberpad 0 key. */
  KEY_NUMPAD0 = 0x60,
  /* The numberpad 1 key. */
  KEY_NUMPAD1 = 0x61,
  /* The numberpad 2 key. */
  KEY_NUMPAD2 = 0x62,
  /* The numberpad 3 key. */
  KEY_NUMPAD3 = 0x63,
  /* The numberpad 4 key. */
  KEY_NUMPAD4 = 0x64,
  /* The numberpad 5 key. */
  KEY_NUMPAD5 = 0x65,
  /* The numberpad 6 key. */
  KEY_NUMPAD6 = 0x66,
  /* The numberpad 7 key. */
  KEY_NUMPAD7 = 0x67,
  /* The numberpad 8 key. */
  KEY_NUMPAD8 = 0x68,
  /* The numberpad 9 key. */
  KEY_NUMPAD9 = 0x69,
  /* The numberpad multiply key. */
  KEY_MULTIPLY = 0x6A,
  /* The numberpad add key. */
  KEY_ADD = 0x6B,
  /* The numberpad separator key. */
  KEY_SEPARATOR = 0x6C,
  /* The numberpad subtract key. */
  KEY_SUBTRACT = 0x6D,
  /* The numberpad decimal key. */
  KEY_DECIMAL = 0x6E,
  /* The numberpad divide key. */
  KEY_DIVIDE = 0x6F,

  /* The F1 key. */
  KEY_F1 = 0x70,
  /* The F2 key. */
  KEY_F2 = 0x71,
  /* The F3 key. */
  KEY_F3 = 0x72,
  /* The F4 key. */
  KEY_F4 = 0x73,
  /* The F5 key. */
  KEY_F5 = 0x74,
  /* The F6 key. */
  KEY_F6 = 0x75,
  /* The F7 key. */
  KEY_F7 = 0x76,
  /* The F8 key. */
  KEY_F8 = 0x77,
  /* The F9 key. */
  KEY_F9 = 0x78,
  /* The F10 key. */
  KEY_F10 = 0x79,
  /* The F11 key. */
  KEY_F11 = 0x7A,
  /* The F12 key. */
  KEY_F12 = 0x7B,
  /* The F13 key. */
  KEY_F13 = 0x7C,
  /* The F14 key. */
  KEY_F14 = 0x7D,
  /* The F15 key. */
  KEY_F15 = 0x7E,
  /* The F16 key. */
  KEY_F16 = 0x7F,
  /* The F17 key. */
  KEY_F17 = 0x80,
  /* The F18 key. */
  KEY_F18 = 0x81,
  /* The F19 key. */
  KEY_F19 = 0x82,
  /* The F20 key. */
  KEY_F20 = 0x83,
  /* The F21 key. */
  KEY_F21 = 0x84,
  /* The F22 key. */
  KEY_F22 = 0x85,
  /* The F23 key. */
  KEY_F23 = 0x86,
  /* The F24 key. */
  KEY_F24 = 0x87,

  /* The number lock key. */
  KEY_NUMLOCK = 0x90,

  /* The scroll lock key. */
  KEY_SCROLL = 0x91,

  /* The numberpad equal key. */
  KEY_NUMPAD_EQUAL = 0x92,

  /* The left shift key. */
  KEY_LSHIFT = 0xA0,
  /* The right shift key. */
  KEY_RSHIFT = 0xA1,
  /* The left control key. */
  KEY_LCONTROL = 0xA2,
  /* The right control key. */
  KEY_RCONTROL = 0xA3,
  /* The left alt key. */
  KEY_LALT = 0xA4,
  /* The right alt key. */
  KEY_RALT = 0xA5,

  /* The semicolon key. */
  KEY_SEMICOLON = 0xBA,
  /* The plus key. */
  KEY_PLUS = 0xBB,
  /* The comma key. */
  KEY_COMMA = 0xBC,
  /* The minus key. */
  KEY_MINUS = 0xBD,
  /* The period key. */
  KEY_PERIOD = 0xBE,
  /* The slash key. */
  KEY_SLASH = 0xBF,

  /* The grave key. */
  KEY_GRAVE = 0xC0,

  KEYS_MAX_KEYS
};

void input_initialize();
void input_shutdown();
void input_update(f64 delta_time);

bool input_is_key_down(keys key);
bool input_is_key_up(keys key);
bool input_was_key_down(keys key);
bool input_was_key_up(keys key);

void input_process_key(keys key, bool pressed);

bool input_is_button_down(buttons button);
bool input_is_button_up(buttons button);
bool input_was_button_down(buttons button);
bool input_was_button_up(buttons button);

void input_get_mouse_position(i32 *x, i32 *y);
void input_get_previous_mouse_position(i32 *x, i32 *y);

void input_process_button(buttons button, bool pressed);
void input_process_mouse_move(i16 x, i16 y);
void input_process_mouse_wheel(i8 z_delta);
