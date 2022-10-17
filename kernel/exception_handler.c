#include "setTimer.h"
#include "exception_handler.h"
#include "uart.h"
#include "thread.h"
void execute_callback(){
    uart_puts("\r");
    event[front].callback(event[front].msg);
    front++;

    //last one
    if(front > back){
        asm volatile(
                "mov x0, 0\n\t"
                "msr cntp_ctl_el0, x0\n\t":::"x0");
        return;
    }
    
    int cval = event[front].cval;
    asm volatile("msr cntp_cval_el0, %0"::"r"(cval):);

    return;
}
char c;
void IRQ_handler(unsigned int source){
    if((source & 0x02) == 0x02) 
        execute_callback();
    else if((source & 0x100) == 0x100){
        volatile unsigned int *AUX_MU_IIR_REG = (unsigned int *)0x3f215048;
        volatile unsigned int *AUX_MU_IO = (unsigned int *)0x3f215040;
        volatile unsigned int *AUX_MU_IER_REG = (unsigned int *)0x3f215044;
        if((*AUX_MU_IIR_REG & 4) == 4){
            //receiver valid
            c = (char)(*AUX_MU_IO);
            (*AUX_MU_IER_REG) = 2;
        } 
        else if((*AUX_MU_IIR_REG & 2) == 2){
            if(c == '\r'){
                (*AUX_MU_IO) = '\n';
                (*AUX_MU_IER_REG) = 0;
                asm volatile(
                        "msr DAIFSet, 0xf\n\t"
                        "b main\n\t");
            }
            //transmit empty
            (*AUX_MU_IO) = c;
            (*AUX_MU_IER_REG) = 1; 
        }
    }
    return;
}

void SYNC_handler(unsigned long long x0, unsigned long long x1){
	unsigned long long x8;
	asm volatile(
			"mov %0, x8\n\t"
			:"=r"(x8)
			:
			:"memory");
	switch(x8){
		case 0:
			return getpid();
		case 1:
			{
				char *buf = (char *)x0;
				int size = x1;
				int ret = 0;

				for(int i = 0; i < size; i++){
					char c = uart_getc();
					if(c){
						buf[ret++] = c;
					}
				}
				return ret;
			}
		case 2:
			{
				char *buf = (char *)x0;
				int size = x1;
				int ret = 0;
				for(int i = 0; i < size; i++){
					char c = buf[i];
					if(c){
						ret++;
						uart_send(c);
					}
				}
				return ret;
			}
	}


}

