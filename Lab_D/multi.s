section .data
    BUFFER_SIZE equ 600                           ; Size of the input buffer
    fmt db "argc = %d", 10, 0                     ; Format string for printf
    string_fmt db "%s", 0                         ; Format string for puts
    hex_fmt db "%04x", 0                          ; Format string for printing hex bytes
    newline db 10, 0                              ; Newline character
    leading_zero db '0'                           ; Leading zero
    input_buffer times BUFFER_SIZE db 0           ; Buffer to store input from fgets
    debug_msg_char db "Ivalid Char", 10, 0        ; Error message for invalid character
    debug_msg_args db "Ivalid Main Args", 10, 0   ; Error message for invalid character

    STATE dw 0xACE1  ; 16 bit Initial seed value (non-zero)
    MASK dw 0xB400   ; Mask for the fibonacci 16 bit LFSR (taps at 16, 14, 13, 11)

    ; Initialize the global structs
    x_struct_user dw 0
    x_num_user times 150 dw 0
    y_struct_user dw 0
    y_num_user times 150 dw 0

    x_struct: dw 5
    x_num: dw 0xaa, 1,2,0x44,0x4f
    pad: dw 0
    y_struct: dw 6
    y_num: dw 0xaa, 1,2,3,0x44,0x4f
    
section .bss
    ; No uninitialized data

section .text
    global main
    extern printf, puts, sscanf, fgets, stdin, malloc


;--------------------------------------------------------------------


_start:
    mov eax, [esp]       ; argc
    lea ebx, [esp+4]     ; argv (pointer to array of arguments)

    push ebx             ; push argv
    push eax             ; push argc
    call main

    mov eax, 1           ; exit
    xor ebx, ebx         ; exit status
    int 0x80

main:
    ; Prologue
    push ebp
    mov ebp, esp
    pushad

    mov eax, [ebp + 8]          ; eax <- argc
    mov ebx, [ebp + 12]         ; ebx <- argv

    cmp eax, 1
    je default_func             ; if no args, then jump to default_func

    mov edx, [ebx + 4]          ; edx <- argv[1]

    cmp word [edx], "-I"        ; check if the first argument is "-I"
    je user_argument

    cmp word [edx], "-R"        ; check if the first argument is "-R"
    je PRNG_argument

    jmp error_args              ; if the first argument is not "-I" or "-R"

end_main:
    ; Epilogue
    popad
    mov esp, ebp
    pop ebp
    ret        ;  returning to _start

error_args:
    ; Print an error message
    push debug_msg_args
    push string_fmt
    call printf
    add esp, 8
    jmp end_main

default_func:
    mov esi, x_struct
    mov edi, y_struct
    jmp print_add_print

user_argument:                  ; "-I" case
    mov esi, x_struct_user
    push esi
    call read_multi
    add esp,4

    mov esi, y_struct_user
    push esi
    call read_multi
    add esp,4

    mov esi, x_struct_user
    mov edi, y_struct_user
    jmp print_add_print


PRNG_argument:                  ; "-R" case
    mov esi, x_struct_user
    push esi
    call PRmulti
    add esp,4

    mov esi, y_struct_user
    push esi
    call PRmulti
    add esp,4

    mov esi, x_struct_user
    mov edi, y_struct_user
    jmp print_add_print

print_add_print:
    push edi            ; save edi
    push esi            ; save esi

    ; print x
    push esi
    call print_multi
    add esp, 4

    pop esi             ; restore esi
    pop edi             ; restore edi

    push edi            ; save edi
    push esi            ; save esi

    ; print y
    push edi
    call print_multi
    add esp, 4
    
    pop esi             ; restore esi
    pop edi             ; restore edi
    
    ; add x and y
    push esi
    push edi
    call add_multi
    add esp, 8
break9:
    ; print the result
    push eax
    call print_multi
    add esp, 4

    jmp end_main        ; end of main


;--------------------------------------------------------------------


print_multi:
    ; Prologue
    push ebp
    mov ebp, esp
    pushad
break5:
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
    call fgets
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
    ; Convert the character to a byte
    sub bl, '0'        ; Convert ASCII character to numeric value
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
    sub bl, '0'
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
    push debug_msg_char
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


;--------------------------------------------------------------------


get_max_min:
    ; Prologue
    push ebp
    mov ebp, esp
    sub esp, 8            ; Allocate 8 bytes on the stack for the return value
    pushad

    ; Load the size fields of the two structs
    mov esi, [ebp+8]   ; esi = p (first struct)
    mov edi, [ebp+12]  ; edi = q (second struct)
    mov ax, word [esi]  ; eax = p->size
    mov bx, word [edi]  ; ebx = q->size

    ; Compare the sizes
    cmp ax, bx
    jge p_is_larger_or_equal

    ; If q is larger, swap esi and edi
    xchg esi, edi

p_is_larger_or_equal:
    ; Set the pointers in eax and ebx
    mov eax, esi  ; eax = pointer to the larger struct
    mov ebx, edi  ; ebx = pointer to the smaller struct
    mov [ebp-4], eax  ; Save the pointer to the larger struct in the allocated space
    mov [ebp-8], ebx  ; Save the pointer to the smaller struct in the allocated space

    ; Epilogue
    popad
    mov eax, [ebp-4]  ; Restore pointer to the larger struct from the allocated space
    mov ebx, [ebp-8]  ; Restore pointer to the smaller struct from the allocated space
    mov esp, ebp
    pop ebp
    ret


;--------------------------------------------------------------------


add_multi:
    ; Prologue
    push ebp
    mov ebp, esp
    sub esp, 4            ; Allocate 4 bytes on the stack for the return value
    pushad

    ; Get pointers to the structs
    mov eax, [ebp+8]   ; eax = p (first struct)
    mov ebx, [ebp+12]  ; ebx = q (second struct)
break1:
    ; Get the larger and smaller structs
    push ebx           
    push eax
    call get_max_min
    add esp, 8         ; Clean up the stack
    ; eax = pointer to the larger struct
    ; ebx = pointer to the smaller struct
break2:
    ; Allocate memory for the result struct
    movzx ecx, word [eax]  ; ecx = size of the larger struct
    inc ecx                ; Increment size for the carry
    shl ecx, 1             ; Multiply by 2 (size in bytes)
    push ebx               ; Save smaller struct
    push eax               ; Save larger struct
    push ecx
    call malloc
    add esp, 4
    mov edi, eax           ; edi = pointer to the result struct
    pop eax                ; Restore eax (ponter to larger struct)
    pop ebx                ; Restore ebx (pointer to smaller struct)
    mov [ebp-4], edi      ; Save the pointer to the result struct in the allocated space

    ; Initialize the result struct
    movzx ecx, word [eax]  ; ecx = size of the larger struct
    mov word [edi], cx     ; result->size = size of the larger struct + 1

break3:
    shl ecx, 1             ; Multiply by 2 (size in bytes)
    add edi, 2             ; edi = result->num
    add eax, 2             ; eax = larger->num
    add ebx, 2             ; ebx = smaller->num
break4:

    ; Perform byte-wise addition
    xor esi, esi           ; esi = index = 0
    clc                    ; Clear carry flag

add_loop:
    mov dl, byte [ebx]    ; dl = smaller->num[esi]
    mov dh, byte [eax]    ; dh = larger->num[esi]
    adc dl, dh                  ; dl = smaller->num[esi] + larger->num[esi] + carry
    mov byte [edi], dl    ; result->num[esi] = eax
    inc edi                     ; Increment index
    inc eax                     ; Increment index
    inc ebx                     ; Increment index
    loop add_loop

Assume_no_final_carry:
    ; Epilogue
    popad
    mov eax, [ebp-4]      ; Restore pointer to the result struct from the allocated space
break8:
    mov esp, ebp
    pop ebp
    ret


;--------------------------------------------------------------------


PRmulti:
    ; Prologue
    push ebp
    mov ebp, esp
    pushad

    ; Generate a random size for the struct
generate_size:
    call rand_num
    and eax, 0xFF      ; Limit the size to 255 (0xFF)
    test eax, eax
    jz generate_size

    ; Get the pointer to the struct multi
    mov edi, [ebp+8]   ; edi = p
    mov word [edi], ax ; p->size = random size

    ; Get the pointer to the num array
    add edi, 2         ; edi = p->num

    ; Generate random values for the num array
    shl eax, 1         ; Multiply the size by 2 (size in bytes)
    mov ecx, eax       ; ecx = size
generate_loop:
    push edi           ; Save the pointer to the num array
    push ecx           ; Save the loop counter
    call rand_num
    pop ecx            ; Restore the loop counter
    pop edi            ; Restore the pointer to the num array
    mov byte [edi], al ; p->num[i] = random value
    inc edi            ; Move to the next byte
    loop generate_loop ; Repeat for the entire size

    ; Epilogue
    popad
    mov esp, ebp
    pop ebp
    ret


;--------------------------------------------------------------------


rand_num:
    ; Prologue
    push ebp
    mov ebp, esp
    sub esp, 4            ; Allocate 4 bytes on the stack for the return value
    pushad

    ; Load the current state
    mov ax, [STATE]

    ; Compute the parity of the relevant bits using the MASK
    mov bx, ax
    and bx, [MASK]

    ; Calculate parity of the lower 8 bits
    test bl, bl
    setpo cl  ; Set CL to 1 if parity is odd (PF=0), otherwise set to 0

    ; Calculate parity of the upper 8 bits
    shr bx, 8
    test bl, bl
    setpo ch  ; Set CH to 1 if parity is odd (PF=0), otherwise set to 0

    ; Combine the parities
    xor cl, ch  ; XOR the parities to get the final parity in CL
    movzx bx, cl  ; Move the final parity to BX

    ; Shift the state to the right and insert the parity bit at the MSB
    shr ax, 1
    shl bx, 15
    or ax, bx

    ; Update the state
    mov [STATE], ax

    ; Return the new random value in ax
    movzx eax, ax
    mov [ebp-4], eax      ; Save the pointer to the result struct in the allocated space

    ; Epilogue
    popad
    mov eax, [ebp-4]      ; Restore pointer to the result struct from the allocated space
    mov esp, ebp
    pop ebp
    ret




