#include "command.h"
#include "uart.h"
#include "string.h"
void load_kernel(){
    uart_puts("\rload kernel...\n");
    //char temp[4];
    char temp[20];
    int kernel_size;
    for(int i=0; i<4; i++){
        char c = uart_getc_raw();
        //uart_send(c);
        temp[i] = c;
        //kernel_size |= (c << (i*8)) ;
    }
    int *n = (int *)temp;
    kernel_size = *n;
    //itoa(kernel_size, temp , 4);
    uart_puts("start loading: ");
    uart_hex(kernel_size);
    uart_puts(" bytes\n");
    for(int i=0; i<kernel_size; i++){
        //void (*in)(void) = (void*)0x80000;
        //in();
        char c = uart_getc_raw();
        //OLD_ADDR[i] = c;
        *(OLD_ADDR + i) = c;
        //uart_puts("a");
    }
    uart_puts("finish\n");
    void (*jump)(void) = (void*)0x80000;
    jump();
}
