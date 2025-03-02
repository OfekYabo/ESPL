section .data
    BUFFER_SIZE equ 600        ; Size of the input buffer
    fmt db "argc = %d", 10, 0  ; Format string for printf
    string_fmt db "%s", 0    ; Format string for puts
    hex_fmt db "%04x", 0     ; Format string for printing hex bytes
    newline db 10, 0           ; Newline character
    leading_zero db '0'          ; Leading zero
    input_buffer times BUFFER_SIZE db 0  ; Buffer to store input from fgets

    ; Initialize the global structs
    x_struct1 dw 0
    x_num1 times 150 dw 0
    x_struct2 dw 0
    x_num2 times 150 dw 0
    ; x_struct: dw 5
    ; x_num: dw 0x00aa, 1,2,0x0044,0x004f

    ; ; Test structs
    ; x_test_struct1 dw 1, 0x1111  ; Size 1, value 0x1111
    ; x_test_struct2 dw 1, 0x2222  ; Size 1, value 0x2222

    ; ; Debug message
    ; debug_msg db "Structs are equal", 10, 0
    debug_msg1 db "Ivalid Char", 10, 0

section .bss
    ; No uninitialized data

section .text
    global main
    extern printf, puts, sscanf, fgets, stdin


main:
    ; Prologue
    push ebp
    mov ebp, esp
    pushad

    ; Get the first number from the user
    mov esi, x_struct1
    push esi
    call read_multi
    add esp, 4

;     ; Test if x_struct1 is equal to x_test_struct1
;     mov esi, x_struct1
;     mov edi, x_test_struct1
;     mov ecx, 2  ; Compare 2 words (size and value)
;     repe cmpsw
;     jne continue_program

;     ; Print debug message if structs are not equal
;     push debug_msg
;     push string_fmt
;     call printf
;     add esp, 8

; continue_program:
    mov esi, x_struct1
    push esi
    call print_multi
    add esp, 4

    ; mov esi, x_test_struct1
    ; push esi
    ; call print_multi
    ; add esp, 4

break12:
    ; Get the second number from the user
    mov esi, x_struct2
    push esi
    call read_multi
    add esp, 4

break13:
    ; Print the second number
    mov esi, x_struct2
    push esi
    call print_multi
    add esp, 4

    ; Epilogue
    popad
    mov esp, ebp
    pop ebp
    ret

; main: ; Task 1
;     ; Prologue
;     push ebp
;     mov ebp, esp
;     pushad

;     ; Get the pointer to the struct multi
;     mov esi, x_struct

;     ; Call print_multi
;     push esi
;     call print_multi
;     add esp, 4
    
;     ; Epilogue
;     popad
;     mov esp, ebp
;     pop ebp
;     ret

print_multi:
    ; Prologue
    push ebp
    mov ebp, esp
    pushad

    ; Get the pointer to the struct multi
    mov esi, [ebp+8]   ; esi = p
    ; Get the size of the multi-precision integer
    movzx ecx, word [esi]   ; ecx = p->size
    ; Get the pointer to the num array
    add esi, 2   ; esi = p->num

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
    push string_fmt
    call printf
    add esp, 8

    ; Epilogue
    popad
    mov esp, ebp
    pop ebp
    ret


;--------------------------------------------------------------------


read_multi:
    ; Prologue
    push ebp
    mov ebp, esp
    pushad

    ; Read a line from stdin using fgets
    mov esi, input_buffer
    push dword [stdin]
    push BUFFER_SIZE
    push esi
    call fgets    ; fgets(input_buffer, BUFFER_SIZE, stdin)
    add esp, 12        ; Clean up the stack

    ; Get the pointer to the struct multi
    mov edi, [ebp+8]   ; edi = p
    add edi, 2         ; edi = p->num

    ; Initialize the size to 0
    mov edx, 0
    ; Get the pointer to the input buffer
    lea esi, [input_buffer]
    ; Find the actual number of characters read (excluding null terminator)
    mov ecx, BUFFER_SIZE

    xor eax, eax ; Clear eax
find_length:
    cmp byte [esi+ecx], 10  ; Compare the character with /n
    je found_length
    dec ecx
    jmp find_length

found_length:
    dec ecx ; clear /n from the buffer

read_loop:
    cmp ecx, 0  ; if ecx less than 0, then jump to end_read_loop
    jl end_read_loop
    xor eax, eax ; Clear eax
    xor ebx, ebx ; Clear ebx

    lea esi, [input_buffer+ecx-1] ; Get the address of the next character
    mov bl, [esi]               ; Read character from the string
break2:
    ; Convert the character to a byte
    sub bl, '0'        ; Convert ASCII character to numeric value
break3:
    cmp bl, 9          ; Compare with 9
    jbe valid_digit1   ; Jump if bl <= 9
    sub bl, 39          ; Adjust for hexadecimal digits 'A'-'F'
    cmp bl, 15         ; Compare with 15
    jbe valid_digit1   ; Jump if bl <= 15
    jmp invalid_digit  ; Otherwise, it's an invalid digit

valid_digit1:
    or al, bl         ; Store the digit in the lower 4 bits of eax
    shl al, 4         ; Shift al left by 4 bits
    lea esi, [input_buffer+ecx]     ; Get the address of the current character
    xor ebx, ebx ; Clear ebx
    mov bl, [esi]
break6:
    sub bl, '0'
break7:
    cmp bl, 9
    jbe valid_digit2
    sub bl, 39
    cmp bl, 15
    ja invalid_digit

valid_digit2:
    or al, bl        ; Store the digit in the lower 4 bits of eax
    sub ecx, 2         ; Move to the next 4 characters

    stosb            ; Store the byte in the buffer
    inc edx           ; Increment the size
    jmp read_loop

invalid_digit:
    ; Handle invalid digit
    push debug_msg1
    push string_fmt
    call printf     ; Print an error message
    add esp, 8
    jmp end_read_loop

end_read_loop:
    ; Store the size in the struct multi
    mov edi, [ebp+8]    ; edi = p
    add dx, 1           ; Add 1 to dx to ensure rounding up
    shr dx, 1           ; Divide dx by 2
    mov word [edi], dx  ; p->size = size

    ; put 0 in the input buffer
    mov ecx, BUFFER_SIZE
    lea esi, [input_buffer]
put_zero:
    cmp ecx, 0
    jle end_put_zero
    mov byte [esi], 0
    dec ecx
    inc esi
    jmp put_zero 
    
end_put_zero:
    ; Epilogue
    popad
    mov esp, ebp
    pop ebp
    ret