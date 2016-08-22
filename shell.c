
#include "shell.h"
#include "string.c"

struct command_table
{
    char * name;
    char * description;
    void * function;
};

char * symbol;
command_table_t CommandTable[MAX_COMMANDS];
unsigned int NumberOfCommands;
unsigned int current_pos;
uint32_t decal = 0;

void print(char * str)
{
    unsigned int current_line = current_pos/SCREEN_WIDTH;
    unsigned int current_column = current_pos - current_line*SCREEN_WIDTH;
    uint32_t lines_jumped = print_str_to_screen(str, current_column, current_line);
    //print_from_command_str(decal);
    if (lines_jumped > 0) {
        current_pos += (lines_jumped + 1) * SCREEN_WIDTH; // +1 for the print of lines_jumped
        current_line = current_pos/SCREEN_WIDTH;
        current_column = current_pos - current_line*SCREEN_WIDTH;
        //print_str_to_screen(its(lines_jumped), current_column, current_line);
    } else       
        current_pos += strlen(str);
}

void printj(char * str)
{
    print(str);
    next_line();
}

void printij(uint32_t i)
{
    char * str = its(i);
    printj(str);
    mfree(str);
}

void printijp(uint32_t i)
{
    char * str = its_p(i, buffer);
    printj(str);
}

void printcc(char c, uint32_t co)
{
    unsigned int current_line = current_pos/SCREEN_WIDTH;
    unsigned int current_column = current_pos - current_line*SCREEN_WIDTH;
    print_char_to_screen_c(c, current_column, current_line, co);
    current_pos += 1;
}

void printc(char c)
{
    printcc(c, 0);
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
    
uint32_t col = 0;

void print_mem()
{
    if (dynamic_memory[0] != fcode && dynamic_memory[0] != ucode);
    else {
    uint32_t u = 0x04;
    uint32_t f = 0x02;
    for(int i = 0; i < 1000; i++) {
        for(int j = 0; j < 80; j++) {
            char t = stuffx(j+i*80);
            uint8_t c = dynamic_memory[j+i*80];
            uint32_t co = (c == fcode)?0:(c == ucode)?0:col;
            if (col == u)
                col = (c == ucode)?0:u;
            else if (col == f)
                col = (c == fcode)?0:f;
            else if (col == 0)
                col = (c == fcode)?f:(c == ucode)?u:0;
            printcc(t, co);
            write_serial(t);
            if (dynamic_memory[j+i*80] == fcode && dynamic_memory[j+i*80 + 1] != ucode && ((dynamic_memory[j+i*80 + 2] != 0 && dynamic_memory[j+i*80 + 3] != 0 && dynamic_memory[j+i*80 + 4] == 0) || dynamic_memory[j+i*80 + 4] == 1)) {
                printcc(stuffx(j+i*80 +1), f);printcc(stuffx(j+i*80+2), f);
                printcc(stuffx(j+i*80+3), f); printcc(stuffx(j+i*80+4), f); col = 0;
                i = j = 100000; //break
            }
        }
        next_line();
    //
    }
    write_serial('\n');
    }
    printc((char)((max_free_blocks - 1) - flp_pointer) + '0');
    next_line();
    int b = 32;
    char used[b];
    for (uint16_t i = 0; i < b/*(max_free_blocks - 1) - flp_pointer*/; i++) {
        if (i == (max_free_blocks - 1) - flp_pointer)
            printcc('|', 0x03);
        char a = (char)free_leaf_pile[(max_free_blocks - 1) - i];
        if (i >= (max_free_blocks - 1) - flp_pointer)
            used[a] = 1;
        printc(a + '0');
    }
    next_line();
    for (uint16_t i = 0; i < b; i++) {
        if (!used[i]) {
            printc(i + '0');printc(':');
            for (uint8_t j = 0; j < sizeof(leaf_t); j++)
                printc(*(j + (uint8_t*)&free_mem_leafs[i]) + '0');
            printc(' ');
        }
    }
    mem_tree();
    next_line();
}

void mem_tree()
{    
    printj("----------------");
    char * ta = tree_to_string(free_mem_tree);
    if (ta != 0)
        print(ta);
    printj("----------------");
}

tree_t * b_tree;

void memory()
{
    //print_mem();
    //mem_tree();
    print_mem();
    int * mem_info = memory_info();
    for(int i = 1; i < mem_info[0]; i++) {
        printij(mem_info[i]);
    //print_mem();
    }
    print_mem();
    mfree(mem_info);
    printj(ez); ez[0] = 0;
    print_mem();
}

void test_tree()
{
//     print_mem();
//     mem_tree();
//     mem_tree();
    char * str;
    write_serial('A');
    
    b_tree = init_b_tree(8, (void*)10);//printc('8');
    ez[0] = 0;
    next_line();
    insert_data(b_tree, 8, (void*)1);//printc('8');
    insert_data(b_tree, 4, (void*)0);//printc('4');
    insert_data(b_tree, 6, (void*)0);//printc('6');
    insert_data(b_tree, 5, (void*)0);//printc('5');
    
    insert_data(b_tree, 3, (void*)0);//printc('3');//its(51515);
    //str = tree_to_string(b_tree);//printj(str);
    insert_data(b_tree, 0, (void*)0);//printc('0');
    insert_data(b_tree, 2, (void*)0);//printc('2');
    remove_leaf(b_tree, 8, (void*)10);
    
    str = tree_to_string(b_tree);
    printj(str);
    print_mem();
//     print_mem();
    //mem_tree();
//     printc('p');
    mfree(str);
//     print_mem();
    //mem_tree();
    
    /*printij( ((leaf_t*)find_leaf(b_tree, 1))->key );
    printij( ((leaf_t*)find_leaf(b_tree, 7))->key );*/
    free_tree(b_tree);
//     mfree(b_tree->root->left);
// //     print_mem();
// //     mem_tree();
//     mfree(b_tree->root->right);
// //     print_mem();
// //     mem_tree();
//     mfree(b_tree->root);
// //     printj(buffer);
//     print_mem();
// //     mem_tree();
//     mfree(b_tree);
//     print_mem();
//     printj(buffer);
//     mem_tree();printj(ez);
    print_mem();
    mem_tree();
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
    add_new_command("memoryp", "print memory infos", print_mem);
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
        } else if (oldc == 72) { //up
            if (decal != 0) decal--;
            print_from_command_str(decal);
        } else if (oldc == 80) { //down
            decal++;
            print_from_command_str(decal);
        } else {
            st[0] = c;//getchar(c,1);
            command_str[i] = st[0];
            print(st);
            //print(its_p(oldc, buffer));
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
