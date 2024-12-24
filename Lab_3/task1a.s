section .data
    newline db 0xA  ; Newline character

section .bss
    buffer resb 128  ; Buffer to store arguments

section .text
global _start
extern system_call

_start:
    ; Save argc and argv
    pop ecx          ; ecx = argc
    mov esi, esp     ; esi = argv

print_args:
    ; Check if there are no more arguments
    cmp ecx, 0
    je exit_program

    ; Load the next argument
    lodsd            ; eax = [esi], esi += 4
    mov ebx, eax     ; ebx = pointer to argument

print_arg:
    ; Check if we reached the end of the argument
    cmp byte [ebx], 0
    je print_newline

    ; Print the current character
    mov eax, 4       ; syscall number for sys_write
    mov edi, 1       ; file descriptor 1 is stdout
    mov ecx, ebx     ; pointer to the character
    mov edx, 1       ; number of bytes to write
    int 0x80         ; call kernel

    ; Move to the next character
    inc ebx
    jmp print_arg

print_newline:
    ; Print a newline character
    mov eax, 4       ; syscall number for sys_write
    mov edi, 1       ; file descriptor 1 is stdout
    lea ecx, [newline]
    mov edx, 1       ; number of bytes to write
    int 0x80         ; call kernel

    ; Move to the next argument
    dec ecx
    jmp print_args

exit_program:
    ; Exit the program
    mov eax, 1       ; syscall number for sys_exit
    xor ebx, ebx     ; exit code 0
    int 0x80         ; call kernel