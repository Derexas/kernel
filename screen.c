

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 25

char * vidptr;

void init_screen()
{
    vidptr = (char*)0xb8000;//video mem begins here.
}

void clear_screen()
{
    unsigned int j = 0;
    /* this loops clears the screen
    * there are 25 lines each of 80 columns; each element takes 2 bytes */
    while(j < SCREEN_WIDTH * SCREEN_HEIGHT * 2) {
	/* blank character */
	vidptr[j] = ' ';
	/* attribute-byte - light grey on black screen */
	vidptr[j+1] = 0x07;
	j = j + 2;
    }
}

uint32_t print_str_to_screen(const char* str, unsigned int column, unsigned int line)
{
    unsigned int i = line * SCREEN_WIDTH + column * 2;
    unsigned int j = 0;
    /* this loop writes the string to video memory */
    while(str[j] != '\0') {
	/* the character's ascii */
        if (str[j] == '\n') {
            i += SCREEN_WIDTH;
            i = (i / SCREEN_WIDTH) * SCREEN_WIDTH;            
        } else {
            vidptr[i] = str[j];
            /* attribute-byte: give character black bg and light grey bg */
            vidptr[i+1] = 0x07;
            i = i + 2;
        }
	++j;
    }
    return (i/SCREEN_WIDTH)-line;
}

uint32_t print_int_to_screen(int n, unsigned int column, unsigned int line)
{
    return print_str_to_screen(its(n), column, line);
}

void print_char_to_screen(char c, unsigned int column, unsigned int line)
{
    char * str = " ";
    str[0] = c;
    print_str_to_screen(str, column, line);
}