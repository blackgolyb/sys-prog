include \masm64\include64\masm64rt.inc

.data
    var_g dq 64
    var_f dq 16
    var_e dq 2
    var_d dq 4
    var_c dq 8
    var_b dq 32
    var_a dq 1

    start_time dq ?
    result1 dq ?
    result2 dq ?
    time1 dq ?
    time2 dq ?

    message_fmt db "Варіант №5",10,
        "Вираз для обчислення: g/f + (ed)/c + b/a",10,
        "Значення: a=%d, b=%d, c=%d, d=%d, e=%d, f=%d, g=%d",10,
        "Арифметично: Результат = %d; Час = %d",10,
        "Бінарно: Результат = %d; Час = %d",10,10,
        "Автор: Омельніцький А.М.",0

    res_buf db 512 dup(0)

.code
GetRDTSC proc
    rdtsc
    shl rdx, 32
    or rax, rdx
    ret
GetRDTSC endp

calc_task proc
    call GetRDTSC
    mov start_time, rax
    ; g/f + (ed)/c + b/a

    ; g/f
    mov rax, var_g
    xor rdx, rdx
    div var_f
    mov r8, rax

    ; b/a
    mov rax, var_b
    xor rdx, rdx
    div var_a
    mov r9, rax

    ; (ed)/c
    mov rax, var_e
    mul var_d
    xor rdx, rdx
    div var_c

    add rax, r8
    add rax, r9
    mov result1, rax

    call GetRDTSC
    sub rax, start_time
    mov time1, rax

    ret
calc_task endp


calc_bin_task proc
    call GetRDTSC
    mov start_time, rax

    ; g/f = 64/16 = 64 >> 4 (так як 16 = 2^4)
    mov rax, var_g
    shr rax, 4
    mov r8, rax

    ; b/a = 32/1 = 32 >> 0 (так як 1 = 2^0)
    mov rax, var_b
    mov r9, rax

    ; (e*d)/c = (2*4)/8 = 8/8 = 1
    mov rax, var_e
    shl rax, 2        ; зсув вліво на 2 біти (множення на 4) = 8
    shr rax, 3        ; зсув вправо на 3 біти (ділення на 8, так як 8=2^3)

    add rax, r8
    add rax, r9
    mov result2, rax

    call GetRDTSC
    sub rax, start_time
    mov time2, rax

    ret
calc_bin_task endp

main proc
    xor start_time, start_time
    invoke calc_task

    xor start_time, start_time
    invoke calc_bin_task

    invoke wsprintf, addr res_buf, addr message_fmt, var_a, var_b, var_c, var_d, var_e, var_f, var_g, result1, result2, time1, time2
    invoke MessageBox, 0, addr res_buf, chr$("Бінарні операції"), 0
    invoke ExitProcess, 0
main endp
end
