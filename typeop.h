//#include <stdlib.h>

char * its(int n)
{
    char str[128];
    int i = 0;
    if(n == 0) {
        str[0] = '0';
        i++;
    }
    while(n > 0 && i < 128) {
        str[i] = n % 10 + '0';
        n /= 10;
        i++;
    }
    char * str2 = malloc((i+1)*sizeof(char));
    str2[i] = '\0';
    for(int j = 0; j < i; j++)
        str2[j] = str[i - j - 1];
    return str2;
}

char * its_p(int n, void * p)
{
    char str[128];
    int i = 0;
    if(n == 0) {
        str[0] = '0';
        i++;
    }
    while(n > 0 && i < 128) {
        str[i] = n % 10 + '0';
        n /= 10;
        i++;
    }
    char * str2 = p;
    str2[i] = '\0';
    for(int j = 0; j < i; j++)
        str2[j] = str[i - j - 1];
    return str2;
}

char itc(int d)
{
    return d + '0';
}
