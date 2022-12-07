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
.global getTicks, getStatus, printHelloWorld, controlLargeSprite, controlBackground
.global writeToMemoryWithIndex, backgroundPaletteMemory, spritePaletteMemory, transparentSpritePalette
.global setBackgroundDataImage, setDisplayMode, setLargeSpriteDataImage
getTicks:
    li a5, 0
    ecall
getStatus:
    li a5, 1
    ecall
printHelloWorld:
    li a5, 2
    ecall
controlLargeSprite:
    li a5, 3
    ecall
controlBackground:
    li a5, 4
    ecall
writeToMemoryWithIndex:
    li a5, 5
    ecall
backgroundPaletteMemory:
    li a5, 6
    ecall
spritePaletteMemory:
    li a5, 7
    ecall
transparentSpritePalette:
    li a5, 8
    ecall
setBackgroundDataImage:
    li a5, 9
    ecall
setDisplayMode:
    li a5, 10
    ecall
setLargeSpriteDataImage:
    li a5, 11
    ecall
.end