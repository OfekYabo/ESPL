section .data
    hello db 'hello world', 0xA  ; Define the string with a newline character
    hello_len equ $ - hello      ; Calculate the length of the string
    sys_write equ 4              ; System call number for sys_write
    sys_exit equ 1               ; System call number for sys_exit
    stdout equ 1                 ; File descriptor for stdout

section .text
global _start

_start:
    ; Write the string to stdout
    mov eax, sys_write  ; syscall number for sys_write
    mov ebx, stdout     ; file descriptor 1 is stdout
    mov ecx, hello      ; pointer to the string
    mov edx, hello_len  ; length of the string
    int 0x80            ; call kernel

    ; Exit the program
    mov eax, sys_exit   ; syscall number for sys_exit
    xor ebx, ebx        ; exit code 0
    int 0x80            ; call kernel