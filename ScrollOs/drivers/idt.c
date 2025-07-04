#include <stdint.h>

struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct IDTEntry IDT[256];

void IDT_SetGate(uint8_t num, uint32_t handler) {
    IDT[num].offset_low = handler & 0xFFFF;
    IDT[num].selector = 0x08;
    IDT[num].zero = 0;
    IDT[num].type_attr = 0x8E;
    IDT[num].offset_high = (handler >> 16) & 0xFFFF;
}

void IDT_Init() {
    IDT_SetGate(0x21, (uint32_t)&Keyboard_ISR);  // IRQ1 (клавиатура)
    asm("lidt [idt_ptr]");
}