; Listen for serial input, echo back each character twice followed by a newline

.org $F000

start:
loop:
    lda $5002
    cmp #$01
    bne loop

getc:
    lda $5000
    sta $5001
    sta $5001
    lda #$0A
    sta $5001
    lda #$0D
    sta $5001

jmp loop
