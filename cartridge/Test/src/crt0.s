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
    jal zero, main
    .cfi_endproc
    

.section .text, "ax"
.global getTicks, getStatus, setGraphicMode, setTextMode
getTicks:
    li a5, 0
    ecall
getStatus:
    li a5, 1
    ecall
setGraphicMode:
    li a4, 1
    ecall
setTextMode:
    li a4, 0
    ecall
.end