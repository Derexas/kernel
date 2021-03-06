
#include "string.h"

int strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2; s1++, s2++)
        if (*s1 == '\0')
            return 0;
    return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}

unsigned int strlen(const char* string)
{
    unsigned int result = 0;
    while ( string[result] )
        result++;
    return result;
}

char * strcat(char * str1, const char * str2)
{
    int str1_s = strlen(str1);
    int str2_s = strlen(str2);
    unsigned int i = 0;
    do {
        str1[str1_s + i] = str2[i];
        i++;
    } while ( str2[i-1] );
    
    char * str = malloc(sizeof(char)*(str1_s + str2_s));  
}

uint8_t strcopy(char * to, const char * from)
{
    int to_s = strlen(to);
    int from_s = strlen(from);
    /*if (to_s < from_s)
        return 0;*/
    for(int i = 0; i < from_s; i++)
        *(to + i) = *(from + i);
    return 1;
}
