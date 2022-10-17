#include "uart.h"
#include "string.h"
void print_addr(char *base, int namesize){
    int pad = 4 - (110 + namesize) % 4;
    char *start = base + 110 + namesize + pad;
    char temp[9];
    temp[8] = '\0';
    for(int i=0;i<8; i++)
        temp[i] = base[i+54];
    int filesize = str2int(temp,16)-1;
    char data[100] = "";
    for(int i=0; i<filesize; i++)
        data[i] = start[i];
    uart_puts("\r");
    uart_puts(data);
    uart_puts("\n");
    return;
}

void print_spsr_el1(){
    int spsr_el1;
    asm volatile(
                "mrs %0, spsr_el1\n\t" : "=r"(spsr_el1) : : "memory"
            );
    uart_puts("spsr_el1:");
    uart_hex(spsr_el1);
    uart_puts("\n");
    return;
}

void print_elr_el1(){
    int elr_el1;
    asm volatile(
                "mrs %0, elr_el1\n\t" : "=r"(elr_el1) : : "memory"
            );
    uart_puts("elr_el1:");
    uart_hex(elr_el1);
    uart_puts("\n");
    return;
}

void print_esr_el1(){
    int esr_el1;
    asm volatile(
                "mrs %0, esr_el1\n\t" : "=r"(esr_el1) : : "memory"
            );
    uart_puts("esr_el1:");
    uart_hex(esr_el1);
    uart_puts("\n\n");
    return;
}

void print_time(int timeout, int now_time, int freq){
    //asm volatile("mrs x0, cntpct_el0");
    if(timeout){
        uart_puts("\nIt's time out!!! \n");
    }
    now_time /= freq;
    char temp[10];
    itoa(now_time, temp , 0);
    uart_puts(temp);
    uart_puts(" seconds after booting\n");
    if(timeout){
        uart_puts("#");
    }
    return;
}
