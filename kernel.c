#include <stdint.h>
#include "b_tree.h"
#include "memory.h"
#include "stdbool.h"
#include "typeop.h"
#include "b_tree.c"
#include "memory.c"
#include "inb.c"
#include "input.c"
#include "time.c"
#include "screen.c"
#include "shell.c"

void init()
{
    init_memory();
    for(unsigned int i = 0; i < 128; i++)
        scancode[i] = i + '0';
    init_input();
    init_screen();
    init_shell();
}

void kmain(void)
{
        init();
	char *str = "my first kernel";
	
        clear_screen();
        /*print_str_to_screen(str, 0, 0);
        
        print_str_to_screen("blah", 0, 2);
        print_str_to_screen("blah", 0, 4);
        print_str_to_screen("blah", 5, 5);*/
        
        shell();
        
        /*unsigned int i = 0;
        char c = -1, oldc = -1;
        str[1] = '\0';
        while(1) {
            while(c <= 0) {
                c = inb(0x60);
            }
            oldc = c;
            str[0] = getchar(c);
            print_str_to_screen(str, i, 3);
            while(c == oldc) {
                c = inb(0x60);
            }
            i++;
        }*/
        
	return;
}