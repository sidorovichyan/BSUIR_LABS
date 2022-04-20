.model tiny     
.code   
org 100h  
start: 
    mov ah,9    
    mov dx,offset message
    int 21h   
    mov dx, offset m1
    mov ax, offset message
    sub dx,ax 
    ret     
message db "Hello World!",0Dh,0Ah,'$' 
m1 db "."
end start