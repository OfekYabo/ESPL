section .data
    newline db 0xA
    sys_exit equ 1
    sys_read equ 3
    sys_write equ 4
    sys_open equ 5
    stdin equ 0
    stdout equ 1
    stderr equ 2
    O_RDONLY equ 0
    O_WRONLY equ 1
    O_CREAT equ 64
    O_TRUNC equ 512
    Infile dd stdin
    Outfile dd stdout

section .bss
    buffer resb 1

section .text
global _start
global system_call
global main
extern strlen
extern strcmp
extern strncmp

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

main: ; int main(int argc, char *argv[])
; Print all command-line arguments and encode input from stdin to stdout.
    
    mov ecx, [esp + 4]      ; get argc
    mov esi, [esp + 8]      ; get argv

    push esi                ; save argv
    push ecx                ; 
    jmp parse_args         ; parse command-line arguments
    main_1:
        pop ecx
        pop esi

        push esi                ; save argv
        push ecx                ; save argc
        jmp print_args         ; print all command-line arguments
    main_2:
        pop ecx
        pop esi
        jmp encode_input
    ; Exit the program
    main_exit:
        mov eax, 0             ; return 0 (success)
        ret

parse_args: ; void parse_args(int argc, char *argv[])
; Parse command-line arguments for -i and -o options
    parse_args_loop:
        ; Check if there are no more arguments
        cmp ecx, 0
        je main_1

        mov ebx, [esi]         ; get argv[i]
        add esi, 4              ; esi += 4 (next argument)
        dec ecx                 ; decrement argc

        cmp byte [ebx], "-"            ; check if the argument starts with '-'
        jne parse_args_loop
        mov eax, sys_open      ; sys_open
        
        parse_i:
            cmp byte [ebx + 1], "i"        ; check if the argument is -i
            jne parse_o

            add ebx, 2              ; skip -i
            push ecx               ; save ecx
            mov ecx, O_RDONLY      ; read-only mode
            int 0x80               ; call kernel
            pop ecx                ; restore ecx
            mov [Infile], eax      ; store the file descriptor in Infile
            jmp parse_args_loop     ; loop

        parse_o:
            cmp byte [ebx + 1], "o"        ; check if the argument is -o
            jne parse_args_loop

            add ebx, 2              ; skip -o
            push ecx               ; save ecx
            mov ecx, O_WRONLY | O_CREAT | O_TRUNC ; write-only, create, truncate
            mov edx, 0666          ; file permissions
            int 0x80               ; call kernel
            pop ecx                ; restore ecx
            mov [Outfile], eax      ; store the file descriptor in Infile
            jmp parse_args_loop     ; loop

print_args: ; void print_args(int argc, char *argv[])
; Print all command-line arguments.
    print_args_loop:
        ; Check if there are no more arguments
        cmp ecx, 0
        je main_2               

        push esi                ; save esi
        push ecx                ; save ecx

        push dword [esi]              ; push argv[i] as argument to strlen
        call strlen             ; strlen(argv[i])
        add esp, 4              ; remove the argument from the stack

        pop ecx                 ; restore ecx
        pop esi                 ; restore esi

        mov edx, eax           ; store the length of the argument in edx
        mov eax, sys_write     ; sys_write
        mov ebx, Outfile       ; Outfile (default stdout)
        mov ecx, [esi]         ; pointer to the argument
        int 0x80                ; call kernel
        mov eax, sys_write     ; sys_write
        mov ebx, Outfile       ; Outfile (default stdout)
        mov ecx, newline       ; load pointer to newline label
        mov edx, 1             ; length = 1
        int 0x80                ; call kernel

        add esi, 4              ; esi += 4 (next argument)
        dec ecx                 ; decrement argc
        jmp print_args_loop     ; loop

encode_input: ; void encode_input()
; Read from Inputfile (default stdin) and encode the input to Outputfile(default stdout).
    encode_input_loop:
        mov eax, sys_read       ; sys_read
        mov ebx, [Infile]       ; stdin by default
        mov ecx, buffer         ; buffer to store the input  
        mov edx, 1              ; length = 1
        int 0x80                ; call kernel

        ; Check if the input is null
        cmp eax, 0
        je main_exit
        cmp byte [buffer], 0
        je main_exit

        ; Encode the character
        cmp byte [buffer], 'A'
        jb write_char
        cmp byte [buffer], 'Z'
        jbe increment_char
        cmp byte [buffer], 'a'
        jb write_char
        cmp byte [buffer], 'z'
        ja write_char

        increment_char:
        inc byte [buffer]        ; increment the character

        write_char:
            ; Write the encoded character to stdout
            mov eax, sys_write      ; sys_write
            mov ebx, [Outfile]      ; stdout by default
            mov ecx, buffer
            mov edx, 1              ; length = 1
            int 0x80                ; call kernel
            jmp encode_input_loop
