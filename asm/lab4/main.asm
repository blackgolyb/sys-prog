include \masm64\include64\masm64rt.inc

.data
    N equ 40

    ; Масиви A і B з 50 елементів (для демонстрації)
    array_A dq 571, 487, 1097, 467, 318, 648, 251, 332, 219, 937,
            610, 384, 410, 897, 327, 357, 987, 471, 457, 879,
            713, 108, 333, 581, 569, 1035, 623, 1029, 378, 1050,
            506, 599, 725, 531, 421, 407, 413, 485, 635, 272
            ;829, 902, 411, 452, 286, 621, 1000, 1012, 209, 597

    array_B dq 227, 764, 201, 838, 1030, 297, 659, 494, 787, 683,
            354, 1006, 690, 854, 887, 1050, 363, 962, 921, 1021,
            769, 1041, 357, 922, 217, 1078, 679, 735, 170, 604,
            874, 874, 317, 1087, 277, 357, 1053, 551, 1051, 587
            ;669, 920, 739, 1078, 838, 839, 328, 680, 769, 922

    ; Результуючий масив C
    array_C dq N dup(?)

    ; Лічильник співпадінь
    match_count dq 0

    message_fmt db "Варіант №5",10,10,
        "A: %s",10,10,
        "B: %s",10,10,
        "C: %s",10,10,
        "Кількість попадань: %d",10,
        "Автор: Омельніцький А.М.",0
    reduce_fmt db "%s, %d",0
    element_fmt db "%d",0
    copy_fmt db "%s",0
    null_fmt db " ",0

    tmp_buffer db 1024 dup(0)
    array_A_buffer db 1024 dup(0)
    array_B_buffer db 1024 dup(0)
    array_C_buffer db 1024 dup(0)
    res_buf db 4096 dup(0)

.code

format_array proc
    xor rsi, rsi
    mov r13, rcx
    mov r14, rdx
    mov r15, r8

    invoke wsprintf, addr tmp_buffer, addr null_fmt
loop_start:
    cmp rsi, r13
    jge loop_end          ; Якщо індекс >= розмір масиву, виходимо з циклу

    invoke wsprintf, addr tmp_buffer, addr copy_fmt, r15 ; копіюємо наш буфер в тимчасовий
    mov r10, [r14 + rsi*8]   ; array[i]
    invoke wsprintf, r15, addr reduce_fmt, addr tmp_buffer, r10 ; додаємо новий елемен

    inc rsi              ; Збільшуємо індекс
    jmp loop_start       ; Повертаємось на початок циклу

loop_end:
    ret
format_array endp

check_bits_match proc value1:DQ, value2:DQ
    ; Перевіряємо біт 0
    bt value1, 0
    setc bh           ; зберігаємо CF в bh (1 якщо біт встановлений)
    bt value2, 0
    setc bl           ; зберігаємо CF в bl

    cmp bh, bl
    jne bits_no_match  ; якщо не збігаються - вихід

    ; Перевіряємо біт 1
    bt value1, 1          ; перевіряємо біт 1 в A[i]
    setc bh
    bt value2, 1          ; перевіряємо біт 1 в B[i]
    setc bl

    cmp bh, bl
    jne bits_no_match  ; якщо не збігаються - вихід

    ; Перевіряємо біт 2
    bt value1, 2          ; перевіряємо біт 2 в A[i]
    setc bh
    bt value2, 2          ; перевіряємо біт 2 в B[i]
    setc bl

    cmp bh, bl
    jne bits_no_match  ; якщо не збігаються - вихід

    ; Всі біти збігаються
    mov al, 1
    jmp bits_check_end

bits_no_match:
    mov al, 0

bits_check_end:
    ret
check_bits_match endp


process_arrays proc
    xor rsi, rsi
loop_start:
    cmp rsi, N
    jge loop_end          ; Якщо індекс >= розмір масиву, виходимо з циклу

    ; Завантажуємо A[i] і B[i]
    mov r10, [array_A + rsi*8]   ; A[i]
    mov r11, [array_B + rsi*8]   ; B[i]


    ; Перевіряємо збіг бітів
    invoke check_bits_match, r10, r11

    test al, al
    jz element_no_match        ; якщо біти не збігаються

    ; Елемент задовольняє умову
    ; Біти збігаються: C[i] = A[i] + B[i]
    mov rax, r10
    add rax, r11            ; A[i] + B[i]
    mov qword ptr [array_C + rsi*8], rax

    inc match_count

    jmp next_element

element_no_match:
    ; Біти не збігаються: C[i] = 0
    mov qword ptr [array_C + rsi*8], 0

next_element:
    inc rsi              ; Збільшуємо індекс
    jmp loop_start       ; Повертаємось на початок циклу

loop_end:
    ret
process_arrays endp


main proc
    invoke process_arrays

    mov rcx, N
    lea rdx, array_A
    lea r8, array_A_buffer
    call format_array

    mov rcx, N
    lea rdx, array_B
    lea r8, array_B_buffer
    call format_array

    mov rcx, N
    lea rdx, array_C
    lea r8, array_C_buffer
    call format_array

    invoke wsprintf, addr res_buf, addr message_fmt, addr array_A_buffer, addr array_B_buffer, addr array_C_buffer, match_count
    invoke MessageBox, 0, addr res_buf, chr$("Бінарні операції"), 0
    invoke ExitProcess, 0
main endp
end
