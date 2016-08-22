

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 55

char * vidptr;

char command_str[SCREEN_WIDTH*10240];

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
        command_str[j] = vidptr[j] = ' ';
        /* attribute-byte - light grey on black screen */
        command_str[j+1] = vidptr[j+1] = 0x07;
        j = j + 2;
    }
}

void print_from_command_str(uint32_t decal)
{
    for (uint32_t i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; i+=1)
        vidptr[i] = command_str[i + SCREEN_WIDTH*decal];
}

uint32_t print_str_to_screen_c(const char* str, unsigned int column, unsigned int line, uint32_t * color)
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
            command_str[i] = vidptr[i] = str[j];
            /* attribute-byte: give character black bg and light grey bg */
            command_str[i + 1] = vidptr[i+1] = (color == 0 || color[j] == 0)? 0x07 : color[j];
            i = i + 2;
        }
	++j;
    }
    return (i/SCREEN_WIDTH)-line;
}

uint32_t print_str_to_screen(const char* str, unsigned int column, unsigned int line)
{
    print_str_to_screen_c(str, column, line, 0);
}

uint32_t print_int_to_screen(int n, unsigned int column, unsigned int line)
{
    char * str = its(n);
    uint32_t ret = print_str_to_screen(str, column, line);
    mfree(str);
    return ret;
}

void print_char_to_screen_c(char c, unsigned int column, unsigned int line, uint32_t co)
{
    char * str = " ";
    str[0] = c;
    uint32_t colors[1] = {co};
    print_str_to_screen_c(str, column, line, colors);
}

void print_char_to_screen(char c, unsigned int column, unsigned int line)
{
    print_char_to_screen_c(c, column, line, 0);
}
