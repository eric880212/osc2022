#include "uart.h"
#include "command.h"
#include "reboot.h"
#include "string.h"
void help(){
    uart_puts("\rhelp\t: print this help menu\nhello\t: print Hello World!\nreboot\t: reboot the device\n");
}

void hello(){
    uart_puts("\rHello World!\n");
}

void reboot(){
    reset(10);
}

void ls(){
    char trailer[10] = "TRAILER!!!";
    char magic[6] = "070701";
    //QEMU loads the cpio archive file to 0x8000000 by default.
    //Then specify the name and loading address in config.txt eg."initramfs initramfs.cpio 0x20000000"
    char *base = (char *)0x8000000;
    while(1){
        char temp[8];
        //referenece:https://www.freebsd.org/cgi/man.cgi?query=cpio&sektion=5
        //namesize 
        for(int i=0; i<8; i++){
            temp[i] = base[i+94];
        }
        int namesize = str_hex2int(temp);
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
}
void print_data(char *base, int namesize){
    int pad = 4 - (110 + namesize) % 4;
    char *start = base + 110 + namesize + pad;
    char temp[8];
    for(int i=0;i<8; i++)
        temp[i] = base[i+54];
    int filesize = str_hex2int(temp)-1;
    char data[100] = "";
    for(int i=0; i<filesize; i++)
        data[i] = start[i];
    uart_puts("\r");
    uart_puts(data);
    uart_puts("\n");
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
        char temp[8];
        for(int i=0; i<8; i++){
            temp[i] = base[i+94];
        }
        int namesize = str_hex2int(temp)-1;
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
                print_data(base,namesize);
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
}
