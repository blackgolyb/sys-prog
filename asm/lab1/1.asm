include \masm64\include64\masm64rt.inc

.data
    surname db 'Омельніцький', 0
    first_name db 'Андрій', 0
    ; 'О е   і   и'
    ; '0 2   6   10'
    ; '012345678910'
    ; 'Омельніцький'
    ; 'А   і'
    ; '0   4'
    ; '01234'
    ; 'Андрій'

    result dq 0,0            ; буфер для результату
    message_fmt db "Варіант №5.",10,  ; форматований рядок для виводу результатів
    "Сума кодів голосних букв у прізвищі ",10,
    "і добуток кодів голосних букв в імені.",10,10,
    "Сума кодів голосних букв прізвища = %d",10,
    "Добуток кодів голосних букв в імені = %d",10,10,
    "Автор: Омельніцький А.М.",0

    lpBuf1 db 32 dup(?),0  ; буфер для імені комп'ютера та системного каталогу
    lenname dq $-lpBuf1    ; довжина буфера

.code
sub_task1 proc
    movzx rax, byte ptr [surname]    ; літера "О"
    movzx rbx, byte ptr [surname+2]  ; літера "е"
    add rax, rbx
    movzx rbx, byte ptr [surname+6]  ; літера "і"
    add rax, rbx
    movzx rbx, byte ptr [surname+10] ; літера "и"
    add rax, rbx
    mov r8, rax

    movzx rax, byte ptr [first_name]    ; літера "А"
    movzx rbx, byte ptr [first_name+4]  ; літера "і"
    imul rax, rbx
    mov r9, rax

    invoke wsprintf, addr result, addr message_fmt, r8, r9   ; форматування результату
    invoke MessageBox, 0, addr result, chr$("Результати"), 0 ; вивід messagebox з результатами

    ret
sub_task1 endp

sub_task2 proc
    invoke GetComputerName, addr lpBuf1, addr lenname  ; отримання імені комп'ютера
    invoke MessageBox, 0, addr lpBuf1, chr$("Ім'я локального комп'ютера"), 0 ; вивід messagebox з результатами визначення імені локального комп'ютера

    invoke GetSystemDirectoryA, addr lpBuf1, addr lenname  ; отримання системного каталогу
    invoke MessageBox, 0, addr lpBuf1, chr$("Шлях до системного каталогу"), 0 ; вивід messagebox з визначеним шляхом до сис.каталогу

    ret
sub_task2 endp

main proc
    call sub_task1
    call sub_task2
    invoke ExitProcess, 0
main endp
end
