include \masm64\include64\masm64rt.inc

.data
    ; 'О е   і   и'
    ; 'Омельніцький'
    ; 'А   і'
    ; 'Андрій'

    ; Голосні літери прізвища
    L1 dq "О"
    L2 dq "е"
    L3 dq "і"
    L4 dq "и"

    ; Голосні букви імені
    N1 dq "А"
    N2 dq "і"

    message_fmt db "Варіант №5",10,
       "Сума кодів гласних літер прізвища = %d",10,
       "Добуток кодів голосних літер імені = %d",10,10,
       "Результат довільної операцій = %d",10,10,
       "Автор: Омельніцький А.М.",0

    res_buf db 512 dup(0)

.code
; В x64 Windows:
; можна звертатися по назві
; 1 находится в RCX
; 2 находится в RDX
; 3 находится в R8
; 4 находится в R9
; 5.. через стек

sum_4_letters proc a1:DQ, a2:DQ, a3:DQ, a4:DQ
    mov     rax, rcx        ; a1
    add     rax, rdx        ; + a2
    add     rax, r8         ; + a3
    add     rax, r9         ; + a4
    ret
sum_4_letters endp

product_2_letters proc a1:DQ, a2:DQ
    mov     rax, rcx        ; a1
    mul     rdx             ; * a2
    ret
product_2_letters endp

random_calc_6 proc a1:DQ, a2:DQ, a3:DQ, a4:DQ, b1:DQ, b2:DQ
    ; (L1 + L2 + L3 + L4) * (N1 + N2)

    ; (L1 + L2 + L3 + L4)
    mov     rax, rcx        ; a1
    add     rax, rdx        ; + a2
    add     rax, r8         ; + a3
    add     rax, r9         ; + a3
    push rax

    ; (N1 + N2)
    mov     rax, b1         ; b1
    add     rax, b2              ; + b2

    ; res
    pop rbx
    mul rbx

    ret
random_calc_6 endp

main proc
    ; Сума кодів голосних прізвища
    invoke sum_4_letters, L1, L2, L3, L4
    mov r10, rax

    ; Добуток голосних імені
    invoke product_2_letters, N1, N2
    mov r11, rax

    invoke random_calc_6, L1, L2, L3, L4, N1, N2
    mov r12, rax

    invoke wsprintf, addr res_buf, addr message_fmt, r10, r11, r12
    invoke MessageBox, 0, addr res_buf, chr$("Процедури з параметрами"), 0
    invoke ExitProcess, 0
main endp
end
