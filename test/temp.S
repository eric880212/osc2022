#define CORE0_TIMER_IRQ_CTRL 0x40000040

.global timer_init
timer_init:
    msr DAIFClr, 0xf
    
    mrs x0, cntfrq_el0
    mov x1, 2
    mul x0, x0, x1
    mrs x1, cntpct_el0
    add x0, x0, x1
    msr cntp_cval_el0, x0 // set expired time
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable   
    msr elr_el1, lr
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1] // unmask timer interrupt
    br lr

