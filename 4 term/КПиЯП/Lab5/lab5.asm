.model small
.stack 100h
.data

    filename db 80 dup(0)

    openString      db "Open the file ",13, 10, '$' 
    closeString     db "Close the file",13, 10, '$' 
    errorString     db "Error",13, 10, '$' 
    exitString      db "Exit",13, 10, '$' 
    newLine         db 13, 10, '$'
    filenameFinal db 'final.txt',0

    nschit db 1
    ndell  db ?

    flag dw 0

    buffer db 0

    handle dw 0
    handleFinal dw 0

.code

get_name proc
    push ax  
    push cx
    push di
    push si
    xor cx, cx
    mov cl, es:[80h]  
    cmp cl, 0
    je end_get_name
    mov di, 82h       
    lea si, filename
    cicle1:
    mov al, es:[di]  
    cmp al, 20h
    jne l1
    inc di
    mov al,es:[di]
    sub al, 30h
    mov ndell ,al
    jmp end_get_name  
    l1:
    cmp al, 0Dh      
    je end_get_name
    mov [si], al      
    inc di            
    inc si            
    jmp cicle1 
    end_get_name:   
    pop si           
    pop di
    pop cx
    pop ax   
    ret
get_name endp


outputString proc
    mov ah, 09h
    int 21h
    ret 
outputString endp 

outputChar proc
    mov ah,02h
    int 21h
    ret
outputChar endp

createFile proc
    mov ah,5Bh
    lea dx, filenameFinal
    int 21h
    jc error
    mov handleFinal,ax
    ret
createFile endp

createFinalFile proc
    mov ah, 3Ch
    mov dx, offset filenameFinal
    int 21h
    mov handleFinal, ax
    ret
createFinalFile endp

closeFile proc
    mov ah,3Eh
    int 21h
    jc error
    ret
closeFile endp

readByteFromFile proc
    mov ah, 3Fh
    mov bx, handle
    mov cx, 1
    lea dx, buffer
    int 21h
    jc error
    cmp ax,cx
    jl goexit
    ret
readByteFromFile endp
goexit: jmp exit

error:
    lea dx, errorString
    call outputString
    mov ah, 4ch
    int 21h

start:
    mov ax, @data
    mov ds, ax

    call get_name

    lea dx,openString
    call outputString


    mov ah, 3Dh
    mov al, 10000010b
    mov dx, offset filename
    int 21h
    jc error
    mov handle, ax

    mov ah,42h
    mov bx,handle
    mov cx,0
    mov dx,0
    mov al,0
    int 21h
    jc error

    mov ah, 3Ch
    mov cx,000000000b
    mov dx, offset filenameFinal
    int 21h
    mov handleFinal, ax
    jc error

    mov ah,42h
    mov bx,handleFinal
    mov cx,0
    mov dx,0
    mov al,0
    int 21h
    jc error

    mainLoop:
    call readByteFromFile
    xor dx,dx
    mov dl, buffer
    
    call outputChar

    cmp dl, 0Ah
    je cheking
    
    mov ax, flag
    cmp ax,0
    je write
    jmp mainLoop


write:
    mov ah, 40h
    mov bx, handleFinal
    mov cx,1
    xor dx,dx
    mov dx, offset buffer
    int 21h
    jc goerror
    jmp mainLoop
goerror: jmp error
cheking:
    cmp flag,1
    je gonext
    mov ah, 40h
    mov bx, handleFinal
    mov cx,1
    xor dx,dx
    mov dx, offset buffer
    int 21h
    jc goerror
    inc nschit
    mov al, nschit
    cmp al, ndell
    je changeFlag
    mov flag,0
    back:
    jmp mainLoop
gonext:
    mov flag, 0
    jmp mainLoop

changeFlag:
    mov flag,1
    mov nschit,1
    jmp back

gogoerror: jmp error
exit:
    xor bx,bx
    mov bx, handle
    call closeFile

    mov ah,3Ch
    lea dx, filename
    int 21h
    mov handle,ax
    jc goerror

    mov ah,42h
    mov bx,handle
    mov cx,0
    mov dx,0
    mov al,0
    int 21h
    jc goerror

    mov ah,42h
    mov bx,handleFinal
    mov cx,0
    mov dx,0
    mov al,0
    int 21h
    jc gogoerror

    lastloop:
    mov ah, 3Fh
    mov bx, handleFinal
    mov cx, 1
    lea dx, buffer
    int 21h
    jc gogoerror
    cmp ax,cx
    jl endend
    
    mov ah, 40h
    mov bx, handle
    mov cx,1
    xor dx,dx
    mov dx, offset buffer
    int 21h
    jc gogoerror
    jmp lastloop
    endend:

  
    xor bx,bx
    mov bx, handle
    call closeFile

    xor bx,bx
    mov bx, handleFinal
    call closeFile

    xor dx,dx
    lea dx, closeString
    call outputString

    xor dx,dx
    lea dx, exitString
    call outputString
    mov ah,41h
    mov dx, offset filenameFinal
    int 21h


    mov ah, 4ch
    int 21h
end start
