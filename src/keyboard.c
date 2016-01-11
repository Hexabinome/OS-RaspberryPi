#include "keyboard.h"
#include "config.h"

const char key_normal[104] = 
{
	0x0, 0x0, 0x0, 0x0, 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
	'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y', 'z', '1', '2',
	'3', '4', '5', '6', '7', '8', '9', '0',
	'\n', 0x0, '\b', '\t', ' ', '-', '=', '[',
	']', '\\', '#', ';', '\'', '`', ',', '.',
	'/', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 19, 0x0, 0x0, 17,
	18, 0x0, 0x0, 0x0, '/', '*', '-', '+',
	'\n', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '0', '.', '\\', 0x0, 0x0, '='
};

const char key_shift[104] =
{
	0x0, 0x0, 0x0, 0x0, 'A', 'B', 'C', 'D',
	'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
	'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', '!', '"',
	0x0, '$', '%', '^', '&', '*', '(', ')',
	'\n', 0x0, '\b', '\t', ' ', '_', '+', '{',
	'}', '|', '~', ':', '@', 0x0, '<', '>',
	'?', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, '/', '*', '-', '+',
	'\n', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '0', '.', '|', 0x0, 0x0, '='
};

uint32_t* kb_address = NULL;

void KeyboardUpdate()
{
	if (kb_address == NULL)
	{
		UsbCheckForChange();
		uint32_t kb_count = KeyboardCount();
		if (kb_count == 0)
		{
			kb_address = NULL;
			return;
		}
		
		kb_address = KeyboardGetAddress(0);
		if (kb_address == NULL)
		{
			return;
		}
	}
	
	KeyboardGetKeyDown(kb_address, 0);
	KeyboardGetKeyDown(kb_address, 1);
	KeyboardGetKeyDown(kb_address, 2);
	KeyboardGetKeyDown(kb_address, 3);
	KeyboardGetKeyDown(kb_address, 4);
	KeyboardGetKeyDown(kb_address, 5);
	
	uint32_t return_code = KeyboardPoll(kb_address);
	if (return_code != 0)
	{
		kb_address = NULL;
		KeyboardUpdate();
	}
}

static const char* get_key_table(uint32_t modifiers)
{
	// If bit 1 or 5 is =1 => shift is held
	if (((modifiers >> 1) & 0x1) == 1
		|| ((modifiers >> 5) & 0x1) == 1)
	{
		return key_shift;
	}
	
	return key_normal;
}

char KeyboardGetChar()
{
	if (kb_address == NULL)
	{
		return 0;
	}
	
	uint32_t i;
	char c;
	for (i = 0; i < 6; ++i)
	{
		c = KeyboardGetKeyDown(kb_address, i);
		if (((uint32_t) c) > 103)
		{
			continue;
		}
		uint32_t modifiers = KeyboardGetModifiers(kb_address);
		const char* key_table = get_key_table(modifiers);
		char result = key_table[(uint32_t) c];
		if (result != 0)
		{
			return result;
		}
	}
	
	return 0;
}

void LogPrint(char* message, uint32_t length)
{
	// Careful, this code is executed too early for us
	// fb_address is still equal to 0, which means we erase the interruption vectors
	
	/*fb_print_text(">> ");
	fb_print_text(message);
	fb_print_char('\n');*/
}
