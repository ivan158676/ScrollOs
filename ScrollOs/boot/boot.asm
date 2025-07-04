[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Инициализация экрана
    mov ax, 0x0003
    int 0x10
    mov ax, 0xB800
    mov es, ax

    ; Вывод заголовка
    mov si, welcome_msg
    mov di, 0              ; Верхняя строка
    mov ah, 0x0F           ; Белый на чёрном
    call print_line

    ; Анимация и звук
    call animate_with_sound

main_loop:
    ; Ожидание клавиши
    mov ah, 0x00
    int 0x16

    ; Проверка на Enter
    cmp ah, 0x1C
    je load_kernel

    jmp main_loop

load_kernel:
    ; Очистка экрана
    mov ax, 0x0003
    int 0x10
    xor di, di

    ; Сообщение о загрузке (ваша версия!)
    mov si, kernel_msg
    mov ah, 0x0A           ; Зелёный текст
    call print_line
    ; Проверяем поддержку VBE
init_video:
    ; Пробуем 1024x768
    mov ax, 0x4F02
    mov bx, 0x4115       ; Режим 0x115 (1024x768x256)
    int 0x10
    jc .vga_fallback     ; Если ошибка — fallback

    ; Сохраняем адрес фреймбуфера
    mov dword [0x7E00], 0xFD000000  ; Для ядра
    ret

.vga_fallback:
    ; Стандартный VGA 640x480
    mov ax, 0x0012       ; 640x480x16
    int 0x10
    mov dword [0x7E00], 0  ; Ядро поймёт, что VBE не работает
    ret
    jmp $
   

; ====== ПРОЦЕДУРЫ ======
animate_with_sound:
    ; Включить динамик
    in al, 0x61
    or al, 0x03
    out 0x61, al

    ; Проиграть 3 ноты
    mov cx, 3
.sound_loop:
    push cx
    mov bx, cx
    dec bx
    shl bx, 1
    mov ax, [frequencies + bx]
    call play_sound
    pop cx
    loop .sound_loop

    ; Выключить динамик
    in al, 0x61
    and al, 0xFC
    out 0x61, al
    ret

play_sound:
    ; Установка частоты
    mov dx, ax
    mov al, 0xB6
    out 0x43, al
    mov ax, dx
    out 0x42, al
    mov al, ah
    out 0x42, al
    mov cx, 0x0FFF
.delay:
    nop
    loop .delay
    ret

print_line:
    ; DI = позиция, SI = строка, AH = цвет
.print_loop:
    lodsb
    test al, al
    jz .done
    stosw
    jmp .print_loop
.done:
    ret

; ====== ДАННЫЕ ======
welcome_msg db "Welcome to ScrollOS", 0
kernel_msg db "Loading ScrollOS kernel...", 0  ; <-- Вот здесь ваше изменение!
frequencies dw 9121, 6833, 4063  ; Ноты C5, E5, G4

times 510-($-$$) db 0
dw 0xAA55
