
#include "memory.h"
#include "shell.h"

uint8_t dynamic_memory[memory_size];
uint32_t n_blocks = 0;
uint32_t mem_left = memory_size; // Total

tree_t free_memory_tree;
tree_t * free_mem_tree = &free_memory_tree;
leaf_t free_mem_leafs[max_free_blocks];
uint16_t free_leaf_pile[max_free_blocks];
uint32_t flp_pointer = max_free_blocks - 1;

uint8_t ez[8000]; // error zone
uint32_t malloc_count = 0;
    
leaf_t * create_leaf_in_pile(uint32_t key, void * data )
{
    uint16_t location = free_leaf_pile[flp_pointer--];
    return create_leaf_there(key, data, &free_mem_leafs[location]);
}

void remove_leaf_from_pile(void * leaf_adr)
{
    uint32_t position = (leaf_adr - (void*)free_mem_leafs) / sizeof(leaf_t);
    
    printc(':');printc(position+'0');
    next_line();
    free_leaf_pile[++flp_pointer] = position;
}

void init_memory()
{
    for (uint32_t i = 0; i < max_free_blocks; i++)
        free_leaf_pile[flp_pointer - i] = i;
    free_mem_tree->root = create_leaf_in_pile(memory_size, dynamic_memory);
    free_mem_tree->size = 0;
}

// ..|ucode|n|data of size n|..|fcode|x|empty space of size x|..|<fcode(y)|empty space of size y|.. (0 when there's not 2o to have |1|0|) <- OLD MODEL
void * malloc(uint32_t requested_size)
{
    malloc_count++;
    uint32_t needed_size = requested_size + size_storing_size + 2; // 4 for storing the block size
    if (needed_size > mem_left) {
        print("not enough mem_left");
        return 0;
    }
    //printc('a');
    leaf_t * leaf_found = find_leaf(free_mem_tree, needed_size); // the pointer to the concerned leaf
    if (leaf_found == 0) {
        print("free block not found");
        return 0;
    }
    
    uint8_t * block_found = leaf_found->data;
    if(block_found < dynamic_memory || block_found > dynamic_memory+memory_size-1) {
        print("block->data not included in mem. a:");
        uint32_t a = (uint32_t)block_found;
        print(its_p((int)a, ez));
        return 0;
    }
    
    uint32_t leaf_size = leaf_found->key;
    uint32_t dif = leaf_size - needed_size;
    detach_leaf(free_mem_tree, leaf_size, block_found);
    if (dif > 0) { // update leaf and free block
        leaf_found->key = leaf_size - needed_size;
        leaf_found->data += needed_size;
        leaf_found->left = leaf_found->right = 0;
        leaf_found->height = 1;
        add_leaf_to_leaf(&free_mem_tree->root, leaf_found);
        carve_free_block(block_found + needed_size, leaf_size - needed_size); // re-carve the updated free block
    } else { // then equals
        remove_leaf_from_pile(leaf_found); // remove leaf from pile
    }
    
    mem_left -= needed_size;
    n_blocks++;
    
    block_found[0] = block_found[needed_size - 1] = ucode; // carve the be. and end bits for used blocks
    carve_uint32(requested_size, ++block_found); // carve the size of the block
    
    block_found += size_storing_size;
    
    return block_found;
}

uint8_t mfree(void * block_to_free)
{
    if((uint8_t*)block_to_free < dynamic_memory || (uint8_t*)block_to_free > dynamic_memory+memory_size-1)
        return 0;
    
    uint8_t can_fuse_with_nearby = 0;
    // HOW CAN I FIND THE NEARBY BLOCKS ?? D: -> ONE BYTE AT EACH SIDE OF EACH BLOCK ? BUT STILL CAN'T KNOW THEIR SIZES D:
    // OLD METHOD OF INDICATING SIZE AT BEGINNING TOO POSSIBLE, BUT HAS TO BE INDICATED AT THE END TOO (only for free space)
    
    uint8_t * data = block_to_free - size_storing_size - 1;
    uint32_t key = *(uint32_t*)(data+1) + size_storing_size + 2;
        carve_free_block(data, key);
        *data = *(data + key - 1) = fcode+3;
    uint8_t * next = data + key;
    uint32_t nkey = 0;
    leaf_t * nleaf = 0;
    if (*next == fcode) // then big free block (size > 127)
        nkey = *(uint32_t*)(next + 1); //free block have the exact size written
    else if (*next < fcode) // then tiny free block (size < fcode)
        nkey = *next;
    if (*next <= fcode) {
        nleaf = detach_leaf(free_mem_tree, nkey, next);
        *next = fcode+3;
        print("nleaf:");printijp((uint32_t)nleaf);
        //blah("next:", nleaf, 0);
        //if (nleaf != 0)
            key += nkey;
    }
    
    uint8_t * prev = data - 1;
    uint32_t pkey = 0;
    leaf_t * pleaf = 0;
    if (*prev == fcode) {
        pkey = *(uint32_t*)(prev - size_storing_size);
    } else if (*prev < fcode) {
        pkey = *prev;
    }
    if (*prev <= fcode) {
        *prev = fcode+3;
        prev -= pkey - 1;//(2 + size_storing_size);
        pleaf = find_leaf(free_mem_tree, pkey);
        //(leaf_t*)(prev - (uint8_t*)pkey + 1);
        print("pleaf:");printijp((uint32_t)data);
        printijp((uint32_t)pleaf);
        
        pleaf = detach_leaf(free_mem_tree, pkey, prev);
        printijp((uint32_t)pleaf);
        if (nleaf != 0) {
            write_serial_word("pleaf:");
            write_serial_word(its_p((uint32_t)pleaf, buffer));
            remove_leaf_from_pile(pleaf); //WHY NOT DEAD ? :(
            write_serial_word("done\n");
        }
        key += pkey;
        data -= pkey;
    }
    
    int32_t dif = key - (sizeof(leaf_t) + size_storing_size + 2);
    if (1/*dif < 0*/) {
        leaf_t * leaf = 0;
        if (nleaf != 0 || pleaf != 0) {
            leaf = (nleaf != 0)? nleaf : pleaf;
            leaf->key = key;
            leaf->data = data;
            leaf->left = leaf->right = 0;
            leaf->height = 1;
        } else {            
            leaf = create_leaf_in_pile(key, data);
        }
        add_leaf_to_leaf(&(free_mem_tree->root), leaf);
        carve_free_block(data, key);
    } else {
        /*carve_used_block(data, sizeof(leaf_t));
        leaf_t * leaf = (void*)data + 1 + size_storing_size;
        leaf->key = key;
        leaf->data = data;
        leaf->left = 0;     leaf->right = 0;    leaf->height = 1;
        add_leaf_to_leaf(&(free_mem_tree->root), leaf);
        data += (sizeof(leaf_t) + size_storing_size + 2);
        key = dif;
        if (dif > 0)
            carve_free_block(data, key);*/
    }
    //strcopy(buffer+strlen(buffer), "]");
}

void carve_used_block(uint8_t * block, uint32_t bsize)
{
    *block = *(block + bsize + size_storing_size + 1) = ucode;
    carve_uint32(bsize, block + 1);
}

void carve_free_block(uint8_t * block, uint32_t bsize)
{
    if (bsize >= (1 + size_storing_size)*2) {
        *block = *(block + bsize - 1) = fcode;
        carve_uint32(bsize, block + 1);
        carve_uint32(bsize, block + bsize - size_storing_size - 1);
    } else if (bsize > 0)
        *block = *(block + bsize - 1) = (uint8_t)bsize;
}

char stuffx(int x)
{
    return itc(dynamic_memory[x]);
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
