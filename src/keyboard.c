#include "keyboard.h"
#include "config.h"
#include "sem.h"
#include "fb_cursor.h"

extern struct sem_s cmd_buffer_sem;
extern struct sem_s shell_sem;
extern char* cmd_buffer;
uint32_t cmd_buffer_idx;
uint32_t cmd_buffer_size;

static void KeyboardUpdate();
static char KeyboardGetChar();

static void cmd_backspace()
{
	if (cmd_buffer_idx > 0) // Copy everything to the right
	{
		uint32_t i;
		for (i = cmd_buffer_size; i >= cmd_buffer_idx; --i)
		{
			cmd_buffer[i-1] = cmd_buffer[i];
		}
	}
	
	--cmd_buffer_idx;
	if (cmd_buffer_idx < 0)
		cmd_buffer_idx = 0;
	
	--cmd_buffer_size;
	if (cmd_buffer_size < 0)
		cmd_buffer_size = 0;
}

static void cmd_delete()
{
	if (cmd_buffer_idx < cmd_buffer_size)
	{
		uint32_t i;
		for (i = cmd_buffer_size; i > cmd_buffer_idx; --i)
		{
			cmd_buffer[i-1] = cmd_buffer[i];
		}
	}
	
	--cmd_buffer_idx;
	if (cmd_buffer_idx < 0)
		cmd_buffer_idx = 0;
	
	--cmd_buffer_size;
	if (cmd_buffer_size < 0)
		cmd_buffer_size = 0;
}

static void cmd_left()
{
	--cmd_buffer_idx;
	if (cmd_buffer_idx < 0)
		cmd_buffer_idx = 0;
}

static void cmd_right()
{
	++cmd_buffer_idx;
	if (cmd_buffer_idx > cmd_buffer_size)
		cmd_buffer_idx = cmd_buffer_size;
}


static void highlight_cursor()
{
	fb_print_curr_char('\0');
	// TODO add negative of current char
}

static void unhighlight_cursor()
{
	fb_print_curr_char(' ');
}

static void display_normal_cursor()
{
	char c = cmd_buffer[cmd_buffer_idx];
	if (c == '\0')
		c = ' ';

	fb_print_curr_char(c);
}

int keyboard_loop()
{	
	cmd_buffer_idx = 0;
	cmd_buffer_size = 0;

	sem_down(&cmd_buffer_sem); // begin when shell says okay
	
	char c;
	uint8_t i = 0;
	while (1)
	{
		if (((i++) & 1) == 0)
			highlight_cursor();
		else
			unhighlight_cursor();
			
		KeyboardUpdate();
		c = KeyboardGetChar();
		
		if (c != NULL)
		{
			display_normal_cursor();
			
			// Display
			if ((uint8_t) c == FB_BACKSPACE)
			{
				fb_backspace();
				cmd_backspace();
			}
			else if ((uint8_t) c == FB_DELETE)
			{
				fb_delete();
				cmd_delete();
			}
			else if ((uint8_t) c == FB_ARROW_LEFT)
			{
				fb_move_cursor_left();
				cmd_left();
			}
			else if ((uint8_t) c == FB_ARROW_RIGHT)
			{
				fb_move_cursor_right();
				cmd_right();
			}
			else
			{
				fb_print_char(c);
				// TODO : realloc size (but not possible because in kernel space)
				if (cmd_buffer_idx == cmd_buffer_size) // write at the end
				{
					cmd_buffer[cmd_buffer_idx++] = c;
					cmd_buffer[cmd_buffer_idx] = '\0';
				}
				else
				{
					uint32_t i;
					for (i = cmd_buffer_size; i >= cmd_buffer_idx; --i)
					{
						cmd_buffer[i+1] = cmd_buffer[i];
					}
					cmd_buffer[cmd_buffer_idx++] = c;
				}
				
				cmd_buffer_size++;
				i = 0;
			}

			// Send command
			if (c == '\n')
			{
				sem_up(&shell_sem); // unblock shell
				sem_down(&cmd_buffer_sem); // block command line
				cmd_buffer_idx = 0;
				cmd_buffer[cmd_buffer_idx] = '\0';
				cmd_buffer_size = 0;
			}
		}
	}
	
	return 0;
}

const char key_normal[104];
const char key_shift[104];

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

const char key_normal[104] = 
{
	0x0, 0x0, 0x0, 0x0, 'q', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
	',', 'n', 'o', 'p', 'a', 'r', 's', 't',
	'u', 'v', 'z', 'x', 'y', 'w', '&', 'e',
	'"', '\'', '(', '-', 'e', '_', 'c', 'a',
	'\n', 0x0, '\b', '\t', ' ', ')', '=', '^',
	'$', '\\', '*', 'm', '\'', '`', ';', ':',
	'!', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 19, 0x0, 0x0, 17,
	18, 0x0, 0x0, 0x0, '/', '*', '-', '+',
	'\n', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '0', ':', '<', 0x0, 0x0, '='
};

const char key_shift[104] =
{
	0x0, 0x0, 0x0, 0x0, 'Q', 'B', 'C', 'D',
	'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
	'?', 'N', 'O', 'P', 'A', 'R', 'S', 'T',
	'U', 'V', 'Z', 'X', 'Y', 'W', '1', '2',
	'3', '4', '5', '6', '7', '8', '9', '0',
	'\n', 0x0, '\b', '\t', ' ', 'o', '+', '{',
	'}', '|', '~', 'M', '@', 0x0, '.', '/',
	'?', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, '!', '*', '-', '+',
	'\n', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '0', '.', '>', 0x0, 0x0, '='
};
