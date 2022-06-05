.model tiny 
.386
.data 

;file                       
file_name db 50 dup (0), '$'    
file_id dw 0000h    

new_file_name db "print.txt", 0    
new_file_id dw 0000h

read_bytes dw 0000h
processed_bytes_l dw 0000h 
processed_bytes_h dw 0000h 
size_of_string dw 0000h  
size_of_word dw 0000h  

;error messages
pathError        db "IO Error: cannot fild path!$" 
openedFilesError db "IO Error: too many opened files!$"     
accessError      db "IO Error: access error!$"       
IOError          db "IO Error!$" 
maxSizeOfWordError db "Error: word that you have entered is bigger than max size of word!$"
invalidIdentifierError db "IO Error: invalid identifier$"
;messages
hello_message         db "Press Enter to close programm: $"  
message_opening_file         db "Try to open file...!$"
message_opened_file      db "File has been opened!$" 
fileNotExistError    db "Error: empty command line!$"
;buffers                `
 

string db 50, 52 dup ('$')   
word_ db 50, 52 dup ('$')
file_buffer db 202 dup ('$')  

start_buffer_flag db 0      

messageOpenFile db "Open file...$"
messageParsing db "Parsing...$"
messageCloseFile db "Closing...$"   
messageDot db ".$"   

old_interrupt_offset     dw  0
old_interrupt_segment    dw  0      
old_interrupt dd 0     

screen_buffer db 202 dup ('$')
.code
org 100h

start:
mov si, 81h
mov di, 0
skip_space:
    mov al, es:[si]
    cmp al, 0dh
    je file_not_exist_error
    cmp al, 20h
    jne read_file_name
    inc si 
    jmp skip_space    
    read_file_name:
        mov file_name[di], al
        inc si
        mov al, es:[si]
        cmp al, 20h
        je _open_file
        cmp al, 0dh
        je _open_file
        inc di  
        jmp read_file_name
lea dx, messageOpenFile
call outputString      

_open_file:   

                                  
mov dx, offset message_opening_file  
call outputString 

call open_file
mov file_id, ax
 
mov dx, offset message_opened_file   
call outputString 
call save_interrupt
call set_new_interrupt    
  
reading:     
    call read_file
    mov ax, read_bytes   
    cmp ax, 0            
    je waiting
    mov si, offset file_buffer
    mov di, offset word_
    mov dx, offset file_buffer                      
    call outputStringWithoutNewLine 
    jmp reading
waiting:
    mov dx, offset hello_message 
    call outputString   
    jmp beskloop
beskloop:
    jmp beskloop    
ending:
    call startNewLine 
    lea dx, messageCloseFile
    call outputString 
    mov bx, file_id
    call close_file  
    call set_old_interrupt 
    int 20h

save_interrupt proc  
    push es 
    push ax
    push bx
    mov ah, 35h
    mov al, 09h 
    int 21h
    mov old_interrupt_segment, es 
    mov old_interrupt_offset, bx 
    mov word ptr old_interrupt, bx
    mov word ptr old_interrupt + 2, es
     
    pop bx  
    pop ax
    pop es
    ret
save_interrupt endp 
                                          
set_new_interrupt proc    
    cli   
    push ax
    push dx
    push es
       
    push ds   
    pop  es   
    
    mov dx, offset new_interrupt   
    mov ah, 25h                
    mov al, 09h 
    int 21h
    
    pop es
    pop ax
    pop dx 
    sti   
    
    ret
set_new_interrupt endp 

set_old_interrupt proc 
    cli
    push bx
    push es
    push ax  
    push ds   
    pop  es   ;es = ds
    mov dx, old_interrupt_offset
    mov ds, old_interrupt_segment 
    mov ah, 25h
    mov al, 09h
    int 21h
    pop ax
    pop es
    pop bx  
    sti 
    ret
set_old_interrupt endp

new_interrupt proc far        
   cli                  ; сброс IF
   pushf                ; push flags
   pusha                ; push registers          
    
   push ds
   push es
   
   push cs
   pop ds  
   

    in al, 60h         
    cmp al,19h          ; код P   
    je go_write
    
    cmp al, 1Ch         ; код Enter
    jne no_write
    mov al, 20h     
    out 20h, al 
    jmp ending


    go_write:
    call cs:create_new_file  


   push 0B800h 
   pop es ;es = B800h
   xor cx, cx 
   mov di, offset screen_buffer
   xor ax, ax
       
    character_recording:  
        cmp al, 200
        jae write_buffer ;>=                    
            
        cmp cx, 47FFh  
        jae exit_handler
        mov bx, cx
        push es:[bx]
        pop  cs:[di] ;cs:[di] = es:[bx]
            
        inc cx
        inc cx
        inc di 
        inc ax                            
            
        jmp character_recording 
            
    write_buffer:
        mov ax, 200
        call write_whole_buffer
        xor ax, ax
        mov di, offset screen_buffer                        
        jmp character_recording 
         
   exit_handler:
   mov ax, 200
   call write_whole_buffer
   
    mov bx, new_file_id
    xor ax, ax    
    mov ah, 3Eh    
    int 21h

   no_write: 
   pop es
   pop ds
   
   popa
   popf
   sti

   push ax
   mov al, 20h
   out 20h, al     
   pop ax
   iret
new_interrupt endp        
                                              

open_file proc
    push dx

    mov ax, 3D02h  
    lea dx, file_name  
    int 21h       
    jc io_error

    pop dx
    ret
open_file endp

read_file proc 
    push bx
    push cx
    push dx
    
    mov bx, file_id 
    mov ah, 3Fh           
    mov cx, 00C8h       
    mov dx, offset file_buffer
    int 21h   
    
    mov read_bytes, ax
    
    pop dx
    pop cx
    pop bx
    ret
read_file endp

close_file proc
    pusha
    
    mov bx, file_id
    xor ax, ax    
    mov ah, 3Eh    
    int 21h
    
    popa
    ret
close_file endp   

create_new_file proc
    pusha
        mov ah, 3Ch
        mov cx, 0 
        lea dx, new_file_name
        int 21h
        mov new_file_id, ax
        jc io_error       
    popa
    ret
create_new_file endp   

write_whole_buffer proc
    pusha
        lea dx, screen_buffer      
        mov bx, new_file_id
        mov cx, ax
        mov ah, 40h   
        int 21h
        jc io_error  ;if (cx != 0) io_error    
    popa
    ret  
write_whole_buffer endp    

io_error: 
    
    
    cmp ax, 0003h
    je cannot_find_path
    cmp ax, 0004h
    je too_many_opened_files
    cmp ax, 0005h
    je cannot_access
    cmp ax, 0006h
    je invalid_identifier
     
    mov dx, offset IOError
    call outputString   
    int 20h
     
    cannot_find_path: 
    mov dx, offset pathError
    call outputString   
    int 20h 
    
    too_many_opened_files:   
    mov dx, offset openedFilesError
    call outputString   
    int 20h  
    
    cannot_access:
    mov dx, offset accessError
    call outputString   
    int 20h    
    
    invalid_identifier:
    mov dx, offset invalidIdentifierError
    call outputString   
    int 20h    
    
    max_size_of_word_error:
    mov dx, offset maxSizeOfWordError  
    call outputString
    int 20h  
    
    file_not_exist_error:
     mov dx, offset  fileNotExistError  
    call outputString
    int 20h    
    call outputString
    int 20h  
    

outputString proc    
    push ax        
     
    mov AH, 09h
    int 21h  
    call startNewLine  
  
    pop ax
    ret              
outputString endp    
outputStringWithoutNewLine proc    
    push ax        
     
    mov AH, 09h
    int 21h  
    ;call startNewLine  
  
    pop ax
    ret              
outputStringWithoutNewLine endp  
                           
inputString proc  
    push ax       
                     
    mov AH, 0Ah
    int 21h          
    call startNewLine  
    
    pop ax
    ret      
inputString endp   
                              
startNewLine proc    
    pusha  
    
    mov DL, 0Dh
    mov Ah, 02h
    int 21h 
    
    mov DL, 0Ah
    mov Ah, 02h
    int 21h
    
    popa
    ret    
startNewLine endp                 

copyData proc  
    push ax  
      
    cmp cx, 0000h
    jz endCopyData   
    
    loop2:  
        mov ax, [si]
        mov [di], ax 
        inc si
        inc di
        loop loop2
   
    endCopyData:        
    pop ax  
ret
copyData endp  
end start