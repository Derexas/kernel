

#define memory_size 16777216 //2^24 bytes (16MB)
#define size_storing_size 4
#define ucode 133
#define fcode 128

void init_memory();

void * malloc(uint32_t requested_size);

uint8_t mfree(uint8_t * free_block);

void carve_free_block(uint8_t * block, uint32_t bsize);

uint32_t read_uint32(uint8_t * block);

void carve_uint32(uint32_t data, uint8_t * block);

uint32_t * memory_info();