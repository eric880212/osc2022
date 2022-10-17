#include "setTimer.h"
#include "uart.h"
#include "string.h"
//struct Timeout event[20];
int back = -1, front = 0;

void add_timer(char *data, void (*callback)(char *),int after){
    back++;
    long int cval;
    asm volatile(
            "mrs x0, cntfrq_el0\n\t"
            "mul x0, x0, %1\n\t"
            "mrs x1, cntpct_el0\n\t"
            "add x0, x0, x1\n\t"
            "mov %0, x0\n\t"
            :"=r"(cval)
            :"r"(after)
            :"x0", "x1", "memory");

    //put it at the end
    for(int i=0;i<strlen(data); i++){
        event[back].msg[i] = data[i];
    }
    event[back].callback = callback;
    event[back].cval = cval;

    //first timer
    if(front == back){
        asm volatile(
                "msr cntp_cval_el0, %0\n\t"
                :
                :"r"(cval)
                :);
        asm volatile(
                "mov x0, 1\n\t"
                "msr cntp_ctl_el0, x0\n\t"
                :
                :
                :"x0"); 
        return;
    }

    //sort it from the end
    for(int i = back-1; i >= front ; i--){
        if(cval < event[i].cval){
            for(int j=0; j < strlen(event[i].msg); j++)
                event[i+1].msg[j] = event[i].msg[j];
            event[i+1].callback = event[i].callback;
            event[i+1].cval = event[i].cval;

            if(i == front){
                for(int j=0; j<strlen(data); j++)
                    event[front].msg[j] = data[j];
                event[front].callback = callback;
                event[front].cval = cval;
                asm volatile(
                        "msr cntp_cval_el0, %0\n\t"
                        "mov x0, 1\n\t"
                        "msr cntp_ctl_el0, x0\n\t"
                        :
                        :"r"(cval)
                        :"x0");
            }
        }
        else{
            for(int j=0; j<strlen(data); j++)
                event[i+1].msg[j] = data[j];
            event[i+1].callback = callback;
            event[i+1].cval = cval;
        }
    }


/*
    if(!event[front].callback){
        for(int i=0;i<strlen(data); i++){
            event[back].msg[i] = data[i];
        }
        event[back].callback = callback;
        asm volatile(
                "msr cntp_cval_el0, %0\n\t"
                "mov x0, 1\n\t"
                "msr cntp_ctl_el0, x0\n\t"
                :
                :"r"(cval)
                :"x0");

        event[back].cval = cval;
        return;
    }
    
    for(int i=front;i<back; i++){
       if(cval < event[i].cval){
            
            break;
       }
    }

*/
    return;
}
/*
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
*/
