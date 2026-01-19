include \masm64\include64\masm64rt.inc

.data
    var_a dq 20
    var_b dq 3
    var_c dq 4
    var_d dq 5
    var_e dq 1
    var_f dq 2
    var_g dq 3

    message_fmt db "Варіант №5",10,
       "Вираз для обчислення: a/c + bd - efg",10,
       "Значення: a=%d, b=%d, c=%d, d=%d, e=%d, f=%d, g=%d",10,
       "Результат = %d",10,10,
       "Автор: Омельніцький А.М.",0

    res_buf db 512 dup(0)

.code
calc_task proc _a:DQ, _b:DQ, _c:DQ, _d:DQ, _e:DQ, _f:DQ, _g:DQ
    ; a/c + bd - efg

    ; e * f * g
    mov rax, _e
    mul _f
    mul _g
    push rax

    ; b * d
    mov rax, _b
    mul _d
    push rax

    ; a/c
    mov rax, _a
    xor rdx, rdx
    div _c

    ; res
    pop rbx
    add rax, rbx
    pop rbx
    sub rax, rbx

    ret
calc_task endp


main proc
    invoke calc_task, var_a, var_b, var_c, var_d, var_e, var_f, var_g
    mov r10, rax
    invoke wsprintf, addr res_buf, addr message_fmt, var_a, var_b, var_c, var_d, var_e, var_f, var_g, r10
    invoke MessageBox, 0, addr res_buf, chr$("Процедури з параметрами"), 0
    invoke ExitProcess, 0
main endp
end
