section .text
global hash_function

hash_function:
    ; Parameters:
    ;   rdi = str (const char*): pointer to the input string
    ;   rsi = len (size_t): length of the string
    ; Return:
    ;   eax = 32-bit hash value (lower 32 bits of the accumulator)

    push rbx                ; Save rbx (callee-saved register)
    xor rax, rax            ; Clear 64-bit accumulator
    xor rdx, rdx            ; Clear 64-bit index counter

.loop:
    cmp rdx, rsi            ; Compare index (rdx) with length (rsi)
    jge .done               ; If index >= length, exit loop
    movzx ecx, byte [rdi + rdx] ; Load next character into ecx (zero-extended to 32 bits)

    ; Inline len_function logic
    cmp cl, '1'             ; Compare character with '1'
    je .one
    cmp cl, '&'             ; Compare character with '&'
    je .and
    cmp cl, '*'             ; Compare character with '*'
    je .star
    mov ecx, 1              ; Default len_value = 1
    jmp .after_len
.one:
    mov ecx, 1              ; len_value = 1 for '1'
    jmp .after_len
.and:
    mov ecx, 2              ; len_value = 2 for '&'
    jmp .after_len
.star:
    mov ecx, 0xFFFFFFFF     ; len_value = 0xFFFFFFFF for '*'

.after_len:
    mov rbx, rdx            ; Copy index to rbx
    inc rbx                 ; rbx = index + 1 (position)
    imul rbx, rcx           ; rbx = (index + 1) * len_value (64-bit multiplication)
    add rax, rbx            ; Add result to 64-bit accumulator
    inc rdx                 ; Increment index
    jmp .loop               ; Repeat for next character

.done:
    pop rbx                 ; Restore rbx
    ret                     ; Return, hash value in eax (lower 32 bits of rax)