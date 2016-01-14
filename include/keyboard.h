#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define FB_BACKSPACE 8
#define FB_DELETE 19
#define FB_ARROW_LEFT 18
#define FB_ARROW_RIGHT 17

int keyboard_loop();

// implemented in csud static linked lib
void UsbInitialise();
void UsbCheckForChange();
uint32_t KeyboardCount();
uint32_t* KeyboardGetAddress(uint32_t idx);
char KeyboardGetKeyDown(uint32_t* addr, uint32_t key_nb);
uint32_t KeyboardPoll(uint32_t* addr);
uint32_t KeyboardGetModifiers(uint32_t* addr);


#endif // KEYBOARD_H
