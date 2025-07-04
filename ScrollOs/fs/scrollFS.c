#include <stdint.h>

#define SECTOR_SIZE  512
#define MAX_FILES    128

typedef struct {
    char name[32];
    uint32_t size;
    uint32_t start_sector;
} FileEntry;

typedef struct {
    FileEntry files[MAX_FILES];
    uint32_t total_sectors;
} ScrollFS;

static ScrollFS fs;

// Инициализация ФС (читаем MBR или создаём новую)
void fs_init() {
    fs.total_sectors = *(uint32_t*)0x1000;  // Читаем из загрузчика
    // ... (загрузка таблицы файлов)
}

// Поиск файла
FileEntry* fs_find(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fs.files[i].name, name) == 0) {
            return &fs.files[i];
        }
    }
    return NULL;
}

// Чтение файла в буфер
void fs_read(FileEntry* file, void* buffer) {
    uint8_t* sector = (uint8_t*)(0x100000 + file->start_sector * SECTOR_SIZE);
    memcpy(buffer, sector, file->size);
}