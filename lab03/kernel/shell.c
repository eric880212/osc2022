#include "shell.h"
#include "uart.h"
#include "command.h"
#include "string.h"

char str_test[4] = "test";
char str_help[4] = "help";
char str_hello[5] = "hello";
char str_reboot[6] = "reboot";
char str_ls[2] = "ls";
char str_cat[3] = "cat";
char str_svc[3] = "svc";
char str_timer[5] = "timer";
char str_setTimeout[10] = "setTimeout";
void shell(char *cmd, char arg[][16]){
    switch(strlen(cmd)){
    case 2:
        if(!strcmp(cmd, str_ls)){
            ls();
            break;
        }

    case 3:
        if(!strcmp(cmd, str_cat)){
            cat();
            break;
        }

        if(!strcmp(cmd, str_svc)){
            svc();
            break;
        }

    case 4:
        if(!strcmp(cmd, str_help)){
            help();
            break;
        }
        if(!strcmp(cmd, str_test)){
            test();
            break;
        }
        
    case 5:
        if(!strcmp(cmd, str_hello)){
            hello();
            break;
        }
        else if(!strcmp(cmd, str_timer)){
            timer();
            break;
        }

    case 6:
        if(!strcmp(cmd, str_reboot)){
            reboot();
            break;
        }
    case 10:
        if(!strcmp(cmd, str_setTimeout)){
            for(int i=0;i<16;i++)
                if(!arg[1][i]){
                    arg[1][i] = '\n';
                    arg[1][i+1] = '#';
                    break;
                }
            setTimeout(arg);
            break;
        }
    }
}
