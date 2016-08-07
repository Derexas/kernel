
#include "memory.h"

typedef struct tree tree_t;

typedef struct leaf leaf_t;

tree_t * init_tree(void * root_data);

void insert_data(tree_t * tree, uint32_t key, void * data);

void add_leaf_to_leaf(leaf_t * * tree, leaf_t * leaf);

void * find_leaf(tree_t * tree, uint32_t key);

void * detach_leaf(tree_t * tree, uint32_t key, void * data);

void * remove_leaf(tree_t * tree, uint32_t key, void * data);

leaf_t * create_leaf(uint32_t key, void * data);

void ts_append(char * str, leaf_t * leaf, uint32_t horizontal_i, uint8_t depth, uint32_t wideness, uint8_t mdepth);

int height(leaf_t * p);