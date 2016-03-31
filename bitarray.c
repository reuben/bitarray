#include "bitarray.h"

#include <stdio.h>
#include <assert.h>

void bitarray_push_literal(bitarray_t* arr, uint8_t lit) {
    bitarray_push_bit(arr, false);
    bitarray_push_byte(arr, lit);
}

void bitarray_push_pointer(bitarray_t* arr, uint16_t length, uint16_t offset) {
    length -= 3;
    offset -= 1;
    bitarray_push_bit(arr, true);
    bitarray_push_byte(arr, (uint8_t)length);
    bitarray_push_byte(arr, (offset >> 8) << 1); // 7 most significant bits
    arr->length--; // sue me
    bitarray_push_byte(arr, (uint8_t)offset); // 8 least significant bits
}

int main(void) {
    bitarray_t* a = bitarray_new(2);
    bitarray_push_literal(a, 'a');
    bitarray_push_literal(a, 'b');
    bitarray_push_literal(a, 'a');
    bitarray_push_pointer(a, 258, 0x8000);
    bitarray_push_literal(a, 'd');
    bitarray_push_literal(a, 'a');
    bitarray_push_pointer(a, 258, 0x8000);
    bitarray_push_literal(a, 'c');

    FILE* fout = fopen("bitarray.out", "wb");
    bitarray_write_to_file(a, fout);
    fclose(fout);
    bitarray_free(a);

    fout = fopen("bitarray.out", "rb");
    a = bitarray_read_from_file(fout);
    fclose(fout);

    uint8_t ch = bitarray_get_byte(a, 1);
    printf("0x%X %c\n", ch, ch);
    ch = bitarray_get_byte(a, 10);
    printf("0x%X %c\n", ch, ch);
    ch = bitarray_get_byte(a, 19);
    printf("0x%X %c\n", ch, ch);

    bitarray_free(a);
    return 0;
}
