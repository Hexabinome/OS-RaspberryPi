#ifndef _FB_CURSOR
#define _FB_CURSOR

#include <stdint.h>

void fb_prompt();
void fb_print_text(const char* const msg);
void fb_print_char(const char c);
void fb_backspace();
void fb_delete();
void fb_move_cursor_left();
void fb_move_cursor_right();
void fb_clear();
void fb_set_zoom(const uint8_t new_zoom);

#endif // _FB_CURSOR
