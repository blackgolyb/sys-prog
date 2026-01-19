include \masm64\include64\masm64rt.inc

.data
    value dq 51
    result dq ?

    message_fmt db "Варіант №5",10,
        "Вираз для обчислення: (value - 1) << 1",10,
        "Значення: %d",10,
        "Результат: %d",10,10,
        "Автор: Омельніцький А.М.",0

    res_buf db 512 dup(0)

.code
calc_task proc
    mov rax, value
    sub rax, 1
    shl rax, 1
    ret
calc_task endp

main proc
    invoke calc_task
    mov result, rax

    invoke wsprintf, addr res_buf, addr message_fmt, value, result
    invoke MessageBox, 0, addr res_buf, chr$("Бінарні операції"), 0
    invoke ExitProcess, 0
main endp
end
