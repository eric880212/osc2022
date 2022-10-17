void thread_init(void);
void kernel_main(void);
void foo(void);
void thread_create(void (*func)(void));
int getpid(void);
void schedule(void);
void idle(void);
void exit(void);
void kill_zombies(void);

typedef struct Context{
	unsigned long x19;
	unsigned long x20;
	unsigned long x21;
	unsigned long x22;
	unsigned long x23;
	unsigned long x24;
	unsigned long x25;
	unsigned long x26;
	unsigned long x27;
	unsigned long x28;
	unsigned long fp;
	unsigned long lr;
	unsigned long sp; 
}context;

typedef struct Thread_struct{
	context ctx;
	int pid;
	char state; //run, terminate
	unsigned long stack;
	unsigned long kernel_stack;
	struct Thread_struct *next;
	struct Thread_struct *prev;
}thread_struct;

extern int pid;
extern thread_struct ts[20];
