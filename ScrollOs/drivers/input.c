#include <stdint.h>
#include "io.h"  // Для inb/outb

// ===== Конфигурация =====
#define PS2_DATA    0x60
#define PS2_CMD     0x64
#define USB_IO_BASE 0xE000  // Условный адрес USB-контроллера

// Состояние драйвера
static enum { 
    INPUT_PS2, 
    INPUT_USB 
} current_driver = INPUT_PS2;

// ===== PS/2 =====
uint8_t ps2_read() {
    while (!(inb(PS2_CMD) & 1)); // Ждём данные
    return inb(PS2_DATA);
}

void ps2_init() {
    outb(PS2_CMD, 0xAE);  // Включить первую PS/2-клавиатуру
}

// ===== USB (упрощённо) =====
uint8_t usb_read() {
    // Эмуляция USB (в реальности — чтение через EHCI/XHCI)
    return inb(USB_IO_BASE);
}

void usb_init() {
    outb(USB_IO_BASE + 1, 0x1);  // Условная инициализация USB
}

// ===== Общий обработчик =====
void input_init() {
    ps2_init();
    if (inb(USB_IO_BASE) != 0xFF) {  // Проверяем, есть ли USB
        usb_init();
        current_driver = INPUT_USB;
    }
}

uint8_t read_key() {
    return (current_driver == INPUT_PS2) ? ps2_read() : usb_read();
}