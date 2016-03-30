#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t* bytes;
    unsigned int length;
    unsigned int size;
} bitarray_t;

bitarray_t* bitarray_new(unsigned int start_size) {
    bitarray_t* arr = (bitarray_t*)malloc(sizeof(bitarray_t));
    arr->bytes = (uint8_t*)malloc(start_size * sizeof(uint8_t));
    memset(arr->bytes, 0, start_size);
    arr->size = start_size;
    arr->length = 0;
    return arr;
}

bitarray_t* bitarray_read_from_file(FILE* fin) {
    fseek(fin, 0, SEEK_END);
    unsigned int size = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    bitarray_t* arr = bitarray_new(size);
    fread(arr->bytes, sizeof(uint8_t), size, fin);
    arr->length = size * 8;
    return arr;
}

void bitarray_write_to_file(bitarray_t* arr, FILE* fout) {
    fwrite(arr->bytes, sizeof(uint8_t), (arr->length / 8) + 1, fout);
}

void bitarray_free(bitarray_t* arr) {
    free(arr->bytes);
    free(arr);
}

void bitarray_grow(bitarray_t* arr, unsigned int new_size) {
    uint8_t* new_bytes = (uint8_t*)malloc(new_size * sizeof(uint8_t));
    memset(new_bytes, 0, new_size);
    memcpy(new_bytes, arr->bytes, arr->size);
    free(arr->bytes);
    arr->bytes = new_bytes;
    arr->size = new_size;
}

unsigned int bitarray_push_bit(bitarray_t* arr, bool v) {
    if (arr->length / 8 == arr->size) {
        bitarray_grow(arr, arr->size * 2);
    }

    if (v) {
        arr->bytes[arr->length / 8] |= (1 << (7 - (arr->length % 8)));
    }
    return arr->length++;
}

bool bitarray_get_bit(bitarray_t* arr, unsigned int addr) {
    return arr->bytes[addr / 8] & (1 << (7 - (addr % 8)));
}

unsigned int bitarray_push_byte(bitarray_t* arr, uint8_t ch) {
    if (arr->length / 8 == arr->size - 1) {
        bitarray_grow(arr, arr->size * 2);
    }

    if (arr->length % 8 == 0) {
        arr->bytes[arr->length / 8] = ch;
    } else {
        arr->bytes[arr->length / 8] |= ch >> (arr->length % 8);
        arr->bytes[(arr->length / 8) + 1] |= ch << (8 - (arr->length % 8));
    }
    arr->length += 8;
    return (arr->length - 8);
}

uint8_t bitarray_get_byte(bitarray_t* arr, unsigned int addr) {
    if (addr % 8 == 0) {
        return arr->bytes[addr / 8];
    }
    uint8_t byte1 = arr->bytes[addr / 8] << (addr % 8);
    uint8_t byte2 = arr->bytes[(addr / 8) + 1] >> (8 - (addr % 8));
    return byte1 | byte2;
}
