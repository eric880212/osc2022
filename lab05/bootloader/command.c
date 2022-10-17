#include "uart.h"
#include "command.h"
#include "load.h"

void bootloader(){
    uart_puts("\rbootloader\n");
    //relocate then jump to load kernel
    //copy myself from 0x80000 to 0x60000
    extern int _start, _end;
    unsigned char *start, *end;
    start = (unsigned char*)&_start;
    end = (unsigned char*)&_end;
    unsigned int size = end-start;
    for(int i=0;i<=size; i++){
        NEW_ADDR[i] = OLD_ADDR[i];
    }
    uart_puts("\rrelocate finish\n");
    //jump to load_kernel() function which is at 0x60000+?
    void (*func)() = (void*)(load_kernel + 0x60000 - 0x80000);
    func();
}
