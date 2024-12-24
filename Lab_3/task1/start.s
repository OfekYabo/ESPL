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
    IntputFile_String db "-i", 0
    OutputFile_String db "-o", 0
    debug_msg db "Debug: ", 0
    debug_open_input db "Open input file: ", 0
    debug_open_output db "Open output file: ", 0
    debug_read db "Read: ", 0
    debug_write db "Write: ", 0

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

    push esi                ; pass argv to print_args
    push ecx                ; pass argc to print_args
    call parse_args         ; parse command-line arguments
    call print_args         ; print all command-line arguments
    add esp, 8              ; clean arguments from the stack

    ; Encode input from stdin and write to stdout
    call encode_input

    ; Exit the program
    mov eax, 0             ; return 0 (success)
    ret

parse_args: ; void parse_args(int argc, char *argv[])
; Parse command-line arguments for -i and -o options

    mov ecx, [esp + 4]     ; get argc
    mov esi, [esp + 8]     ; get argv

    parse_args_loop:
        ; Check if there are no more arguments
        cmp ecx, 0
        je return

        push esi                ; save argv
        push ecx                ; save argc

        push dword [esi]        ; push argv[i] as argument to print_arg
        call check_i_option     ; check for -i option
        cmp eax, 0              ; if -i option is found, skip -o check
        je continue
        call check_o_option     ; check for -o option
        
        continue:
            add esp, 4              ; remove the argument argv[i] from the stack

            pop ecx                 ; restore argc
            pop esi                 ; restore argv

            ; Move to the next argument
            dec ecx                 ; decrement argc
            add esi, 4              ; esi += 4 (next argument)
            jmp parse_args_loop     ; loop




check_i_option: ; int check_i_option(char *arg)
; Check if the argument start with the -i option.
    
    mov esi, [esp + 4]      ; get argv[i] argument
    push esi                ; save argument argv[i]

    push dword 2            ; length of "-i"
    push IntputFile_String  ; "-i"
    push esi                ; pass argument to strncmp
    call strncmp
    add esp, 12             ; clean arguments from the stack

    pop esi                 ; restore argument argv[i]

    cmp eax, 0
    jne return

    add esi, 2              ; skip the first 2 characters ("-i")

    push esi                ; pass the file name to open_input_file
    call open_input_file
    add esp, 4              ; clean argument from the stack
    mov eax, 0             ; return 0 (true)
    ret

check_o_option: ; int check_o_option(char *arg)
; Check if the argument start with the -o option.
    
    mov esi, [esp + 4]      ; get argv[i] argument
    push esi                ; save argument argv[i]

    push dword 2            ; length of "-o"
    push OutputFile_String  ; "-o"
    push esi                ; pass argument to strncmp
    call strncmp
    add esp, 12             ; clean arguments from the stack

    pop esi                 ; restore argument argv[i]

    cmp eax, 0
    jne return

    add esi, 2              ; skip the first 2 characters ("-o")

    push esi                ; pass the file name to open_output_file
    call open_output_file
    add esp, 4              ; clean argument from the stack
    mov eax, 0             ; return 0 (true)
    ret


; cmp byte [edi], '-'
; jne parse_args_loop
; cmp byte [edi + 1], 'i'
; jne check_o_option
; add edi, 2             ; skip -i
; push edi
; call open_input_file
; jmp parse_args_loop

; check_o_option:
;     ; Check for -o option
;     cmp byte [edi + 1], 'o'
;     jne parse_args_loop
;     add edi, 2             ; skip -o
;     push edi
;     call open_output_file
;     jmp parse_args_loop


open_input_file: ; void open_input_file(char *filename)
; Open the input file specified by -i{file}

    mov eax, sys_open      ; sys_open
    mov ebx, [esp + 4]     ; get Input filename argument
    mov ecx, O_RDONLY      ; read-only mode
    int 0x80               ; call kernel
    mov [Infile], eax      ; store the file descriptor in Infile

    ; Debug print
    push eax
    push debug_open_input
    call print_debug
    add esp, 8

    ret

open_output_file: ; void open_output_file(char *filename)
; Open the output file specified by -o{file}

    mov eax, sys_open      ; sys_open
    mov ebx, [esp + 4]     ; get Output filename argument
    mov ecx, O_WRONLY | O_CREAT | O_TRUNC ; write-only, create, truncate
    mov edx, 0666          ; file permissions
    int 0x80               ; call kernel
    mov [Outfile], eax     ; store the file descriptor in Outfile

    ; Debug print
    push eax
    push debug_open_output
    call print_debug
    add esp, 8

    ret

print_debug: ; void print_debug(int value, char *msg)
; Print a debug message with a value to stderr

    mov eax, sys_write      ; sys_write
    mov ebx, stderr         ; stderr
    mov ecx, [esp + 4]      ; pointer to the message
    mov edx, 7              ; length of the message
    int 0x80                ; call kernel

    mov eax, sys_write      ; sys_write
    mov ebx, stderr         ; stderr
    mov ecx, [esp + 8]      ; pointer to the value
    mov edx, 4              ; length of the value
    int 0x80                ; call kernel

    ret

print_args: ; void print_args(int argc, char *argv[])
; Print all command-line arguments.

    mov ecx, [esp + 4]     ; get argc
    mov esi, [esp + 8]     ; get argv

    print_args_loop:
        ; Check if there are no more arguments
        cmp ecx, 0
        je return
        
        push esi                ; save argv
        push ecx                ; save argc

        push dword [esi]        ; push argv[i] as argument to print_arg
        call print_arg          ; print the argument
        add esp, 4              ; remove the argument from the stack

        pop ecx                 ; restore ecx
        pop esi                 ; restore esi

        ; Move to the next argument
        dec ecx                 ; decrement argc
        add esi, 4              ; esi += 4 (next argument)
        jmp print_args_loop     ; loop

print_arg: ; void print_arg(char *arg)
; Print a single argument.

    mov esi, [esp + 4]     ; get argv[i]

    print_arg_loop:
        ; Check if the current character is null
        cmp byte [esi], 0
        je print_newline

        ; Print the current character
        mov eax, sys_write      ; sys_write
        mov ebx, Outfile        ; Outfile (default stdout)
        mov ecx, esi            ; pointer to the character
        mov edx, 1              ; length = 1
        int 0x80                ; call kernel

        ; Move to the next character
        inc esi
        jmp print_arg_loop

    print_newline:
        ; Print a newline
        mov eax, sys_write      ; sys_write
        mov ebx, Outfile        ; Outfile (default stdout)
        mov ecx, newline        ; load pointer to newline label
        mov edx, 1              ; length = 1
        int 0x80                ; call kernel

        ret

encode_input: ; void encode_input()
; Read from Inputfile (default stdin) and encode the input to Outputfile(default stdout).

    mov eax, sys_read       ; sys_read
    mov ebx, [Infile]       ; stdin by default
    mov ecx, buffer         ; buffer to store the input  
    mov edx, 1              ; length = 1
    int 0x80                ; call kernel

    ; Debug print
    push eax
    push debug_read
    call print_debug
    add esp, 8

    ; Check if the input is null
    cmp eax, 0
    je return
    mov esi, buffer
    cmp byte [esi], 0
    je return

    ; Encode the character
    cmp byte [esi], 'A'
    jb write_char
    cmp byte [esi], 'Z'
    jbe increment_char
    cmp byte [esi], 'a'
    jb write_char
    cmp byte [esi], 'z'
    ja write_char

    increment_char:
    inc byte [esi]        ; increment the character

    write_char:
        ; Write the encoded character to stdout
        mov eax, sys_write      ; sys_write
        mov edi, [Outfile]      ; stdout by default
        mov ecx, buffer
        mov edx, 1              ; length = 1
        int 0x80                ; call kernel


        ; Debug print
        push eax
        push debug_write
        call print_debug
        add esp, 8
        
        jmp encode_input

return:
    ret

exit_program:
    mov eax, sys_exit       ; sys_exit
    xor ebx, ebx            ; exit code 0
    int 0x80                ; call kernel
