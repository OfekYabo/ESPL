main: ; Task 0
    ; Prologue
    push ebp
    mov ebp, esp
    pushad

    ; Get argc
    mov eax, [ebp+8]   ; argc

    ; Print argc
    push eax
    push fmt
    call printf        ; C Function call printf
    add esp, 8

    ; Print argv[i] using puts
    mov ebx, 0         ; i = 0

    print_argv_loop:
        ; Get argc and argv
        mov eax, [ebp+8]   ; argc
        mov ecx, [ebp+12]  ; argv

        cmp ebx, eax       ; Compare i with argc
        jge end_loop       ; If i >= argc, exit loop

        ; Get argv[i]
        mov edx, [ecx + ebx*4]

        ; Print argv[i]
        push ebx          ; Save ebx
        push edx
        call puts          ; C Function call puts
        add esp, 4
        pop ebx           ; Restore ebx

        ; Increment i
        inc ebx
        jmp print_argv_loop

    end_loop:
        ; Epilogue
        popad
        mov esp, ebp
        pop ebp
        ret