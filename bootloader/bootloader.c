#include "uart.h"
#include "shell.h"
void main()
{
    // set up serial console
    uart_init();
    //print_hw_info();
    while(1) {
        char s[100] = "";
        uart_puts("\r");
        int i = 0;
        while(1){
            char c = uart_getc();
            if(c < 0 || c > 127)
                continue;
            uart_send(c);
            if(c == '\n')
                break;
            else
                s[i++] = c;
        }
        //parse the command
        shell(s);
    }
}
