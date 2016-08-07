
#include "shell.h"

char * symbol;
command_table_t CommandTable[MAX_COMMANDS];
unsigned int NumberOfCommands;
unsigned int current_pos;
char command_str[4096];

void print(char * str)
{
    unsigned int current_line = current_pos/SCREEN_WIDTH;
    unsigned int current_column = current_pos - current_line*SCREEN_WIDTH;
    uint32_t lines_jumped = print_str_to_screen(str, current_column, current_line);
    if (lines_jumped > 0) {
        current_pos += (lines_jumped + 1) * SCREEN_WIDTH; // +1 for the print of lines_jumped
        current_line = current_pos/SCREEN_WIDTH;
        current_column = current_pos - current_line*SCREEN_WIDTH;
        print_str_to_screen(its(lines_jumped), current_column, current_line);
    } else       
        current_pos += strlen(str);
}

void printj(char * str)
{
    print(str);
    next_line();
}

void printc(char c)
{
    unsigned int current_line = current_pos/SCREEN_WIDTH;
    unsigned int current_column = current_pos - current_line*SCREEN_WIDTH;
    print_char_to_screen(c, current_column, current_line);
    current_pos += 1;
}

void clear()
{
    current_pos = 0; //because followed by next_line
    clear_screen();
}

void tableprint()
{
    int k = 0;
    for(int i = 1; i < 24; i++) {
        for(int j = 0; j < 8; j++) {
            int kk = k;
            print_int_to_screen(kk, j*10, i); print_str_to_screen(":", j*10+3, i);
            print_char_to_screen((char)kk, j*10+6, i);
            k++;
        }
        next_line();
    }
}

void help_command()
{
    for(unsigned int i = 0; i < NumberOfCommands; i++) {
        print(CommandTable[i].name); print(" : "); print(CommandTable[i].description);
        next_line();
    }
}

void print_mem()
{
    for(int i = 0; i < 64; i++)
        printc(stuffx(i));
    next_line();
}

tree_t * b_tree;

void memory()
{
    for(int i = 0; i < 64+16; i++)
        printc(stuffx(i));
    next_line();
    
    int * mem_info = memory_info();
    for(int i = 1; i < mem_info[0]; i++) {
        char * str = its(mem_info[i]);
        print(str);
        next_line();
    }
    printj("----------------");
    printj(tree_to_string(free_mem_tree));
    printj("----------------");
    printj(its(malloc_count));
}

void test_tree()
{
    /*struct tree {
        char (*comp) (uint32_t, uint32_t); // FUNC POINTER STUFF TESt : OK
    } tree;
    char add(uint32_t a, uint32_t b) {
        return 'e';
    }
    struct tree t;
    t.comp = &add;
    int add2to3(char (*comp) (uint32_t, uint32_t)) {
        return (*comp)(2, 3);
    }
    printc(add2to3(t.comp));
    next_line();*/
    char * str;
    /*uint8_t comp (uint32_t a, uint32_t b) {
        return (a > b) ? 1 : (a < b) ? -1 : 0;
    };*/
    //printc(STF() + '0');
    b_tree = init_b_tree(8, (void*)10);//, &comp);
    //printc((char)b_tree->comp+'0');
    next_line();
    insert_data(b_tree, 8, (void*)1);
    insert_data(b_tree, 4, (void*)0);
    insert_data(b_tree, 6, (void*)0);
    insert_data(b_tree, 5, (void*)0);
    /*insert_data(b_tree, 1, 0);
    insert_data(b_tree, 3, 0);
    insert_data(b_tree, 2, 0);*/
    insert_data(b_tree, 8, (void*)2);
    insert_data(b_tree, 8, (void*)3);
    insert_data(b_tree, 8, (void*)4);
    insert_data(b_tree, 8, (void*)5);
    insert_data(b_tree, 8, (void*)6);
    insert_data(b_tree, 8, (void*)7);
    insert_data(b_tree, 8, (void*)8);
    /*
    insert_data(b_tree, 8, 10);*/
    
    str = tree_to_string(b_tree);
    printj(str);
    insert_data(b_tree, 3, (void*)0);
    insert_data(b_tree, 0, (void*)0);
    insert_data(b_tree, 2, (void*)0);
    //remove_leaf(b_tree, 8, (void*)1);
    next_line();
    insert_data(b_tree, 8, (void*)9);
    
    /*printj(its((uint32_t)find_leaf(b_tree, (void*)4)));
    char * str = to_string(b_tree);
    printj(str);
    next_line();*/
    
    // PROBLEM WITH MALLOC :((((
    //for(int i = 0; i < 1000; i++) { char * cc = malloc(2); for(int i = 0; i < 2; i++) cc[i] = i%255; }
    //for(int i = 0; i < 1000; i++) its(5); // THIS DOES
    
    /*print(its((uint32_t)b_tree->root->key));                 printc(' ');printj(its(height(b_tree->root)));
    print(its((uint32_t)b_tree->root->right->key));          printc(' ');printj(its(height(b_tree->root->right)));
    print(its((uint32_t)b_tree->root->left->key));           printc(' ');printj(its(height(b_tree->root->left)));
    print(its((uint32_t)b_tree->root->right->right->key));   printc(' ');printj(its(height(b_tree->root->right->right)));
    print(its((uint32_t)b_tree->root->right->left->key));    printc(' ');printj(its(height(b_tree->root->right->left)));
    print(its((uint32_t)b_tree->root->left->right->key));    printc(' ');printj(its(height(b_tree->root->left->right)));
    print(its((uint32_t)b_tree->root->left->left->key));     printc(' ');printj(its(height(b_tree->root->left->left)));*/
    
    
    str = tree_to_string(b_tree);
    printj(str);
    
}

void init_shell()
{
    symbol = "> ";
    current_pos = 0;
    NumberOfCommands = 0;
    add_new_command("help", "You code this one.", help_command);
    add_new_command("print", "Print the arg", print);
    add_new_command("clear", "clear the screen", clear);
    add_new_command("tableprint", "Print the char/code table", tableprint);
    add_new_command("memory", "print memory infos", memory);
    add_new_command("tree", "test the tree structure", test_tree);
}

void shell()
{
    while(command_str != "exit") {;
        
        print(symbol);
        getinput(); //load command_str
        next_line();
        
        
        if(command_str != "exit") {
            int i = findCommand(command_str);
            if(i >= 0) {
                myFunc_v f = CommandTable[i].function;
                f();
            } else {
                print("function not found");
                next_line();
            }
        }
    }
    print("bye!");
}

void getinput()
{
    unsigned int i = 0;
    char c = 0, oldc = c;
    char * st = " ";
    bool back, enter = false, command_overflow;
    while(!enter) {
        while(c <= 0) {
            c = inb(0x60);
        }
        oldc = c;
        c = getcharfromcode(c);
        back = (c == BACK);
        enter = (c == ENTER);
        command_overflow = (i == 4096);
        if(back || enter || command_overflow) {
            if (back && i > 0) {
                i--;
                current_pos--;
                print(" \0"); // NEVER use " " only (interpret as "" or something very fucky and makes bugs)
                current_pos--;
            } else if (enter) {
                //print_str_to_screen("enter", 0, 12);
            }
        } else {
            st[0] = c;//getchar(c,1);
            command_str[i] = st[0];
            print(st);
            i++;
        }
        c = oldc;
        while(c == oldc) {
            c = inb(0x60);
        }
    }
    command_str[i] = '\0';
}

void next_line()
{
    current_pos = (current_pos/SCREEN_WIDTH + 1) * SCREEN_WIDTH;
}

void add_new_command(char *name, char* description, void *function)
{
    if(NumberOfCommands + 1 < MAX_COMMANDS)
    { 
        CommandTable[NumberOfCommands].name = name;
        CommandTable[NumberOfCommands].description = description;
        CommandTable[NumberOfCommands].function = function;
        NumberOfCommands++;
    }
    return;
}

int findCommand(char* command)
{
    int i;
    int ret;
 
    for(i = 0; i <= NumberOfCommands; i++)
    {
        ret = strcmp(command, CommandTable[i].name);
 
        if(ret == 0)
            return i;
    }
    return -1;
}