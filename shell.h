
#include "string.h"

#define UP 72
#define DOWN 80

#define MAX_COMMANDS 100

typedef struct command_table command_table_t;

typedef void (*myFunc_v)();

void print(char * str);
void printj(char * str);
void printij(uint32_t i);
void printijp(uint32_t i);
void printc(char c);
void next_line();
void help_command();
void print_mem();
void mem_tree();

void init_shell();
void shell();

void getinput();

void add_new_command(char *name, char* description, void *function);
int findCommand(char* command);
