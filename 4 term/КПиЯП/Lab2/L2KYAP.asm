.model small
.stack 100h
.data
message_input  db "Enter a line for sorting: $"
message_output db "Result: $"
message_error  db "Error! $"
message_source db "Your line: $"
endline        db 10, 13, '$'

size1 equ 202
inputsize equ 200
line db size1 DUP('?')

.code
output macro str 
    mov ah, 9
    mov dx, offset str
    int 21h
endm

input macro str 
    mov ah, 0Ah
    mov dx, offset str
    int 21h
endm 

start:         
    mov ax, @data
    mov ds, ax
    mov es, ax   
    mov ax,3
    int 10h
    output message_input 
    mov cx,inputsize
    xor ah,ah
loopinput:
    mov ah,1
    int 21h
    mov line[si],al
    inc si
    cmp al,0Dh
    je gome
loopne loopinput
    gome:
    mov line[si], 0Dh
    mov line[si+1], '$'
    xor si,si
    xor cx,cx
    lea si, line
    jmp check_loop
edn:
    jmp error_end
str5:
    jmp main_loop
main_loop:         
    xor si, si
    xor di, di
    xor ax, ax
    xor dx, dx    
    mov si, offset line     
first_word:       
    cmp byte ptr[si], 9
    je edn
    cmp byte ptr[si], 32 
    jne check_compare 
    inc si
    cmp byte ptr[si], 13
    je goend          
    jmp first_word
goend:
    jmp the_end     
loop_per_line:
    inc si
    cmp byte ptr[si], ' '
    je check_whitespace ;???? ??????
    cmp byte ptr[si], 13 
    jne loop_per_line 
    cmp ax, 0
    jne main_loop
    jmp the_end ;???? ????? ??????, ?? ?????
       
check_compare:
    cmp dx, 0
    jne compare ;???? ???? ??? ?????, ?? ??????????
    push si ;??????? ????? ??????? ????? ? ????
    mov dx, 1 
    jmp loop_per_line
check_whitespace:
    cmp byte ptr[si+1], ' '
    je loop_per_line ;???? ????????? ????????, ??? ??????
    inc si ;????? ??????? ?????
    jmp check_compare 
compare:
    pop di ;????????? ? es:di ????? ??????? ?????    
    push si ;???????? ? ???? ????? ??????? ? ??????? ????? 
    push di    
    mov cx, si
    sub cx, di
    repe cmpsb ;?????????? ???? ??????? ?????   
    dec si
    dec di
    xor bx, bx
    mov bl,byte ptr[di] 
    cmp bl, byte ptr[si] 
    jg change ;???????????? ???? ?????? ????? > ???????
    pop di
    pop si
    push si 
    jmp loop_per_line
change:
    inc al
    pop di
    pop si
    xor cx, cx
    xor bx, bx
    mov dx, si 
loop1: 
    dec si
    inc cx
    cmp byte ptr [si-1], ' '
    je loop1   
loop2:
    dec si
    mov bl, byte ptr [si] 
    push bx 
    inc ah 
    cmp si, di
    jne loop2
    mov si, dx  
loop3:  
    cmp byte ptr [si], 13
    je loop4
    mov bl, byte ptr [si]
    xchg byte ptr [di], bl
    inc si
    inc di
    cmp  byte ptr [si], ' '
    jne loop3
loop4:
    mov byte ptr[di], ' '
    inc di
    loop loop4   
    mov si, di
    mov dx, si
    dec si
loop5: 
    inc si
    cmp byte ptr[si], 13
    je goMainLoop   
    pop bx
    mov byte ptr[si], bl
    dec ah
    cmp ah, 0
    je loop6
    jmp loop5    
loop6:
    push dx
    mov dx, 1
    xor cx, cx
    jmp loop_per_line 
goMainLoop:
    jmp main_loop    
check_loop:      
     cmp [si], 9
     je tab_to_space
     inc si      
     cmp [si], '$'
     jne check_loop
     jmp str5             
tab_to_space:
    mov [si], 32
    jmp check_loop                  
error_end:   
    mov ax, 3
    int 10h
    output message_error
    mov ah, 4Ch
    int 21h
    jmp endend        
the_end:   
    output endline
    output message_output 
    mov ah, 9
    mov dx, offset line 
    int 21h
    mov ah, 4Ch
    int 21h
    jmp endend  
endend:
end start