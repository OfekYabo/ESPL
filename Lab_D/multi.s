section .data
    fmt db "argc = %d", 10, 0  ; Format string for printf
    puts_fmt db "%s", 10, 0    ; Format string for puts
    hex_fmt db "%04hhx", 0     ; Format string for printing hex bytes
    newline db 10, 0           ; Newline character

    ; Initialize the global struct
    x_struct db 5
    x_num dw 0x00aa, 1, 2, 0x0044, 0x004f  ; Use 'dw' to define words

section .bss
    ; No uninitialized data

section .text
    global main
    global print_multi
    extern printf, puts
    global read_multi
    extern printf, puts, sscanf

main: ; Task 1
    ; Prologue
    push ebp
    mov ebp, esp
    pushad

    ; Get the pointer to the struct multi
    mov esi, x_struct

    ; Call print_multi
    push esi
    call print_multi
    add esp, 4

    ; Epilogue
    popad
    mov esp, ebp
    pop ebp
    ret

print_multi:
    ; Prologue
    push ebp
    mov ebp, esp
    pushad

    ; Get the pointer to the struct multi
    mov esi, [ebp+8]   ; esi = p
    ; Get the size of the multi-precision integer
    movzx ecx, byte [esi]   ; ecx = p->size
    ; Get the pointer to the num array
    add esi, 1   ; esi = p->num

print_loop:
    dec ecx         ; Loop through the num array and print each word in hex in reverse order
    js end_print_loop

    ; Get the current word
    movzx edx, word [esi + ecx*2]

    push ecx  ; Save ecx
    push esi  ; Save esi

    ; Print the word in hex
    push edx
    push hex_fmt
    call printf
    add esp, 8
    pop esi  ; Restore esi
    pop ecx  ; Restore ecx

    jmp print_loop

end_print_loop:
    ; Print a newline
    push newline
    call puts
    add esp, 4

    ; Epilogue
    popad
    mov esp, ebp
    pop ebp
    ret

read_multi:
    ; Prologue
    push ebp
    mov ebp, esp
    pushad

    ; Get the pointer to the struct multi
    mov esi, [ebp+8]   ; esi = p
    ; Get the pointer to the string
    mov edi, [ebp+12]  ; edi = str

    ; Initialize the size to 0
    mov byte [esi], 0

read_loop:
    ; Read two characters from the string
    lodsb
    cmp al, 0
    je end_read_loop

    ; Convert the character to a byte
    sub al, '0'
    cmp al, 9
    jbe valid_digit
    sub al, 7
    cmp al, 15
    ja invalid_digit

valid_digit:
    shl al, 4
    lodsb
    sub al, '0'
    cmp al, 9
    jbe valid_digit2
    sub al, 7
    cmp al, 15
    ja invalid_digit

valid_digit2:
    or al, ah
    stosb
    inc byte [esi]

    jmp read_loop

invalid_digit:
    ; Handle invalid digit
    jmp end_read_loop

end_read_loop:
    ; Epilogue
    popad
    mov esp, ebp
    pop ebp
    ret