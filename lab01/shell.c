#include "shell.h"
#include "command.h"
#include "string.h"
char str_help[4] = "help";
char str_hello[5] = "hello";
char str_reboot[6] = "reboot";

void shell(char *s){
    switch(strlen(s)){
    case 4:
        if(!strcmp(s, str_help))
            help();
        break;
    case 5:
        if(!strcmp(s, str_hello))
            hello();
        break;
    case 6:
        if(!strcmp(s, str_reboot))
            reboot();
        break;
    }
}
