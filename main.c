#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

size_t encode_varint(uint32_t, uint8_t*);
uint32_t decode_varint(const uint8_t**);
uint32_t generate_number();
int write_and_read();

size_t encode_varint(uint32_t value, uint8_t* buf)
{
    assert(buf != NULL);
    uint8_t* cur = buf;
    while (value >= 0x80) {
        const uint8_t byte = (value & 0x7f) | 0x80;
        *cur = byte;
        value >>= 7;
        ++cur;
    }
    *cur = value;
    ++cur;
    return cur - buf;
}

uint32_t decode_varint(const uint8_t** bufp)
{
    const uint8_t* cur = *bufp;
    uint8_t byte = *cur++;
    uint32_t value = byte & 0x7f;
    size_t shift = 7;
    while (byte >= 0x80) {
        byte = *cur++;
        value += (byte & 0x7f) << shift;
        shift += 7;
    }
    *bufp = cur;
    return value;
}

uint32_t generate_number()
{
    const int r = rand();
    const int p = r % 100;
    if (p < 90)
        return r % 128;
    if (p < 95)
        return r % 16384;
    if (p < 99)
        return r % 268435455;
    return r % 268435455;
}
int write_and_read()
{
    srand(time(NULL));

    uint8_t buf[4];
    size_t sizeBuf[1000000];

    FILE* uncom;
    FILE* com;
    if ((uncom = fopen("uncompressed.dat", "wb")) == NULL)
    {
        return -1;
    }
    if ((com = fopen("compressed.dat", "wb")) == NULL)
    {
        return -1;
    }

    for (int i = 0; i < 1000000; i++) {
        uint32_t value = generate_number();
        fwrite(&value,
               sizeof(uint32_t),
               1,
               uncom);
        size_t size = encode_varint(value, buf);
        fwrite(buf, sizeof(uint8_t), size,
               com); 
        if (i < 1000000)
            sizeBuf[i] = size;
    }

    uint8_t flag = 0;

    fclose(uncom); 
    fclose(com);
    if ((uncom = fopen("uncompressed.dat", "rb")) == NULL) 
    {
        return -1; 
    }
    if ((com = fopen("compressed.dat", "rb")) == NULL) 
    {
        return -1;
    }

    for (int i = 0; i < 1000000; i++) {
        uint32_t valueUncom = 0;
        uint32_t valueCom = 0;

        fread(&valueUncom,
              sizeof(uint8_t),
              sizeof(uint32_t),
              uncom);
        fread(&valueCom,
              sizeof(uint8_t),
              sizeBuf[i],
              com);

        const uint8_t* value
                = (uint8_t*)&valueCom;
        valueCom = decode_varint(&value);
        if (valueUncom != valueCom) {
            flag = 1;
        };
    }
    if (flag == 1) {
        printf("Not compared\n");
    } else {
        printf("Compared\n");
    };
    return 0;
}
int main()
{
    write_and_read();
    return 0;
}

