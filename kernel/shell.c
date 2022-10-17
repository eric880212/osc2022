#include "shell.h"
#include "uart.h"
#include "command.h"
#include "string.h"

char str_test[5] = "test\0";
char str_help[5] = "help\0";
char str_hello[6] = "hello\0";
char str_reboot[7] = "reboot\0";
char str_ls[3] = "ls\0";
char str_cat[4] = "cat\0";
char str_svc[4] = "svc\0";
char str_timer[6] = "timer\0";
char str_setTimeout[11] = "setTimeout\0";
char str_palloc[7] = "palloc\0";
char str_malloc[7] = "malloc\0";
char str_freepage[9] = "freepage\0";
char str_free[5] = "free\0";
char str_reserve[4] = "rsv\0";
char str_async_IO[9] = "async_IO\0";
char str_test_kernel[12] = "test_kernel\0";

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
        if(!strcmp(cmd, str_reserve)){
            int start = str2int(arg[1], 16);
            int end = str2int(arg[2], 16);
            reserve(start, end);
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
        if(!strcmp(cmd, str_free)){
            long long int temp = str2int(arg[1], 16);
            void *addr = (void *)temp;
            free(addr);
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
        if(!strcmp(cmd, str_palloc)){ //palloc arg[1] = number of page frame
            int size = str2int(arg[1],10);
            palloc(size);
            break;
        }
        if(!strcmp(cmd, str_malloc)){ //malloc arg[1] = number of bytes
            int size = str2int(arg[1],10);
            void *temp = malloc(size);
            long long int addr = (long long int)temp;
            uart_puts("\nget address from 0x");
            uart_hex(addr);
            uart_puts("\n");
            break;
        }

    case 8:
        if(!strcmp(cmd, str_freepage)){ //free page arg[1] = index to free
            int index = str2int(arg[1],10);
            freepage(index);
            break;
        }
        if(!strcmp(cmd, str_async_IO)){
            async_IO();
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
	case 11:
		if(!strcmp(cmd, str_test_kernel)){
			test_kernel();
			break;
		}
    }
}
