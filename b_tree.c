
#include "b_tree.h"
#include "shell.h"

struct tree {
    leaf_t * root;
    uint32_t size;
    //uint8_t (*comp) (uint32_t, uint32_t);
};

struct leaf {
    uint32_t key;
    void * data;
    leaf_t * left, * right;
    uint8_t height;
};

char strbuffer[20480];

tree_t * init_b_tree(uint32_t root_key, uint8_t * root_data)//, uint8_t (*comp)(uint32_t, uint32_t))
{
    tree_t * tree = malloc(sizeof(tree_t)); //DON'T USE MALLOC BEFORE END OF INIT_MALLOC
    tree->root = create_leaf(root_key, root_data);
    tree->size = 1;
    //tree->comp = comp;
}

uint8_t comp(uint32_t l1k, uint32_t l2k, void * l1d, void * l2d)
{
    if (l1k > l2k)
        return 1;
    else if (l1k < l2k)
        return -1;
    else {
        if ((void*)l1d > (void*)l2d)
            return 1;
        else if ((void*)l1d < (void*)l2d)
            return -1;
        else //equal
            return 2;
    }
}

//char STFU(tree_t * tree, int a, int b) { return tree->comp(a,b); }

int max(int a, int b)
{
    return a > b ? a : b;
}

int height(leaf_t * p)
{
    return p ? p -> height : 0;
}

void recalc(leaf_t * p)
{
    p->height = 1 + max(height(p->left), height(p->right));
}

leaf_t * rotate_right(leaf_t * p)
{
    leaf_t * q = p->left;

    p->left = q->right;
    q->right = p;

    recalc(p);
    recalc(q);

    return q;
}

leaf_t * rotate_left(leaf_t * p)
{
    leaf_t * q = p->right;
    p->right = q->left;
    q->left = p;

    recalc(p);
    recalc(q);

    return q;
}

leaf_t * balance(leaf_t * p)
{
    recalc(p);
    
    if ( height(p->left) - height(p->right) == 2 )
    {
        if ( height(p->left->right) > height(p->left->left) )
            p->left = rotate_left(p->left);
        return rotate_right(p);
    }
    else if ( height(p->right) - height(p->left) == 2 )
    {
        if ( height(p->right->left) > height(p->right->right) )
            p->right = rotate_right(p->right);
        return rotate_left(p);
    }

    return p;
}

void blah(char * m, uint32_t key, tree_t * tree)
{
    /*buffer[strlen(buffer)] = '(';
    strcopy(buffer+strlen(buffer), m);
    buffer[strlen(buffer)] = (char)(key & 0xff) + '0';
    buffer[strlen(buffer)] = ')';
    if (tree != 0) {
//         tree_to_string(tree);
//         strcopy(buffer+strlen(buffer), strbuffer);
    }*/
}

void insert_data(tree_t * tree, uint32_t key, void * data)
{
    blah("insert ", key, tree);
    add_leaf_to_leaf(&(tree->root), create_leaf(key, data));
    tree->size++;
}

void add_leaf_to_leaf(leaf_t * * top, leaf_t * leaf) // WHAT IF THE SPACE USED TO REFERENCE EMPTY SPACE IS THE EMPTY SPACE ITSELF ?
{
    blah("add ", leaf->key, 0);
    leaf_t * * previous[height(*top)];
    uint8_t h = height(*top) - 1;
    leaf_t * * current = top;
    while (*current != 0) {
        ((*current)->height)++;
        previous[h--] = current;
        current = (comp(leaf->key, (*current)->key, leaf->data, (*current)->data) == 1) ? &((*current)->right) : &((*current)->left);
    }
    *current = leaf;
    for (uint8_t i = 0; i < height(*top); i++)
        *previous[i] = balance(*previous[i]);
}

void * find_leaf(tree_t * tree, uint32_t key) //or tiniest bigger if not found
{
    uint8_t h = height(tree->root) - 1;
    leaf_t * current = tree->root, * last = current;
    while (current != 0 && !(key == current->key) ) {
        if (current->key > key)
            last = current;
        current = /*tree->comp*/(key > current->key) ? current->right : current->left; // if a > b (in the memory_tree case)
    }
    if (current == 0 && /*tree->comp(*/key < last->key)// == -1) // if not bigger, then at the biggest tree element already
        current = last;
    return current; // ret 0 if no room
}

leaf_t * detach_leaf(tree_t * tree, uint32_t key, void * data)
{
    //printc('v');
    blah("detach ", key, tree);
    uint8_t h = height(tree->root) - 1;
    leaf_t * * current = &(tree->root);
    leaf_t * * previous[height(tree->root)];
    while (*current != 0 && comp(key, (*current)->key, data, (*current)->data) != 2) {
        previous[h--] = current;
        current = (comp(key, (*current)->key, data, (*current)->data) == 1) ? &((*current)->right) : &((*current)->left);
    }//printc('x');
    leaf_t * tmp, * to_remove = *current;
    if (*current == 0)
        return 0; // else causes problems with what's next
    
    if (height((*current)->left) < height((*current)->right)) {
        tmp = (*current)->left;
        *current = (*current)->right;
    } else {
        tmp = (*current)->right;
        *current = (*current)->left;
    }
    if (tmp != 0)
        add_leaf_to_leaf(current, tmp);
    
    for (uint8_t i = h+1; i < height(tree->root); i++)
        *previous[i] = balance(*previous[i]);
    leaf_t * ret = to_remove;
    return ret;
}

void * remove_leaf(tree_t * tree, uint32_t key, void * data) // detach and free leaf memory
{
    blah("remove ", key, tree);
    leaf_t * ret = detach_leaf(tree, key, data);
    mfree(ret);
    tree->size -= 1;
    //return ret;
}

void free_leaf(leaf_t * leaf)
{
    if (leaf->left != 0)
        free_leaf(leaf->left);
    if (leaf->right != 0)
        free_leaf(leaf->right);
    mfree(leaf);
    print_mem();
}

void free_tree(tree_t * tree)
{
    free_leaf(tree->root);
    mfree(tree);
}

leaf_t * create_leaf_there(uint32_t key, void * data, void* addr)
{
    leaf_t * leaf = addr;
    leaf->key = key;
    leaf->data = data;
    leaf->left = 0;
    leaf->right = 0;
    leaf->height = 1;
    return leaf;
}

leaf_t * create_leaf(uint32_t key, void * data)
{
    return create_leaf_there(key, data, malloc(sizeof(leaf_t)));
}

char * tree_to_string(const tree_t * tree)
{
    uint32_t wideness = 2;
    for(uint8_t i = 0; i < height(tree->root); i++) wideness *= 2;
    //wideness *= 2;
    uint32_t str_size = (wideness+1)*(height(tree->root)) + 4; // h:H\0
    char * str = strbuffer;//malloc(sizeof(char)*str_size);
    if (str == 0) return 0;
    for(uint32_t i = 0; i < str_size; i++)
        str[i] = '_';
    ts_append(str, tree->root, wideness/2 - 1, 0, wideness, height(tree->root));
    for(uint8_t d = 1; d <= height(tree->root); d++)
        str[d*(wideness + 1) - 1] = '\n';
    //str[height(tree->root)*(wideness + 1) - 1] = 'n';
    str[str_size - 1] = 0;//\0';
    str[str_size - 2] = height(tree->root) + '0';
    *(str + str_size - 4) = 'h';    *(str + str_size - 3) = ':';
    return str;
}

void ts_append(char * str, leaf_t * leaf, uint32_t horizontal_i, uint8_t depth, uint32_t wideness, uint8_t mheight)
{
    str[horizontal_i + depth*(wideness+1)] = '0' + (char)(leaf->key & 0xff);
    uint32_t dif = 2;
    for (uint8_t i = 0; i < mheight - depth; i++) dif*=2;
    dif /= 4;
    if (leaf->right != 0) ts_append(str, leaf->right, horizontal_i+dif, depth+1, wideness, mheight);
    if (leaf->left != 0) ts_append(str, leaf->left, horizontal_i-dif, depth+1, wideness, mheight);
}

