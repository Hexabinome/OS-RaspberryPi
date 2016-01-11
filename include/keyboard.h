#ifndef KEYBOARD_H
#define KEYBOARD_H

#define FB_BACKSPACE 8
#define FB_DELETE 19
#define FB_ARROW_LEFT 18
#define FB_ARROW_RIGHT 17

void KeyboardUpdate();
char KeyboardGetChar();

// implemented in csud static linked lib
void UsbInitialise();


#endif // KEYBOARD_H
