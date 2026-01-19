include \masm64\include64\masm64rt.inc

.data
    var_a dq 5
    var_e dq 12
    var_c dq 2

    message_fmt db "Варіант №5.",10,  ; форматований рядок для виводу результатів
                "Значення: a=%d, c=%d, e=%d",10,
                "Вираз: e/3c + ac",10,
                "Результат = %d",10,
                "Автор: Омельніцький А.М.",0

    domain_name db 256 dup(0)  ; буфер для імені домену
    domain_len dd 256          ; довжина буфера для GetComputerName
    domain_message_fmt db "%s",0       ; формат для домена

    res_buf db 512 dup(0)

.code
sub_task1 proc
    mov rax, var_c
    mov rbx, 3
    mul rbx        ; rax = 3 * c
    mov r10, rax
    mov rax, var_e
    xor rdx, rdx   ; очищення для ділення
    div r10        ; rax = e / 3c
    mov r10, rax   ; r10 = e / 3c
    mov rax, var_a
    mul var_c      ; rax = a * c
    add r10, rax   ; rax = e/3c + ac

    invoke wsprintf, addr res_buf, addr message_fmt, var_a, var_c, var_e, r10   ; форматування результату
    invoke MessageBox, 0, addr res_buf, chr$("Результат"), MB_ICONINFORMATION   ; вивід messagebox з результатами

    ret
sub_task1 endp

sub_task2 proc
    invoke GetComputerName, addr domain_name, addr domain_len
    invoke wsprintf, addr res_buf, addr domain_message_fmt, addr domain_name
    invoke MessageBox, 0, addr res_buf, chr$("Домен комп'ютера"), MB_ICONINFORMATION

    ret
sub_task2 endp

main proc
    call sub_task1
    call sub_task2
    invoke ExitProcess, 0
main endp
end
