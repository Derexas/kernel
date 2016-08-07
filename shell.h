
#include "string.h"

#define UP 72
#define DOWN 80

#define MAX_COMMANDS 100

typedef struct
{
    char * name;
    char * description;
    void * function;
} command_table_t;

typedef void (*myFunc_v)();

void print(char * str);
void next_line();
void help_command();

void init_shell();
void shell();

void getinput();

void add_new_command(char *name, char* description, void *function);
int findCommand(char* command);