.model small
.stack 100h
.data           
.code

jmp start

Init_COM1 proc
   xor ax,ax 
   mov al,10100011b
   mov dx,0
   int 14h
   ret            
Init_COM1 endp

Write_COM1 proc
   mov al,'A'
   mov ah,1
   mov dx,0
   int 14h
   ret 
Write_COM1 endp

Read_COM2 proc
    mov ah,2
    mov dx,1
    int 14h
    ret
Read_COM2 endp

Output proc
   mov ah,02h
   mov dl,al
   int 21h
   ret
Output endp

Exit proc
    mov ax,4C00h
    int 21h
    ret
Exit endp

start:
   call Init_COM1
   call Write_COM1
   call Read_COM2       
   call Output
   call Exit
end start