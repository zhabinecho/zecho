#ifndef BREE_H
#define BREE_H

/**/
#define TREE_ORDER  5 /*child node count*/
#define MIN_KEY_NUM ((TREE_ORDER / 2))

#define INVALID_KEY ((int)-1)
#define TREE_SOLDIER    ((int)-1)

typedef unsigned int KeyType;
typedef struct bnode
{
    unsigned int key_count;
    KeyType keys[TREE_ORDER]; /*last one is soldier*/
    struct bnode *child_node[TREE_ORDER + 1];
    struct bnode *parent;
}bnode;


#define KEY_TO_RIGHT(index) ((index) + 1)
#define KEY_TO_LEFT(index) (index)
#define NODE_TO_RIGHT(index) ((index))
#define NODE_TO_LEFT(index) ((index) - 1)

#define tree_malloc malloc
#define tree_free free


//#define my_print(fmt,...) printf("%s:%d:"fmt, __FUNCTION__, __LINE__, __VA_ARGS__) 
#define my_print(...) \
do{ \
    printf("%s:%d:",__FUNCTION__, __LINE__);\
    printf(__VA_ARGS__); \
}while(0)

#define MY_ASSERT(con) \
do{ \
    if(!con) \
    {   \
        my_print("%s Assert Faild!\n", #con); \
    } \
}while(0)

struct bnode *btree_create(KeyType *data, unsigned int length);
struct bnode *btree_search(struct bnode *btree, KeyType data);
int btree_insert(struct bnode *btree, KeyType data);

#endif
