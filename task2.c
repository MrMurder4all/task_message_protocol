#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_DATA_LENGTH 255 //максимум 255 байт

// Структура для хранения сообщения
typedef struct {
    uint8_t preamble[4]; // Преамбула
    uint8_t length;      // Длина сообщения
    uint8_t src_addr;    // Адрес источника
    uint8_t dest_addr;   // Адрес приёмника
    uint8_t data[MAX_DATA_LENGTH]; // Данные
    uint8_t crc;         // CRC
} ProtocolMessage;

// Инициализация структуры сообщения
void init_protocol_message(ProtocolMessage *msg) {
    memset(msg->preamble, 0, sizeof(msg->preamble));
    msg->length = 0;
    msg->src_addr = 0;
    msg->dest_addr = 0;
    memset(msg->data, 0, sizeof(msg->data));
    msg->crc = 0;
}

// Вычисление CRC
uint8_t calculate_crc(const ProtocolMessage *msg) {
    uint8_t crc = 0;
    
    for (int i = 0; i < sizeof(msg->preamble); i++) {
        crc ^= msg->preamble[i];
    }
    
    crc ^= msg->length;
    crc ^= msg->src_addr;
    crc ^= msg->dest_addr;

    for (int i = 0; i < msg->length; i++) {
        crc ^= msg->data[i];
    }

    return crc;
}

// Проверка корректности CRC
bool verify_crc(const ProtocolMessage *msg) {
    return calculate_crc(msg) == msg->crc;
}

// получение сообщения из входного потока (симуляция)
bool receive_message(ProtocolMessage *msg, uint32_t timeout_ms) {
    // Симуляция получения данных
    
    // (в реальной реализации здесь будет код для чтения данных)
    init_protocol_message(msg);
    
    memcpy(msg->preamble, (uint8_t[]){0x01, 0x02, 0x03, 0x04}, sizeof(msg->preamble));
    
    msg->length = 3; // Пример длины данных
    msg->src_addr = 0x10; // Пример адреса источника
    msg->dest_addr = 0x20; // Пример адреса приёмника
    
    memcpy(msg->data, (uint8_t[]){0xAA, 0xBB, 0xCC}, msg->length);
    
    // Вычисление CRC и его добавление в сообщение
    msg->crc = calculate_crc(msg);
    
    return true;
}

int main() {
    ProtocolMessage message;

    if (receive_message(&message, 1000)) {
        if (verify_crc(&message)) {
            printf("Message successfully received and verified.\n");
        } else {
            printf("CRC check error.\n");
        }
    } else {
        printf("Message received error.\n");
    }

    return 0;
}