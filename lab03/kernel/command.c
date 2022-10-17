#include "uart.h"
#include "command.h"
#include "reboot.h"
#include "string.h"
#include "print.h"
#include "setTimer.h"
void test(){
    asm volatile(
            "msr DAIFSet, 0xf":
            :
            :);
    asm volatile(
            "mov x0, 0\n\t"
            "mrs x1, cntpct_el0\n\t"
            "mrs x2, cntfrq_el0\n\t"
            "bl print_time\n\t"
            "bl main\n\t");
//    print_time(0);   
//    asm volatile("bl core_timer_enable\n\t");
    return;
}
void help(){
    uart_puts("\rhelp\t: print this help menu\n"
                "hello\t: print \"Hello World!\"\n"
                "reboot\t: reboot the device\n"
                "ls\t: ls\n"
                "cat\t: cat one file\n"
                "svc\t:\n"
                "");
    return;
}

void hello(){
    uart_puts("\rHello World!\n");
    return;
}

void reboot(){
    reset(10);
    return;
}

void ls(){
    char trailer[10] = "TRAILER!!!";

    char magic[6] = "070701";
    //QEMU loads the cpio archive file to 0x8000000 by default.
    //Then specify the name and loading address in config.txt eg."initramfs initramfs.cpio 0x20000000"
    char *base = (char *)0x8000000;
    while(1){
        char temp[9];
        temp[8] = '\0';
        //referenece:https://www.freebsd.org/cgi/man.cgi?query=cpio&sektion=5
        //namesize 
        for(int i=0; i<8; i++){
            temp[i] = base[i+94];
        }
        int namesize = str2int(temp,16);
        char filename[100] = ""; 
        for(int i=0; i<namesize-1; i++){
            filename[i] = base[i+110];      
        }

        if(namesize == 11){
            int i;
            for(i=0; i<namesize-1; i++){
                if(filename[i] != trailer[i])
                    break;
            }
            if(i == namesize-1)
                return;
        }
        uart_puts("\r");
        uart_puts(filename);
        uart_puts("\n");
        //count datasize
        /*
           for(int i=0; i<8; i++)
           temp[i] = base[i+54];
           int datasize = str_hex2int(temp);
         */
        //count next base
        //base += (110+datasize-1+namesize-1);
        base += 110;
        while(1){
            int i;
            for(i=0;i<6; i++){
                if(base[i] != magic[i])
                    break;
            }
            if(i == 6)
                break;
            base ++;
        }
    }
    return;
}

void cat(){
    uart_puts("\rFilename:");
    int targetsize=0;
    char target[100] = "";
    while(1){
        char c = uart_getc();
        if(c < 0 || c > 127)
            continue;
        uart_send(c);
        if(c=='\n')
            break;
        else
            target[targetsize++] = c;
    }
    char *base = (char *)0x8000000;
    char trailer[10] = "TRAILER!!!";
    char magic[6] = "070701";
    while(1){
        char temp[9];
        temp[8] = '\0';
        for(int i=0; i<8; i++){
            temp[i] = base[i+94];
        }
        int namesize = str2int(temp,16)-1;
        char filename[100] = ""; 
        for(int i=0; i<namesize; i++){
            filename[i] = base[i+110];      
        }

        if(namesize == 10){
            int i;
            for(i=0; i<namesize; i++){
                if(filename[i] != trailer[i])
                    break;
            }
            if(i == namesize){
                uart_puts("\r");
                uart_puts(target);
                uart_puts(": No such file or directory\n");
                return;
            }
        }
        
        if(namesize == targetsize){
            int i;
            for(i=0; i<namesize; i++){
                if(filename[i] != target[i])
                    break;
            }
            if(i == namesize){
                print_addr(base,namesize);
                return;
            }
        }
        base += 110;
        while(1){
            int i;
            for(i=0;i<6; i++){
                if(base[i] != magic[i])
                    break;
            }
            if(i == 6)
                break;
            base++;
        }
    }
    return;
}

void svc(void){
    asm volatile(
            "mov x0, 0x3c0\n\t"
            "msr spsr_el1, x0\n\t"
            "mov x0, #0x00E8\n\t"
            "movk x0, #0x800, lsl #16\n\t"
            "msr ELR_EL1, x0\n\t"
            "mov x0, 0x60000\n\t"
            "msr SP_EL0, x0\n\t"
            "eret\n\t"
            :
            :
            :);
    return;
}

void timer(void){
    
    //print now time 
    asm volatile(
            "mov x0, 0\n\t"
            "mrs x1, cntpct_el0\n\t"
            "mrs x2, cntfrq_el0\n\t"
            "bl print_time\n\t");
            
    //set timer
    asm volatile("bl core_timer_enable\n\t");
    
    return;
}


#define CORE0_TIMER_IRQ_CTRL 0x40000040

void setTimeout(char arg[][16]){
    char *data = arg[1];
    int after = str2int(arg[2], 10);
    //timer init
    asm volatile(
            "msr DAIFClr, 0xf\n\t" //still in el1
            "mov x0, 2\n\t"
            "ldr x1, =0x40000040\n\t"
            "str w0, [x1]\n\t"); // unmask timer interrupt
    add_timer(data,uart_puts,after);
    return;
}
