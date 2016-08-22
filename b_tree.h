
#include "memory.h"

typedef struct tree tree_t;

typedef struct leaf leaf_t;

tree_t * init_tree(void * root_data);

void blah(char * m, uint32_t key, tree_t * tree);

void insert_data(tree_t * tree, uint32_t key, void * data);

void add_leaf_to_leaf(leaf_t * * tree, leaf_t * leaf);

void * find_leaf(tree_t * tree, uint32_t key);

leaf_t * detach_leaf(tree_t * tree, uint32_t key, void * data);

void * remove_leaf(tree_t * tree, uint32_t key, void * data);

leaf_t * create_leaf(uint32_t key, void * data);

void ts_append(char * str, leaf_t * leaf, uint32_t horizontal_i, uint8_t depth, uint32_t wideness, uint8_t mdepth);

int height(leaf_t * p);

char * tree_to_string(const tree_t * tree);
