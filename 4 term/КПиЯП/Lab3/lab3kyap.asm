.model small
.stack 100h
.data
  
msg0 db "note: calculator works with integer values only.  ",0Dh,0Ah,'$'
msg1 db  0Dh,0Ah, 'enter first number: $'
msg3 db "enter second number: $"
error db  'error overflow$' 
errorDeviZero db  'Exeption. Devide by zero!$' 
summ db  0dh,0ah , 'Sum: $' 
divv db  0dh,0ah , 'Div: $' 
mull db  0dh,0ah , 'Mul: $' 
subb db  0dh,0ah , 'Sub: $'  
andd db  0dh,0ah , 'And: $' 
orr db  0dh,0ah ,  'Orr: $' 

numb1LAref db 0dh,0ah , 'Numb1 to left arefm : $' 
numb1LLog db  0dh,0ah , 'Numb1 to left logic : $' 
numb1RAref db 0dh,0ah , 'Numb1 to right aref :   $' 
numb1RLog db  0dh,0ah , 'Numb1 to right logic:  $'

numb2LAref db  0dh,0ah ,  'Numb2 to left arefm : $' 
numb2LLog db  0dh,0ah ,  'Numb2 to left logic : $' 
numb2RAref db 0dh,0ah , 'Numb2 to right arefm : $' 
numb2RLog db  0dh,0ah ,  'Numb2 to right logic: $'

endline        db 10, 13, '$'

num1 dw ?
num2 dw ?  
.code

PUTC    MACRO   char
        PUSH    AX
        MOV     AL, char
        MOV     AH, 0Eh
        INT     10h     
        POP     AX
ENDM

output macro str ;âûâîä ñòðîêè
    mov ah, 9
    mov dx, offset str
    int 21h
endm

WriteNum proc
        xor di,di
        xor ax,ax   ;
        xor bx,bx   ;   clearing
        xor cx,cx   ;
        mov bx,10   ;use for dividing by 10
        StartLoopEnter:
            mov ah,1
            int 21h
            xor ah,ah
            ;checker 'enter' and letters+
            cmp al,0Dh
            je OutFunWrite
            cmp al,'-'
            je MinusTrue
            cmp al, '0'
            jl OutFunWrite
            cmp al,'9'
            jg OutFunWrite
            jmp ContLoopEnter
            MinusTrue:
            cmp dx,1    ;if minus was already written we leave function as letter error
            je RetFunWrite
            mov dx,1    ;otherwise we set flag to make number negative in future
            jmp StartLoopEnter
            ContLoopEnter:
            ;checker 'enter' and letters-
                ;additional check of overflow
                cmp di,4
                jne ContinueAddingNum
                sub al,30h
                cmp dx,1
                je NegativeChecker
                cmp al,7
                jg CallBuffOverflow
                add al,30h
                jmp ContinueAddingNum
                NegativeChecker:
                cmp al,8
                jg CallBuffOverflow
                add al,30h
                jmp ContinueAddingNum
            ContinueAddingNum:
            xchg ax,cx  ;because mul command multiply number in ax, we swap two atributes
            push dx     ;remember number in dx (negative flag)
            imul bx     ;multiply by 10 (actually there is no need in imul, we could use just mul)
            pop dx      ;returning negative flag
            jo  CallBuffOverflow
            xchg ax,cx  ;returning normal number in cx
            sub al,30h  
            add cx,ax   ;adding number to cx
            inc di
            jmp StartLoopEnter
    OutFunWrite:
        cmp dx,1        ;if negative flag was set
        jne RetFunWrite
        xor dx,dx
        neg cx          ;we make negative num
    RetFunWrite:
        ret
    CallBuffOverflow:
        mov ax,3
        int 10h
        call pereporn
        jmp exit
        ret
    WriteNum endp

start:
mov ax, @data
mov ds, ax
mov es, ax

mov ax, 3
int 10h 

output msg0
output msg1 
call WriteNum
mov num1, cx 
output endline 
mov ax,num1
call OutBin
output endline 
xor ax,ax
output msg3
call WriteNum
mov num2, cx 
output endline 

mov ax,num2
call OutBin
backOut:
output endline 
xor ax,ax

output summ
jmp do_plus
back1:
output subb
jmp do_minus
back2:
output mull
jmp do_mult 
back3:
output divv
jmp do_div
back4:
output andd
jmp do_END
back5:
output orr
jmp do_OR
back6:
output endline 
jmp do_sdvig_n1
back7:
output endline 
jmp do_sdvig_n2
back8:
output endline 
jmp exit
exit:
mov ah, 4Ch
int 21h
jmp endend


do_sdvig_n2:
output numb2LAref
mov ax,num2
sal ax,1
call OutBin

output numb2LLog
mov ax,num2
shl ax, 1
call OutBin

output numb2RAref
mov ax,num2
sar ax, 1
call OutBin

output numb2RLog
mov ax,num2
shr ax, 1
call OutBin
jmp back8


do_sdvig_n1:
output numb1LAref
mov ax,num1
sal ax,1
call OutBin

output numb1LLog
mov ax,num1
shl ax, 1
call OutBin

output numb1RAref
mov ax,num1
sar ax, 1
call OutBin

output numb1RLog
mov ax,num1
shr ax, 1
call OutBin
jmp back7



do_plus:
mov ax, num1
add ax, num2
jno noerrodPLUS
mov ah,3
int 10h
call pereporn
jmp exit
noerrodPLUS:
call print_num
jmp back1

do_minus:
mov ax, num1
sub ax, num2
call print_num
jmp back2

do_mult:
mov ax, num1
imul num2
jno noplerror2
call pereporn
jmp back3
noplerror2:
call print_num
jmp back3
call print_num    
jmp back3

do_div:
mov ax, num1 
cwd
mov bx,num2

cmp bx, 0 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
jne noerror
output errorDeviZero
jmp back4
noerror:
idiv bx  ; ax = (dx ax) / num2.
jnc noplerror3
call pereporn
jmp back4
noplerror3:
call print_num
jmp back4

do_END:
mov ax, num1
mov bx, num2
and ax, bx
call OutBin    ; print ax value.
jmp back5

do_OR:
mov ax,num1
mov bx,num2
or ax,bx
call OutBin
jmp back6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; these functions are copied from emu8086.inc ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pereporn PROC
lea dx, error
mov ah, 09h      ; output string at ds:dx
int 21h 
ret
pereporn ENDP

OutBin PROC    NEAR
    mov bx,ax
    mov cx,16
ob1:
    shl bx,1
    jc ob2
    
    mov dl,'0'
    jmp ob3
    
ob2:
    mov dl,'1'
ob3:
    mov ah,2
    int 21h
    loop ob1   
    ret
OutBin ENDP
PRINT_NUM       PROC    NEAR
        PUSH    DX
        PUSH    AX

        CMP     AX, 0
        JNZ     not_zero

        PUTC    '0'
        JMP     printed

not_zero:
        ; the check SIGN of AX,
        ; make absolute if it's negative:
        CMP     AX, 0
        JNS     positive
        NEG     AX

        PUTC    '-'

positive:
        CALL    PRINT_NUM_UNS
printed:
        POP     AX
        POP     DX
        RET
PRINT_NUM       ENDP
PRINT_NUM_UNS   PROC    NEAR
        PUSH    AX
        PUSH    BX
        PUSH    CX
        PUSH    DX

        ; flag to prevent printing zeros before number:
        MOV     CX, 1

        ; (result of "/ 10000" is always less or equal to 9).
        MOV     BX, 10000       ; 2710h - divider.

        ; AX is zero?
        CMP     AX, 0
        JZ      print_zero

begin_print:

        ; check divider (if zero go to end_print):
        CMP     BX,0
        JZ      end_print

        ; avoid printing zeros before number:
        CMP     CX, 0
        JE      calc
        ; if AX<BX then result of DIV will be zero:
        CMP     AX, BX
        JB      skip
calc:
        MOV     CX, 0   ; set flag.

        MOV     DX, 0
        DIV     BX      ; AX = DX:AX / BX   (DX=remainder).

        ; print last digit
        ; AH is always ZERO, so it's ignored
        ADD     AL, 30h    ; convert to ASCII code.
        PUTC    AL


        MOV     AX, DX  ; get remainder from last div.

skip:
        ; calculate BX=BX/10
        PUSH    AX
        MOV     DX, 0
        MOV     AX, BX
        DIV     CS:ten  ; AX = DX:AX / 10   (DX=remainder).
        MOV     BX, AX
        POP     AX

        JMP     begin_print
        
print_zero:
        PUTC    '0'
        
end_print:

        POP     DX
        POP     CX
        POP     BX
        POP     AX
        RET
PRINT_NUM_UNS   ENDP
ten             DW      10      ; used as multiplier/divider by SCAN_NUM & PRINT_NUM_UNS.
GET_STRING      PROC    NEAR
PUSH    AX
PUSH    CX
PUSH    DI
PUSH    DX

MOV     CX, 0                   ; char counter.

CMP     DX, 1                   ; buffer too small?
JBE     empty_buffer            ;

DEC     DX                      ; reserve space for last zero.


wait_for_key:

MOV     AH, 0                   ; get pressed key.
INT     16h

CMP     AL, 0Dh                  ; 'RETURN' pressed?
JZ      exit_GET_STRING


CMP     AL, 8                   ; 'BACKSPACE' pressed?
JNE     add_to_buffer
JCXZ    wait_for_key            ; nothing to remove!
DEC     CX
DEC     DI
PUTC    8                       ; backspace.
PUTC    ' '                     ; clear position.
PUTC    8                       ; backspace again.
JMP     wait_for_key

add_to_buffer:

        CMP     CX, DX          ; buffer is full?
        JAE     wait_for_key    ; if so wait for 'BACKSPACE' or 'RETURN'...

        MOV     [DI], AL
        INC     DI
        INC     CX
        
        ; print the key:
        MOV     AH, 0Eh
        INT     10h

JMP     wait_for_key
;============================

exit_GET_STRING:

; terminate by null:
MOV     [DI], 0

empty_buffer:

POP     DX
POP     DI
POP     CX
POP     AX
RET
GET_STRING      ENDP
endend:
ret
end start

