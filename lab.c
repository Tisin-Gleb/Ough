#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
    if (p < 90) {
        return r % 128;
    }
    if (p < 95) {
        return r % 16384;
    }
    if (p < 99) {
        return r % 2097152;
    }
    return r % 268435455;
}

/*int encode_file(const char *in_file_name, const char *out_file_name)
{
	FILE *comp;
	if ( (comp = fopen("uncompresse.dat", "wb") == NULL) )
		return -1;
	
	fseek(comp, 0,SEEK_END);
	
	for (i = 0; i < ftell(comp); i++){
		fread(comp);
	}
	
}
*/
int main()
{
	FILE *uncompressed, *compressed;
	uint32_t buffer[1000000];
	uint8_t buf[4];
	if ( (uncompressed = fopen("uncompressed.dat", "wb") ) == NULL)
		return -1;
		
	for(int i = 0; i < 1000000; i++){
		buffer[i] = generate_number();
	}
	fwrite(buffer, sizeof(uint32_t), 1000000, uncompressed);
	fclose(uncompressed);
	
	if ( (compressed = fopen("compressed.dat", "wb") ) == NULL)
		return -1;
		
	for(int i = 0; i < 1000000; i++){
		size_t length = encode_varint(buffer[i], buf);
		fwrite(buf, sizeof(uint8_t), length, compressed);
	}
	fclose(compressed);
	
	
	if ( (uncompressed = fopen("uncompressed.dat", "rb") ) == NULL)
		return -1;
	if ( (compressed = fopen("compressed.dat", "rb") ) == NULL)
		return -1;
	
	int flag = 0;
	
	for (int i = 0; i < 1000000; i++) {
        uint32_t value_uncom = 0;
        uint32_t value_com = 0;
        
        fread(&value_uncom,
			  sizeof(uint8_t),
			  sizeof(uint32_t),
			  uncompressed);
        fread(&value_com,
              sizeof(uint8_t),
              buffer[i],
              compressed);

        const uint8_t* value = (uint8_t*)&value_com;
        value_com = decode_varint(&value);
        if (value_uncom != value_com) {
			flag = 1;
		}
	}
    if (flag == 1){
			puts("bin files arent equal");
	}else{
		puts("II bin files are equal II");
	}
	
	fclose(uncompressed);
	fclose(compressed);
	

	
	return 0;
}
