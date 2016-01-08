#include "fb_cursor.h"

#include "fb.h"
#include "hw.h"

#include "util.h"

#define BITMAP_SIZE 8
#define DEFAULT_FONT_SIZE 8
#define OFFSET 48
extern const uint64_t font[128];

extern uint32_t fb_x, fb_y;

static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;
// Determines the last position, which is empty
static uint32_t end_x = 0;
static uint32_t end_y = 0;

static uint32_t prompt_x;
static uint32_t prompt_y;

static uint32_t zoom = 1;

// size of a character
static uint32_t size = DEFAULT_FONT_SIZE;
static uint32_t max_x = 79; // TODO sure always the case ? even if when real monitor
static char* prompt = "$ ";



static void draw_bitmap(const uint64_t bitmap);
static uint64_t get_bitmap_pixel_color(const uint64_t bitmap, const uint8_t x, const uint8_t y);
static uint32_t to_zoomed(const uint32_t coord);
static uint32_t to_rea(const uint32_t coord);
static uint32_t coord_cursor_x();
static uint32_t coord_cursor_y();
static void paint_block_black(const uint32_t x, const uint32_t y);
static void copy_pixel_block(const uint32_t src_x, const uint32_t src_y, const uint32_t dest_x, const uint32_t dest_y);
static void move_right(uint32_t* const x,uint32_t* const y);
static void move_left(uint32_t* const x, uint32_t* const y);
static void move_chars_right();
static void move_chars_left();
static uint8_t is_cursor_at_end();
static uint8_t is_cursor_at_prompt();
static uint8_t is_cursor_at(const uint32_t x, const uint32_t y);



void fb_prompt()
{
	fb_print_text(prompt);
	prompt_x = cursor_x;
	prompt_y = cursor_y;
}

void fb_print_text(const char* const msg)
{
	uint32_t i = 0;
	char c = msg[i++];
	while (c != '\0')
	{
		fb_print_char(c);
		c = msg[i++];
	}
}

void fb_print_char(const char c)
{
    if (!is_cursor_at_end())
	{
		move_chars_right();
	}

	// Write char
	if (c == '\n') // TODO better handling (if cursor not at end : copy chars right, etc)
	{
		cursor_x = 0;
		cursor_y += 1;
		end_x = cursor_x;
		end_y = cursor_y;
	}
	else
	{
		uint64_t f = font[(int)c];
		draw_bitmap(f);
		move_right(&cursor_x, &cursor_y);
	    move_right(&end_x, &end_y);
	}

}

void fb_print_int(const int x)
{
	if(x ==0)
	{
		fb_print_char(OFFSET);
		return;
	}
		
	int digit;
	int tmp = x;
	int i = 0;
	int a,b;
	while(tmp > 0)
	{
		tmp = divide(tmp,10);
		i++;
	}
	while(i > 0)
	{
		a = pow(10,i-1);
		b = divide(x,a);
		digit = mod(b,10);
		fb_print_char(digit + OFFSET);
		i--;
	}	
}

static uint8_t is_cursor_at_end()
{
	return is_cursor_at(end_x, end_y);
}

// Draws a 8x8 bitmap at current position, represented by a uin64_t (=one character)
static void draw_bitmap(const uint64_t bitmap)
{
	uint8_t x, y;
    uint32_t cursor_x = coord_cursor_x();
    uint32_t cursor_y = coord_cursor_y();
    uint32_t i, j, x_rea, y_rea;
	for (y = 0; y < BITMAP_SIZE; ++y)
	{
		for (x = 0; x < BITMAP_SIZE; ++x)
		{
			uint64_t bitmap_color = get_bitmap_pixel_color(bitmap, x, y);
			
            x_rea = to_zoomed(x) + cursor_x;
            y_rea = to_zoomed(y) + cursor_y;
            
			if (bitmap_color != 0)
			{
                for(i = 0; i < (1<<(zoom-1)); i++){
                    for(j = 0; j < (1<<(zoom-1)); j++){
				        put_pixel_RGB24(x_rea+i, y_rea+j, 255, 255, 255);
                    }
                }
			}
			else
			{
                for(i = 0; i < (1<<(zoom-1)); i++){
                    for(j = 0; j < (1<<(zoom-1)); j++){
				        put_pixel_RGB24(x_rea+i, y_rea+j, 0, 0, 0);
                    }
                }
			}
		}
	}
}

static uint64_t get_bitmap_pixel_color(const uint64_t bitmap, const uint8_t x, const uint8_t y)
{
	uint8_t curr_bit_number = 63 - (BITMAP_SIZE*y) - x;
	uint64_t mask = (uint64_t) 1 << curr_bit_number;
	return bitmap & mask;
}

void fb_backspace()
{
	if (is_cursor_at_prompt())
	{
		// TODO play sound
		return;
	}
	
	move_left(&cursor_x, &cursor_y);
	fb_delete();
}

static uint8_t is_cursor_at_prompt()
{
	return is_cursor_at(prompt_x, prompt_y);
}

void fb_delete()
{
	if (is_cursor_at_end())
	{
		// TODO play sound
		return;
	}

	paint_block_black(cursor_x, cursor_y);

	if (!is_cursor_at_end())
	{
		move_chars_left();
	}

	move_left(&end_x, &end_y);
}

void fb_move_cursor_left()
{
	if (is_cursor_at_prompt())
	{
		// TODO play sound
		return;
	}

	move_left(&cursor_x, &cursor_y);
}

void fb_move_cursor_right()
{
	if (is_cursor_at_end())
	{
		// TODO play sound
		return;
	}

	move_right(&cursor_x, &cursor_y);
}

void fb_clear()
{
	paint_all_black();

	cursor_x = 0;
	cursor_y = 0;

	fb_prompt();
}

void fb_set_zoom(const uint8_t new_zoom) // TODO should update all the current written characters
{
	zoom = new_zoom;
    size = zoom * DEFAULT_FONT_SIZE;
    max_x = divide(fb_x, size);
}


// TODO écrire variable de type différent comme un : printf("%s, %d", "coucou", 4);

// TODO prompt personnalisable (avec une commande shell : au début : juste du texte. Puis un \t ou un truc comme ça = sys_gettime() qui sera affiché)

static uint32_t to_zoomed(const uint32_t coord)
{
    return coord * zoom;
}

static uint32_t to_rea(const uint32_t coord)
{
    return coord * size;
}

static uint32_t coord_cursor_x()
{
    return size * cursor_x;
}

static uint32_t coord_cursor_y()
{
    return size * cursor_y;
}

static void paint_block_black(const uint32_t x, const uint32_t y)
{
	uint32_t x_rea = to_rea(x);
    uint32_t y_rea = to_rea(y);
    uint32_t limit_x = x_rea + size;
	uint32_t limit_y = y_rea + size;

    uint32_t x_init = x_rea;
	for (; y_rea < limit_y; y_rea++)
	{
        x_rea = x_init;
		for (; x_rea < limit_x; x_rea++)
		{
			put_pixel_RGB24(x_rea, y_rea, 0, 0, 0);
		}
	}
}

static void copy_pixel_block(const uint32_t src_x, const uint32_t src_y, const uint32_t dest_x, const uint32_t dest_y)
{
	uint32_t i_x, i_y;
	uint8_t red, green, blue;
    uint32_t src_x_rea = to_rea(src_x);
    uint32_t src_y_rea = to_rea(src_y);
    uint32_t dest_x_rea = to_rea(dest_x);
    uint32_t dest_y_rea = to_rea(dest_y);
	for (i_y = 0; i_y < size; i_y++)
	{
		for (i_x = 0; i_x < size; i_x++)
		{
			get_pixel_RGB24(src_x_rea + i_x, src_y_rea + i_y, &red, &green, &blue);
			put_pixel_RGB24(dest_x_rea + i_x, dest_y_rea + i_y, red, green, blue);
		}
	}
}

static void move_right(uint32_t* const x, uint32_t* const y)
{
	(*x) = (*x) + 1; // Move forward
	if ((*x) > max_x)	// If it is too far, go to next line
	{
		(*x) = 0;
		(*y) = (*y) + 1;
	}
}

static void move_left(uint32_t* const x, uint32_t* const y)
{
	if ((*x) == 0) // If at the left border, move up to the end of upper line
	{
		(*y) = (*y) - 1;
		(*x) = max_x;
	}
	else // Else, just move forward
	{
		(*x) = (*x) - 1;
	}
}

static void move_chars_right()
{
	uint32_t curr_x = end_x;
	uint32_t curr_y = end_y;
	uint32_t prev_x = end_x;
	uint32_t prev_y = end_y;
	while (!is_cursor_at(curr_x, curr_y))
	{
		move_left(&prev_x, &prev_y);

		// Copy into this case the content of the previous one
		copy_pixel_block(prev_x, prev_y, curr_x, curr_y);

		// Update current coords
		curr_x = prev_x;
		curr_y = prev_y;
	}
}

static void move_chars_left()
{
	uint32_t src_x, src_y, dest_x, dest_y;
	dest_x = cursor_x;
	dest_y = cursor_y;
	src_x = cursor_x;
	src_y = cursor_y;

	move_right(&src_x, &src_y);

	while (dest_x != end_x || dest_y != end_y)
	{
		copy_pixel_block(src_x, src_y, dest_x, dest_y);

		dest_x = src_x;
		dest_y = src_y;

		move_right(&src_x, &src_y);
	}
}

static uint8_t is_cursor_at(const uint32_t x, const uint32_t y)
{
	return cursor_x == x && cursor_y == y;
}
