#include "string.h"

int strlen ( char * s ){
    int i = 0;
    while ( 1 )
    {
        if ( *(s+i) == '\0' )
            break;
        i++;
    }
    return i;
}
int strcmp(char *str1, char *str2){
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);
    int max = str1_len;
    if(str1_len < str2_len)
        max = str2_len;
    for(int i = 0; i < max; i++){
        if((str1[i] - str2[i]) != 0)
            return str1-str2;
    }
    return 0;
}
void reverse ( char * s ){
    int i;
    char temp;

    for ( i = 0; i < strlen(s) / 2; i++ ) 
    {
        temp = s[strlen(s) - i - 1];
        s[strlen(s) - i - 1] = s[i];
        s[i] = temp;
    }
}

void itoa (int x, char str[], int d) { 
    int i = 0; 
    while (x) { 
        str[i++] = (x % 10) + '0'; 
        x = x / 10; 
    } 
  
    // If number of digits required is more, then 
    // add 0s at the beginning 
    while (i < d) 
        str[i++] = '0'; 
    
    str[i] = '\0'; 
    reverse(str); 
} 

int char2int(char c){
    if(c >= '0' && c <= '9')
        return c - '0';
    else{
        switch(c){
            case'A':
                return 10;
            case'B':
                return 11;
            case'C':
                return 12;
            case 'D':
                return 13;
            case'E':
                return 14;
            case'F':
                return 15;
        }
    }
    return 0;
}
int str_hex2int(char *s){
    int num = 0;
    for(int i=0; i<8; i++){
        num += char2int(s[i]);
        if(i != 7)
            num *= 16;
    }
    return num;
}
