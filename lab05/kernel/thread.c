#include "thread.h"
#include "uart.h"
#include "command.h"

void thread_init(){
	ts[0].pid = pid;
	ts[0].stack_top = 0x80000;
	ts[0].state = 'r';
	
	ts[0].next = &ts[0];
	ts[0].prev = &ts[0];
	asm volatile(
			"msr tpidr_el1, %0\n\t"
			:
			:"r"(&ts[0])
			:);
/*
	ts[1].pid = ++pid;
	unsigned long stack_top = palloc(1) + 0x1000;
	ts[1].stack_top = stack_top;
	ts[1].ctx.sp = stack_top;
	ts[1].ctx.fp =stack_top;
	ts[1].ctx.lr = (unsigned long)(idle);
	ts[1].state = 'r';

	ts[0].next = &ts[1];
	ts[0].prev = &ts[1];
	ts[1].next = &ts[0];
	ts[1].prev = &ts[0];
*/
	return;
}

void foo(){
    for(int i = 0; i < 8; ++i) {
		int pid;
		asm volatile(
				"mov x8, #0\n\t"
				"svc #0\n\t"
				"mov %0, x0\n\t"
				:"=r"(pid)
				:
				:"x8", "x0");
        printf("Thread id: %d %d\n", getpid(), i);
		//for(int i = 0; i < 100000000; i++)
			;
        schedule();
    }
	exit();
	return;
}

void kernel_main() {
    // ...
    // boot setup
    // ...
    for(int i = 0; i < 4; ++i)  // N should > 2
        thread_create(foo);
    idle();
}

void thread_create( void (*func)(void)){
	pid++;

	thread_struct *temp;
	asm volatile(
			"mrs %0, tpidr_el1\n\t"
			:"=r"(temp)
			:
			:);

	ts[pid].next = temp -> next;
	ts[pid].prev = temp;
	temp -> next -> prev = &ts[pid];
	temp -> next = &ts[pid];
	
	ts[pid].pid = pid;
	int stack_top = palloc(1) + 0x1000;
	ts[pid].stack_top = stack_top;
	ts[pid].ctx.sp = stack_top;
	ts[pid].ctx.fp = stack_top;
	ts[pid].ctx.lr = (unsigned long)func;
	ts[pid].state = 'r';

	asm volatile(
			"mov x1, %0\n\t"
			:
			:"r"(&ts[pid])
			:);

	get_current();	
 	switch_to();
	return;
}

int getpid(){
	thread_struct *temp;
	asm volatile(
			"mrs %0, tpidr_el1\n\t"
			:"=r"(temp)
			:
			:"memory");
	int pid = temp -> pid;
	return pid;
}


void schedule(){

	thread_struct *temp;
	asm volatile(
			"mrs %0, tpidr_el1\n\t"
			:"=r"(temp)
			:
			:"memory");	
	do{
		temp = temp -> next;
	}
	while(temp -> state != 'r');

	asm volatile(
			"mov x1, %0\n\t"
			:
			:"r"(temp)
			:);

	get_current();	
	switch_to();
}

void idle(){
	while(1){
		kill_zombies();
		schedule();
		//printf("schedule\n");
	}
}

void kill_zombies(){
	thread_struct *now, *temp;
	asm volatile(
			"mrs %0, tpidr_el1\n\t"
			:"=r"(now)
			:
			:"memory");
	temp = now -> next;
	while(temp != now){
		temp = temp -> next;
		if(temp -> state == 't'){
			temp -> prev -> next = temp -> next;
			temp -> next -> prev = temp -> prev;
		}
	}
	return;
}
int fork(){
	thread_create();	
}

void exit(){
	thread_struct *temp;
	asm volatile(
			"mrs %0, tpidr_el1\n\t"
			:"=r"(temp)
			:
			:"memory");
	temp -> state = 't';
	schedule();
	return;
}
