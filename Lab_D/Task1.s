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