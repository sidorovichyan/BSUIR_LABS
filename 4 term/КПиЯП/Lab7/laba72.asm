.model tiny
.code
    org 80h		
    cmd_length db ?	
    cmd_line db ?		
    org 100h			
start:
    cld
    mov bp,sp
    mov cl,cmd_length
    cmp cl,1         
    ;jle exit       
    
    mov cx,-1
    mov di,offset cmd_line    
find_param:         
    mov al,' '
    repz scasb
    dec di  
    push di
    inc word ptr argc
    mov si,di    
scan_params:
    lodsb          
    cmp al,0Dh     
    je params_ended
    cmp al,20h      
    jne scan_params 
    dec si
    mov byte ptr [si],0  
    mov di,si
    inc di
    jmp short next_param  
params_ended:
    dec si
    mov byte ptr [si],0 


next_param:       
    mov al,' '
    repe scasb
    dec di
    inc word ptr argc
    mov si, di     
    mov di, offset number
scan_param:
    cmp [si],0Dh     
    je param_ended
    cmp [si],20h      
    je param_ended
    movsb
    jmp scan_param
param_ended:
    mov byte ptr [si],0   

    mov si, offset number

string_to_num:
    xor dx,dx   
loop_:    
    xor ax,ax
    lodsb       
    test al,al 
    jz  ex
    cmp al,'9'  
    jnbe  loop_
    cmp al,'0'       
    jb    loop_
    sub ax,'0' 
    
    push ax
    mov ax, dx
    mov dx, 10
    mul dx
    mov dx, ax
    pop ax 
    add dx, ax  
    jmp  loop_
ex:     
    mov ax,dx   
    mov num, ax
    cmp num, 0
    jle er
    cmp num,255
    jg er
    mov sp, program_length+100H+200H
    
    mov ah, 4Ah
    stack_offset = program_length+ 100h + 200h
    mov bx, stack_offset shr 4 + 1     
    int 21h

    mov ax,cs
    mov word ptr EPB+4,ax  
    mov word ptr EPB+8,ax   
    mov word ptr EPB+0Ch,ax   

    mov cx, num    
cycle:

    mov ax,4B00h		
    mov dx, offset cmd_line+1	
    mov bx, offset EPB		
    int 21h  
      
    jnc next
er:    			
    mov ah,9
    lea dx, error
    int 21h
    int 20h
next:
    loop cycle

exit:
    int 20h

error db "error",10,13,'$'    
EPB dw 0000                  
dw offset commandline,0      
dw 005Ch,0,006Ch,0             
commandline db 125         
db " /?"                     
command_text db 122 dup(?)     
programm db 80 dup(0)  
number db 80 dup(0)
num dw 0
argc dw 0
program_length equ $-start     
end start