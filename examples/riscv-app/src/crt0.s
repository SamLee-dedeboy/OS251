.section .init, "ax"
.global _start
_start:
    .cfi_startproc
    .cfi_undefined ra
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    la sp, __stack_top
    add s0, sp, zero
    jal ra, init
    nop
    jal ra, main
    .cfi_endproc


.section .text, "ax"
.global timer, get_controller_status, moveLargeSprite, my_printf, video, moveSmallSprite, thread_initialize, setBG_color
timer:
moveLargeSprite:
get_controller_status:
my_printf:
video:
moveSmallSprite:
thread_initialize:
setBG_color:
    ecall
    ret

    .end
