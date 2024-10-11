#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_DATA_LENGTH 255 // максимум 255 байт

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

// Получение сообщения из входного потока (по одному байту)
bool receive_message(ProtocolMessage *msg) {
    init_protocol_message(msg);

    // Чтение преамбулы
    for (int i = 0; i < sizeof(msg->preamble); i++) {
        if (fscanf(stdin, "%hhx", &msg->preamble[i]) != 1) { // Считываем в шестнадцатеричном формате
            return false; 
        }
    }

    // Чтение длины сообщения
    if (fscanf(stdin, "%hhu", &msg->length) != 1) { // Считываем длину
        return false; 
    }

    // Чтение адреса источника и адреса получателя
    if (fscanf(stdin, "%hhx %hhx", &msg->src_addr, &msg->dest_addr) != 2) {
        return false; 
    }

    // Чтение данных
    if (msg->length > MAX_DATA_LENGTH) {
        return false; 
    }
    
    for (int i = 0; i < msg->length; i++) {
        if (fscanf(stdin, "%hhx", &msg->data[i]) != 1) { // Считываем данные
            return false; 
        }
    }

    // Вычисление CRC и его добавление в сообщение
    msg->crc = calculate_crc(msg);

    return true;
}

int main() {
    ProtocolMessage message;

    // Пояснение о формате ввода
    printf("Please enter the message in the following format:\nPreamble (4 bytes): 0x01 0x02 0x03 0x04\nLength of message (1 byte): N (number of data bytes)\n");
    printf("Source Address (1 byte): 0x10\nDestination Address (1 byte): 0x20\nData (N bytes): 0xAA 0xBB 0xCC\n");
    printf("Example input: 01 02 03 04 03 10 20 AA BB CC\n");

    if (receive_message(&message)) {
        if (verify_crc(&message)) {
            printf("Message successfully received and verified.\n");
        } else {
            printf("CRC check error.\n");
        }
        
        // Вывод информации о сообщении(на всякий случай)
        printf("Preamble: ");
        for (int i = 0; i < sizeof(message.preamble); i++) {
            printf("%02X ", message.preamble[i]);
        }
        printf("\nLength: %d\n", message.length);
        printf("Source Address: %02X\n", message.src_addr);
        printf("Destination Address: %02X\n", message.dest_addr);
        printf("Data: ");
        for (int i = 0; i < message.length; i++) {
            printf("%02X ", message.data[i]);
        }
        printf("\nCRC: %02X\n", message.crc);
        
    } else {
        printf("Message received error.\n");
    }

    return 0;
}