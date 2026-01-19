include \masm64\include64\masm64rt.inc

.data
    x_start dq 3.0
    x_step dq 1.25
    x_count dq 6
    coeff_a  dq 5.1                            ; Коефіцієнт a
    coeff_b  dq 5.3                            ; Коефіцієнт b
    results  dd 5 dup(?)                       ; Масив для результатів (32-бітні цілі числа)
    ; Обчислити 6 значень функції Yn = 4x/(x + 5) (х змінюється від 3  з кроком 1,25).
    ; Результат округлити до цілого числа.

    buf      db 512 dup(?)                     ; Буфер для виводу тексту
    fmt      db "Варіант №4",10,
                   "Функція: Yn = 4x/(x + 5)",10,
                   "Результати:",10,
                   "x = 4.7, Y = %d",10,
                   # "x = 7.7, Y = %d",10,
                   # "x = 10.7, Y = %d",10,
                   # "x = 13.7, Y = %d",10,
                   # "x = 16.7, Y = %d",10,10,
                   "Автор: Настенко Д.О., група КН-1023б",0
    titl     db "Обчислення функції",0

.code
calc_f proc val_x:DQ
    push BP
    mov BP, SP

    ; Завантажити x
    fld val_x               ; st(0) = x

    ; 4*x
    fld const_a             ; st(0) = 4.0, st(1) = x
    fmul st(0), st(1)       ; st(0) = 4*x, st(1) = x
    ; Стек: st(0)=4x, st(1)=x

    ; x+5
    fld st(1)               ; st(0) = x, st(1) = 4x, st(2) = x
    fld const_b             ; st(0) = 5.0, st(1) = x, st(2) = 4x, st(3) = x
    fadd                    ; st(0) = x+5, st(1) = 4x, st(2) = x
    ; Стек: st(0)=(x+5), st(1)=4x, st(2)=x

    ; 4x/(x+5)
    fdiv st(1), st(0)       ; st(1) = 4x/(x+5), st(0) = x+5
    fstp st(0)              ; Видалити st(0), тепер st(0) = 4x/(x+5), st(1) = x
    ; Стек: st(0)=результат, st(1)=x

    ; Округлити до цілого
    fld half                ; st(0) = 0.5, st(1) = результат, st(2) = x
    fadd                    ; st(0) = результат + 0.5, st(1) = x
    frndint                 ; st(0) = округлений результат, st(1) = x

    ; Зберегти результат
    fistp rax               ; Зберегти як ціле число, st(0) = x
    fstp st(0)              ; Очистити стек

    pop BP
    ret
calc_f endp

calculate_all_values proc
    push bp
    mov bp, sp
    push si
    push cx

    mov cx, x_count         ; Лічильник - 6 значень
    lea si, results         ; Вказівник на масив результатів

    ; Завантажити початкове значення x
    fld x_start             ; ST(0) = x = 3.0

calc_all_loop:
    ; Зберегти поточне x у пам'яті для передачі в процедуру
    fst r11                 ; Зберегти x у temp_x

    ; Викликати процедуру обчислення одного значення
    invoke calc_f r11

    ; Результат повертається у temp_result
    mov eax, temp_result
    mov [si], eax           ; Зберегти результат в масиві
    add si, 4               ; Перейти до наступного елементу

    ; Збільшити x на крок
    fld step                ; ST(0) = 1.25, ST(1) = x
    fadd                    ; ST(0) = x + 1.25

    loop calc_all_loop

    fstp st(0)              ; Очистити стек співпроцесора

    pop cx
    pop si
    pop bp
    ret
calculate_all_values endp

main proc
    finit                           ; Ініціалізація FPU

    fld x_start
    fst r11
    invoke calc_f r11
    invoke wsprintf, addr buf, addr fmt, rax
    invoke MessageBox, 0, addr buf, addr titl, MB_ICONINFORMATION
    invoke ExitProcess, 0
main endp
end
