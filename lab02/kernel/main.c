#include "uart.h"
#include "mbox.h"
#include "reboot.h"
#include "shell.h"
void print_hw_info(){
    // print hardware information by mail box
    mbox[0] = 7*4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_GET_BOARD_REVISION;
    mbox[3] = 4;
    mbox[4] = 0; //TAG_REQUEST_CODE
    mbox[5] = 0; //VALUE BUFFER
    mbox[6] = MBOX_TAG_LAST;
    if(mbox_call(MBOX_CH_PROP)){
        uart_puts("My board revision is: ");
        uart_hex(mbox[5]);
        uart_puts("\n");

    }
    else{
        uart_puts("Unable \n");
    }

    mbox[0] = 8*4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_GET_ARM_MEMORY;
    mbox[3] = 8;
    mbox[4] = 0; //TAG_REQUEST_CODE
    mbox[5] = 0; //VALUE BUFFER
    mbox[6] = 0;
    mbox[7] = MBOX_TAG_LAST;
    if(mbox_call(MBOX_CH_PROP)){
        uart_puts("My ARM memory address base address is:");
        uart_hex(mbox[5]);
        uart_puts("\n");
        uart_puts("size is:");
        uart_hex(mbox[6]);
        uart_puts("\n");
    }
    else{
        uart_puts("Unable \n");
    }
}
void main()
{
    // set up serial console
    uart_init();
    //print_hw_info();
    while(1) {
        char s[100] = "";
        uart_puts("\r#");
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
