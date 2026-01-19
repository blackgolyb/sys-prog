include \masm64\include64\masm64rt.inc

.data
    array dq -5, 10, -15, -20, 7, -30, -13, -14, 25, -17
    array_size dq 10
    E dq -13

    res_sum dq ?
    res_n dq ?

    message_fmt db "Варіант №5",10,
       "Массив: -5, 10, -15, -20, 7, -30, -13, -14, 25, -17",10,
       "Cума елементів: %d",10,
       "Кількість елементів: %d",10,10,
       "Автор: Омельніцький А.М.",0

    res_buf db 512 dup(0)

.code

main proc
    ; r10 - current element
    ; rsi - current index
    ; rds - sum
    ; rcx - number of good elements

    xor rcx, rcx        ; Обнуляємо лічильник
    xor rdx, rdx        ; Обнуляємо суму
    xor rsi, rsi        ; Обнуляємо індекс

loop_start:
    cmp rsi, [array_size]
    jge loop_end          ; Якщо індекс >= розмір масиву, виходимо з циклу

    mov r10, [array + rsi*8]

    cmp r10, [E]
    jg next_element     ; Перевіряємо умову: Ai ≤ E Якщо елемент > E, пропускаємо

    ; Елемент задовольняє умову
    inc rcx             ; Збільшуємо лічильник
    add rdx, r10        ; Додаємо елемент до суми

next_element:
    inc rsi              ; Збільшуємо індекс
    jmp loop_start       ; Повертаємось на початок циклу

loop_end:
    mov res_n, rcx
    mov res_sum, rdx

    invoke wsprintf, addr res_buf, addr message_fmt, res_sum, res_n
    invoke MessageBox, 0, addr res_buf, chr$("Перебираємо масив"), 0
    invoke ExitProcess, 0
main endp
end
