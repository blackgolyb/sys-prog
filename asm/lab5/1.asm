include \masm64\include64\masm64rt.inc

.data
    N equ 15

    ; Масиви A і B з 15 елементів (для демонстрації)
    array_A dq 7, 13, 22, 5, 9, 31, 17, 4, 26, 8, 12, 19, 28, 3, 15
    array_B dq 15, 9, 3, 7, 22, 28, 5, 11, 14, 31, 4, 10, 13, 18, 8

    ; Лічильник співпадінь
    match_count dq 0

    message_fmt db "Варіант №5",10,10,
        "A: %s",10,10,
        "B: %s",10,10,
        "Результат: %d",10,
        "Автор: Омельніцький А.М.",0
    message_fmt2 db "Варіант №5",10,10,
        "Результат: %d",10,
        "Автор: Омельніцький А.М.",0
    reduce_fmt db "%s, %d",0
    element_fmt db "%d",0
    copy_fmt db "%s",0
    null_fmt db " ",0

    tmp_buffer db 1024 dup(0)
    array_A_buffer db 1024 dup(0)
    array_B_buffer db 1024 dup(0)
    res_buf db 4096 dup(0)
    result_message_size dq 0

    fName BYTE "lab5_res.txt",0
    fHandle dq ?
    cWritten dq ?
    fmt2 db "%d",0
    buf2 dq 4 dup(0)
    hFile dq ?,0



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


array_contains proc
    push rsi

    mov r13, rcx
    mov r14, rdx
    mov r15, r8

    xor rsi, rsi
loop_start:
    cmp rsi, r13
    jge element_not_match          ; Якщо індекс >= розмір масиву, виходимо з циклу

    mov r10, [r14 + rsi*8]   ; array[i]

    cmp r10, r15
    je element_match

    inc rsi              ; Збільшуємо індекс
    jmp loop_start       ; Повертаємось на початок циклу

element_match:
    mov al, 1
    jmp test_end

element_not_match:
    mov al, 0
    jmp test_end

test_end:
    pop rsi
    ret
array_contains endp


process_arrays proc
    xor rsi, rsi
loop_start:
    cmp rsi, N
    jge loop_end          ; Якщо індекс >= розмір масиву, виходимо з циклу

    mov r10, [array_A + rsi*8]   ; A[i]

    mov rcx, N
    lea rdx, array_B
    mov r8, r10
    call array_contains

    test al, al
    jz next_element

    inc match_count

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

    invoke wsprintf, addr res_buf, addr message_fmt, addr array_A_buffer, addr array_B_buffer, match_count
    invoke MessageBox, 0, addr res_buf, chr$("Робота з файлами"), 0

    invoke lstrlenA, addr res_buf
    mov result_message_size, rax

    invoke CreateFile, addr fName, GENERIC_WRITE,0,0,CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE,0
    mov hFile, rax
    invoke WriteFile, hFile, addr res_buf, result_message_size, addr cWritten, 0
    invoke CloseHandle, hFile

    invoke ExitProcess, 0
main endp
end
