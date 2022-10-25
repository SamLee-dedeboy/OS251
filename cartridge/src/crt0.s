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
.global getTicks, getStatus, setGraphicsMode, setTextMode 
getTicks:
    li a5, 0
    ecall
getStatus:
    li a5, 1
    ecall
setGraphicsMode:
    li a5, 2
    ecall
setTextMode:
    li a5, 3
    ecall
.end