//save general registers to stack
.macro save_all
    sub sp, sp, 32 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
.endm

// load general registers from stack
.macro load_all
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.endm
exception_handler_Sync1:
exception_handler_IRQ1:
    save_all
    mov x0, 1
    mrs x1, cntpct_el0
    mrs x2, cntfrq_el0
    bl print_time
    mov x0, 0
    msr cntp_ctl_el0, x0
    load_all
    eret
exception_handler_FIQ1:
exception_handler_SError1:

exception_handler_Sync2:
exception_handler_IRQ2:
    save_all
    bl execute_callback
    /* 
    mov x0, 0
    msr cntp_ctl_el0, x0
    */
    load_all
    eret
exception_handler_FIQ2:
exception_handler_SError2:

exception_handler_Sync3:
    save_all
    bl print_spsr_el1
    bl print_elr_el1
    bl print_esr_el1
    load_all
    eret
exception_handler_IRQ3:
    save_all
    mov x0, 1
    mrs x1, cntpct_el0
    mrs x2, cntfrq_el0
    bl print_time
    mov x0, 0
    msr cntp_ctl_el0, x0  //timer disable
    load_all
    eret
exception_handler_FIQ3:
exception_handler_SError3:

exception_handler_Sync4:
exception_handler_IRQ4:
exception_handler_FIQ4:
exception_handler_SError4:

.align 11 // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:
    //Exception from the curentEL while using SP_EL0
    b exception_handler_Sync1 // branch to a handler function.
    .align 7// entry size is 0x80, .align will pad 0
    b exception_handler_IRQ1
    .align 7
    b exception_handler_FIQ1
    .align 7
    b exception_handler_SError1
    .align 7

    //Exception from the current EL whike using SP_ELx
    b exception_handler_Sync2
    .align 7
    b exception_handler_IRQ2
    .align 7
    b exception_handler_FIQ2
    .align 7
    b exception_handler_SError2
    .align 7

    
    //Exception from a lower EL and all lower ELs are AARCH64
    b exception_handler_Sync3
    .align 7
    b exception_handler_IRQ3
    .align 7
    b exception_handler_FIQ3
    .align 7
    b exception_handler_SError3
    .align 7

    //Exception from a lower EL and all lower ELs are AARCH32
    b exception_handler_Sync4
    .align 7
    b exception_handler_IRQ4
    .align 7
    b exception_handler_FIQ4
    .align 7
    b exception_handler_SError4
    .align 7

1: wfe
.global set_exception_vector_table
set_exception_vector_table:
    adr x0, exception_vector_table
    msr vbar_el1, x0
    //jump to C code, should not return
    bl      main
    //for failsafe, halt this core too
    b       1b
