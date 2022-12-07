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
.global getTicks, getStatus, genRandom, setGraphicsMode, setTextMode, setColor, calcSmallSpriteControl, calcLargeSpriteControl, calcBackgroundControl
.global setSmallSpriteControl, setLargeSpriteControl, setBackgroundSpriteControl, shiftSmallSpriteControl, shiftLargeSpriteControl, 
.global getSmallSpriteControl, getLargeSpriteControl, getBackgroundSpriteControl
.global printLine, setBackgroundColor
.global InitContext, SwitchContext, getVideoInterruptCount
getTicks:
    li a5, 0
    ecall
getStatus:
    li a5, 1
    ecall
genRandom:
    li a5, 2
    ecall
setGraphicsMode:
    li a5, 3
    ecall
setTextMode:
    li a5, 4
    ecall
setColor:
    li a5, 5
    ecall
calcSmallSpriteControl:
    li a5, 6
    ecall
calcLargeSpriteControl:
    li a5, 7
    ecall
calcBackgroundControl:
    li a5, 8
    ecall
setSmallSpriteControl:
    li a5, 9
    ecall
setLargeSpriteControl:
    li a5, 10
    ecall
setBackgroundSpriteControl:
    li a5, 11
    ecall
shiftSmallSpriteControl:
    li a5, 12
    ecall
shiftLargeSpriteControl:
    li a5, 13
    ecall
getSmallSpriteControl:
    li a5, 14
    ecall
getLargeSpriteControl:
    li a5, 15
    ecall
getBackgroundSpriteControl:
    li a5, 16
    ecall
printLine:
    li a5, 17
    ecall
setBackgroundColor:
    li a5, 18
    ecall
InitContext:
    li a5, 19
    ecall
SwitchContext:
    li a5, 20
    ecall
getVideoInterruptCount:
    li a5, 21
    ecall
.end

