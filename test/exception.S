//save general registers to stack
.macro save_all
    sub sp, sp, 16 * 17
	mrs x0, sp_el0
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
	mrs x0, sp_el0
	mrs x1, elr_el1
	mrs x2, spsr_el1
    stp x30, x0 [sp, 16 * 15]
	stp x1, x2 [sp, 16* 16]
.endm

// load general registers from stack
.macro load_all
	ldp x30, x0 [sp ,16 * 15]
	ldp x1, x2 [sp, 16 * 16]
	msr sp_el0, x0
	msr elr_el1, x1
	msr spsr_el1, x2
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
    add sp, sp, 16 * 17
.endm
exception_handler_Sync1:
exception_handler_IRQ1:
exception_handler_FIQ1:
exception_handler_SError1:

exception_handler_Sync2:	
exception_handler_IRQ2:
    save_all
    mov x1, #0x00060
    movk x1, #0x4000, lsl #16
    ldr x0, [x1]
    bl IRQ_handler
    load_all
    eret
exception_handler_FIQ2:
exception_handler_SError2:

exception_handler_Sync3:
	save_all
   	bl SYNC_handler
	str x0, [sp, 16 * 15 + 8]
    load_all
	ldr x0, [sp, -8]
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

    //Exception from the current EL while using SP_ELx
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
