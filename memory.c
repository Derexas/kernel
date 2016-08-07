
#include "memory.h"

uint8_t dynamic_memory[memory_size];
uint32_t n_blocks = 0;
uint32_t mem_left = memory_size; // Total

tree_t * free_mem_tree;

uint8_t aa[1024]; uint32_t malloc_count = 0;
    
/*uint8_t comp23 (uint32_t k1, uint32_t k2) {
    return (k1 > k2) ? 1 : (k1 < k2) ? -1 : 0;
};*/

void init_memory()
{
    uint32_t counter = 0;
    void * a(uint32_t b) {
        counter += b;
        return (void*)(counter - b + (uint32_t)dynamic_memory);
    };
    *(char*)a(1) = ucode;
    carve_uint32(sizeof(tree_t), a( sizeof(uint32_t) )); //carve the mem tree size at the beginning
    free_mem_tree = a( sizeof(tree_t) );
    *(char*)a(1) = ucode;
    *(char*)a(1) = ucode;
    carve_uint32(sizeof(leaf_t), a( sizeof(uint32_t) )); //carve the root size
    leaf_t * leaf = a( sizeof(leaf_t) );
    *(char*)a(1) = ucode;
    mem_left -= counter;
    *(char*)a(0) = fcode; //carve the free memory block
    carve_uint32(mem_left, a(0)+1); //carve the free mem size
    dynamic_memory[memory_size - 1] = fcode;
    carve_uint32(mem_left, dynamic_memory + memory_size - 4 - 1);
    
    leaf->key = mem_left;
    leaf->data = a( 0 );
    leaf->left = 0;    leaf->right = 0;    leaf->height = 1;
    free_mem_tree->root = leaf;
    free_mem_tree->size = 1;
    counter += 10;
}

//char STF() { return (char)(free_mem_tree->comp(1, 2)) + '0'; }

// ..|ucode|n|data of size n|..|fcode|x|empty space of size x|..|<fcode(y)|empty space of size y|.. (0 when there's not 2o to have |1|0|) <- OLD MODEL
void * malloc(uint32_t requested_size)
{malloc_count++;
    uint32_t needed_size = requested_size + size_storing_size + 2; // 4 for storing the block size
    if (needed_size > mem_left)
        return 0;
    
    leaf_t * leaf_found = find_leaf(free_mem_tree, needed_size); // the pointer to the concerned leaf
    if (leaf_found == 0) {
        return 0;
    }
    
    uint8_t * block_found = leaf_found->data;
    if(block_found < dynamic_memory || block_found > dynamic_memory+memory_size-1)
        return 0;
    
    uint32_t leaf_size = leaf_found->key;
    
    if (leaf_size > needed_size) { // update leaf and free block
        detach_leaf(free_mem_tree, leaf_size, block_found);
        leaf_found->key = leaf_size - needed_size;
        leaf_found->data += needed_size;
        add_leaf_to_leaf(&free_mem_tree->root, leaf_found);
        carve_free_block(block_found + needed_size, leaf_size - needed_size); // re-carve the updated free block        
    } else {        
        remove_leaf(free_mem_tree, leaf_size, block_found); // remove leaf
    }
    
    mem_left -= needed_size;
    n_blocks++;
    
    block_found[0] = block_found[needed_size - 1] = ucode; // carve the be. and end bits for used blocks
    carve_uint32(requested_size, ++block_found); // carve the size of the block
    
    block_found += size_storing_size;
    
    return block_found;
}

uint8_t mfree(uint8_t * block_to_free)
{
    if(block_to_free < dynamic_memory || block_to_free > dynamic_memory+memory_size-1)
        return 0;
    
    uint8_t can_fuse_with_nearby = 0;
    // HOW CAN I FIND THE NEARBY BLOCKS ?? D: -> ONE BYTE AT EACH SIDE OF EACH BLOCK ? BUT STILL CAN'T KNOW THEIR SIZES D:
    // OLD METHOD OF INDICATING SIZE AT BEGINNING TOO POSSIBLE, BUT HAS TO BE INDICATED AT THE END TOO (only for free space)
    
    uint32_t key = *(uint32_t*)(block_to_free - size_storing_size);
    uint8_t * data = block_to_free;
    
    uint8_t * next = block_to_free + key;
    if (*next == fcode) // then big free block (size > 127)
        key += 1 + size_storing_size + *(uint32_t*)(next + 1);
    else if (*next < fcode) // then tiny free block (size < fcode)
        key += *next;
    if (*next <= fcode)
        remove_leaf(free_mem_tree, key, data);
    
    uint8_t * prev = block_to_free - size_storing_size - 1;
    if (*prev == fcode) {
        key += 1 + size_storing_size + *(uint32_t*)(prev - 1 - size_storing_size);
        data -= *(uint32_t*)(prev - 1 - size_storing_size);
    } else if (*prev < fcode) {
        key += *prev;
        data -= *prev;
    }
    
    if (!can_fuse_with_nearby)
        insert_data(free_mem_tree, key, block_to_free);
}

void carve_free_block(uint8_t * block, uint32_t bsize)
{
    if (bsize > (1 + size_storing_size)*2) {
        *block = *(block + bsize - 1) = fcode;
        carve_uint32(bsize, block + 1);
        carve_uint32(bsize, block - size_storing_size - 1);
    } else
        *block = *(block + bsize - 1) = (uint8_t)bsize;
}

char stuffx(int x)
{
    return itc(dynamic_memory[x]);
}

char stuffa(int x)
{
    return itc(aa[x]);
}

uint32_t read_uint32(uint8_t * block)
{
    uint32_t c_block = 0;
    for (uint32_t i = 0; i < sizeof(uint32_t); i++) {
        c_block |= (uint32_t)block[i] << (sizeof(uint32_t) - i - 1)*8;
    }
    return c_block;
}

void carve_uint32(uint32_t data, uint8_t * block)
{
    for (uint32_t i = 0; i < sizeof(uint32_t); i++) {
        block[i] = (data >> (i/*sizeof(uint32_t) - i - 1*/)*8) & 0xff; // Carve the size of the block at it's 4 first bytes
    }
}

uint32_t * memory_info()
{
    uint32_t data_size = 4 + 1; //(1 for the data_size)
    uint32_t * infos = malloc(data_size * sizeof(uint32_t));
    infos[0] = data_size;
    infos[1] = memory_size;
    infos[2] = mem_left;
    infos[3] = n_blocks;
    infos[4] = (memory_size - mem_left) * 1000 / memory_size;
    return infos;
}
