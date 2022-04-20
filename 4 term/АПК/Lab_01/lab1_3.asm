.model small
.stack 100h

.data

Information db "Byte sent: $"
Data_Byte db 'A'
Data_Byte2 db ?
           
.code

IsWrite_COM1 proc
    xor al,al
    mov dx,3FDh
    in al,dx
    ret
IsWrite_COM1 endp
           
Send_Byte proc
    mov dx,3F8h
    mov al,Data_Byte
    out dx,al
    ret
Send_Byte endp

IsRead_COM2 proc
    xor al,al
    mov dx,3FDh
    in al,dx
    ret
IsRead_COM2 endp

Read_Byte proc
    mov dx,2F8h
    in al,dx
    mov Data_Byte2,al
    ret
Read_Byte endp

Exit proc
    mov ax,4C00h
    int 21h
    ret
Exit endp

start:
    mov ax,@data
    mov ds,ax
    call IsWrite_COM1
    call Send_Byte
    mov al,2 
    call IsRead_COM2
    call Read_Byte
    mov dx,offset Information
    mov ah,9
    int 21h
    mov ah,02h
    mov dl,Data_Byte2
    int 21h 
    call Exit
end start