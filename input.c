
#define BACK 0
#define TAB 1
#define ENTER 2
#define LCTRL 3
#define LSHIFT 4
#define RSHIFT 5
#define LALT 6
#define CAPSLCK 7
#define F1 8
#define F2 9
#define F3 10
#define F5 11
#define F6 12
#define F7 13
#define F8 14
#define F9 15
#define F10 16
#define NBLCK 17
#define SCOLLLCK 18
#define ESC 19

char scancode[127] = {0};

void init_input()
{
    char l1[100] = {ESC,145,'&',130,'"','\'','(','-',138,'_',135,133,')','=',BACK,TAB,'a','z','e','r','t','y','u','i','o','p','^','$',ENTER,LCTRL,'q','s','d','f','g','h','j','k','l','m',151,'*',LSHIFT,'<','w','x','c','v','b','n',',',';',':','!',RSHIFT,'*',LALT,' ',CAPSLCK,F1,F2,F3,F5,F6,F7,F8,F8,F9,F10,NBLCK,SCOLLLCK,'7','8','9','-','4','5','6','+','1','2','3','0','.'};
    for(int i = 0x01; i <= 0x53; i++) scancode[i] = l1[i];
}

/*
PS/2 keyboard code.
Dependencies:
inb function and scancode table.
*/
char getScancode()
{
    char c=0;
    do {
        if(inb(0x60)!=c)
        {
            c=inb(0x60);
            if(c>0)
            return c;
        }
    } while(1);
}

char getchar()
{
    return scancode[getScancode()+1];
}

char getcharfromcode(char c)
{
    return scancode[c];
}