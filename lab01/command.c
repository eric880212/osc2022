#include "uart.h"
#include "command.h"
#include "reboot.h"

void help(){
    uart_puts(
        "\rhelp\t: print this help menu\n"
        "\rhello\t: print Hello World!\n"
        "reboot\t: reboot the device\n");
}

void hello(){
    uart_puts("\rHello World!\n");
}

void reboot(){
   reset(10);
}
