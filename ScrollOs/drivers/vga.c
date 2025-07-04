#include <stdint.h>

// Режимы
#define MODE_VBE  1
#define MODE_VGA  0

static uint8_t video_mode;
static uint32_t* framebuffer;

void VGA_Init() {
    // Читаем результат из загрузчика
    uint32_t fb_addr = *(uint32_t*)0x7E00;

    if (fb_addr != 0) {
        video_mode = MODE_VBE;
        framebuffer = (uint32_t*)fb_addr;
    } else {
        video_mode = MODE_VGA;
    }
}

void VGA_Clear(uint32_t color) {
    if (video_mode == MODE_VBE) {
        for (int i = 0; i < 1024*768; i++) {
            framebuffer[i] = color;
        }
    } else {
        // Для VGA 640x480 (16 цветов)
        uint8_t bg = color & 0x0F;
        __asm__ volatile (
            "mov $0x06, %%ah\n"  ; BIOS: установка цвета фона
            "mov %0, %%al\n"
            "int $0x10"
            :: "r" (bg)
        );
    }
}

void VGA_DrawPixel(int x, int y, uint32_t color) {
    if (video_mode == MODE_VBE) {
        framebuffer[y * 1024 + x] = color;
    } else {
        // В VGA режиме — через BIOS
        __asm__ volatile (
            "mov $0x0C, %%ah\n"  ; BIOS: рисование пикселя
            "mov %0, %%al\n"     ; Цвет
            "mov %1, %%cx\n"     ; X
            "mov %2, %%dx\n"     ; Y
            "int $0x10"
            :: "r" (color & 0x0F), "r" (x), "r" (y)
        );
    }
}