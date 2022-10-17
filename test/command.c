#include "uart.h"
#include "command.h"
#include "reboot.h"
#include "string.h"
#include "print.h"
#include "setTimer.h"
#include "node.h"
#include "math.h"
#include "thread.h"

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
    char trailer[11] = "TRAILER!!!";

    char magic[7] = "070701";
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
        char filename[100] = {0}; 
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
            "msr DAIFClr, 0xf\n\t"
            "mov x0, 2\n\t"
            "ldr x1, =0x40000040\n\t"
            "str w0, [x1]\n\t"); // unmask timer interrupt
    add_timer(data,uart_puts,after);
    return;
}

int palloc(int page_num){

    extern frame fm[0x10000];
    extern frame *fm_head[17];
    extern rsv *rsv_head; 
	
    int size = page_num * 4; //KB
    int block_size = 4; //KB 
    int exp, index;
/*
    uart_puts("\rIntial state: \n");
    print_fm();
*/
    for(int i=0;i<=16; i++){
        block_size = 4 * pow(2,i);
/*
        uart_puts("Checking 4kb * 2^");
        char temp[10] = {0};
        itoa(i,temp,0);
        uart_puts(temp);
 */
        if(block_size < size){
 //           uart_puts(" : Too small\n");
            continue;
        }
        else if(fm_head[i] == (frame *)0){
//            uart_puts(" : There isn't a free block exists.\n");
            continue;
        }
        else{
            exp = i;
//            uart_puts(" : Get block size : 4K * 2^");
            
            //char temp[10];
/*
            itoa(i,temp,0);
            uart_puts(temp);
            uart_puts(" bytes, Index is ");
 */
            index = fm_head[i]->index;
/*
            itoa(index, temp, 0);
            uart_puts(temp);
            uart_puts("\n");
 */
            break;
        }
    }

    fm_head[exp] = fm_head[exp]->next;
	if(fm_head[exp])
		fm_head[exp] -> prev = fm_head[exp];

    if(block_size/2 < size ){
        int start = index;
        int end = index + block_size/4 - 1;
        rsv *rsv_temp = rsv_head;
        while(rsv_temp){
            if(start <= rsv_temp->start_idx && end >= rsv_temp->start_idx)
               return palloc(size/4);
            if(start > rsv_temp->start_idx && end <= rsv_temp->end_idx)
                return palloc(size/4);
            rsv_temp = rsv_temp->next;
        } 
//        uart_puts("finish!\n"); 
//        print_fm();
        fm[index].value = (-1) * (block_size/4 );
		fm[index].prev = (frame *)0;
		fm[index].next = (frame *)0;
        for(int i=index + 1; i<index + block_size/4; i++){
            fm[i].value = -3;
        }
		int addr = index * 0x1000 + 0x10000000;
        return addr;
    }
    while(block_size/2 >= size){
//        uart_puts("\nToo big, release redundant memory block, \n");
        block_size /= 2;
		exp--;
        int temp_idx = index + pow(2,exp);
		fm_add(exp, temp_idx);
//        print_fm();
//        uart_puts("\n");
    }

    int start = index;
    int end = index + block_size/4 - 1;
    rsv *rsv_temp = rsv_head;
    while(rsv_temp){
        if(start <= rsv_temp->start_idx && end >= rsv_temp->start_idx)
            return palloc(size/4);
        if(start > rsv_temp->start_idx && end <= rsv_temp->end_idx)
            return palloc(size/4);
        rsv_temp = rsv_temp->next;
    }  

    fm[index].value = (-1) * ( block_size/4 );
    //if find it then delete and return 0, if can't find directly return -1
	fm[index].next = (frame *)0;
	fm[index].prev = (frame *)0;
    for(int i = index+1; i < index + block_size/4; i++){
        fm[i].value = -3;
	}
   	int addr = index * 0x1000 + 0x10000000;
    return addr;  
}

void freepage(int index){
    extern frame fm[0x10000];

    if(fm[index].value >= 0 || fm[index].value == -3 || fm[index].value== -5){
        uart_puts("wrong index\n");
        return;
    }
    int frame_num = (-1) * fm[index].value;
    int exp = log(2,frame_num);   

    while(1){
        int buddy = index ^ frame_num;
/*
        uart_puts("It's buddy is index ");
        char temp[10];
        itoa(buddy,temp,0);
        uart_puts(temp);
        uart_puts("\n");
*/
		if(fm[buddy].value >= 0 && frame_num < pow(2, 16)){
			fm_delete(exp, buddy);
        	exp++;
            frame_num *= 2;
            index = min(index, buddy);
//            uart_puts("Merge the buddy success.\n");
//            print_fm();
		}
		else{
//            uart_puts("Fail to merge the buddy.\n");
			fm_add(exp, index);
//			print_fm();
			break;
		}
    }
    
    return;
}

fs fs_pool[1000];
int fs_pool_p = 0;
int malloc_size[0x10000];

void* malloc(int size){  
    extern long long int malloc_start;
    extern fs* fs_head[5];
    int t = size/16 - 1;
    if(fs_head[t]){
        
        void *addr = fs_head[t]->addr;
        fs_head[t] = fs_head[t]->next;
        return addr;
    }
    else{
        uart_puts("There isn't free chunk memory.So, palloc(4).\n");
        long long int index = palloc(1);
        long long int base = index * 4096 + malloc_start;
        malloc_size[index] = size; 
        uart_puts("Put them into linked list fs.\n");
        for(int i=0; i<4096; i += size){
            long long int addr = base + i;
            fs_pool[fs_pool_p].addr = (void *)addr;
            fs_add(t,&fs_pool[fs_pool_p++]);
        }
        void *temp = fs_head[t]->addr;
        fs_head[t] = fs_head[t]->next;
        return temp;
   }
}
void free(void *addr){
    uart_puts("Put it back into linked listi fs.\n");
    extern long long int malloc_start;
    int index = ((long long int)addr - malloc_start) / 4096;
    int size = malloc_size[index];
    int t = size/16 - 1;
    fs_pool[fs_pool_p].addr = addr;
    fs_add(t,&fs_pool[fs_pool_p++]);
    
    return;
}

rsv rsv_pool[1000];
int rsv_pool_p = 0;
void reserve(int start, int end){
    extern long long int  malloc_start;
    int start_idx = (start - malloc_start) >> 12;
    int end_idx = (end - malloc_start) >> 12;
    
    rsv_add(start_idx, end_idx);
    return;
}
void async_IO(){
    uart_puts("\r");
    volatile unsigned int *AUX_MU_IER_REG = (unsigned int *)0x3f215044;
    volatile unsigned int *sec_IRQ_1 = (unsigned int *)0x3f00b210;
    *AUX_MU_IER_REG = 1;
    *sec_IRQ_1 |= (1 << 29); 
	asm volatile("msr DAIFClr, 0xf\n\t");
	//uart_puts("test\n");
	while(1){}
	return;
}

void test_kernel(){
	kernel_main();
	return;
}
