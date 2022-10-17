#include "shell.h"
#include "command.h"
#include "string.h"

char str_load[4] = "load";

void shell(char *s){
    switch(strlen(s)){
    case 4:
        if(!strcmp(s, str_load)){
            bootloader();
            break;
        }
    }
}
