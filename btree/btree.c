#include<stdio.h>
#include<stdlib.h>
#include "btree.h"

static bnode_keys_dump(struct bnode *node)
{
    unsigned int i;
    for(i = 0; i < node->key_count; i++)
    {
        printf("%d ",node->keys[i]);
    }
    return;
}

void btree_dump(struct bnode *bnode, int level)
{
    unsigned int i;
    if(bnode == NULL)
    {
        return;
    }
    printf("Level[%d]node[%p]parent[%p]Keys[", level,bnode,bnode->parent);
    bnode_keys_dump(bnode);
    printf("]\n");
    for(i = 0; i < (bnode->key_count + 1); i++)
    {
        btree_dump(bnode->child_node[i], level + 1);
    }

    return;
}

struct bnode *btree_search(struct bnode *btree, KeyType data)
{
    struct bnode *find_node;
    unsigned int index;

    if(btree == NULL || btree->key_count == 0)
    {
        my_print("%s\n", "btree is empty!");
        return NULL;
    }
    find_node = btree;
    index = 0;
    while(1)
    {
        KeyType exist_data;
        exist_data = find_node->keys[index];
        my_print("find data in node %p\n", find_node);
        if(exist_data == data)
        {
            my_print("find data [%d] in %p\n", data, find_node);
            return find_node;
        }
        else if(exist_data < data)
        {
            index ++;
            continue;
        }
        /*left child node*/
        find_node = find_node->child_node[KEY_TO_LEFT(index)];
        /*find from first data*/
        index = 0;
        if(find_node == NULL || find_node->key_count == 0)
        {
            break;
        }
    }
    my_print("data[%d]not in the btree!\n", data);
    return NULL;
}

static struct bnode *bnode_create()
{
    struct bnode  *new_node = NULL;
    unsigned int i;
    new_node = (struct bnode *)tree_malloc(sizeof(struct bnode));
    if(new_node == NULL)
    {
        my_print("malloc null!\n");
        return NULL;
    }
    new_node->key_count = 0;
    new_node->parent = NULL;
    for(i = 0; i < TREE_ORDER + 1; i++)
    {
        new_node->child_node[i] = NULL;
    }
    for(i = 0; i < TREE_ORDER; i++)
    {
        new_node->keys[i] = INVALID_KEY;
    }
    new_node->keys[TREE_ORDER - 1] = TREE_SOLDIER; 

    return new_node;
}

static int get_node_index( struct bnode *parent, struct bnode *bnode)
{
    unsigned int i;

    for(i = 0; i < (parent->key_count + 1); i++)
    {
        my_print("child_node %p node %p\n", parent->child_node[i], bnode);
        if((parent->child_node[i]) == bnode)
        {
            return i;
        }
    }

    return -1;
}
static int get_data_index(struct bnode *node, KeyType data)
{
    unsigned int i;
    for(i = 0; i < node->key_count; i++)
    {
        if(node->keys[i] == data)
        {
            return i;
        }
    }

    return -1;
}

static struct bnode *get_left_brother(struct bnode *node)
{
    struct bnode *parent = node->parent;
    unsigned int index;
    if(parent == NULL)
    {
        return NULL;
    }
    index = get_node_index(parent, node);
    my_print("get node index %d\n", index);
    if(index == 0)
    {
        return NULL;
    }
    return parent->child_node[index - 1];
#if 0
    for(index = 0; index < TREE_ORDER + 1; index++)
    {
        if(node == parent->child_node[index])
        {
            return brother;
        }
        brother = node;
    }
#endif

    return NULL;
}
static struct bnode *get_right_brother(struct bnode *node)
{
    struct bnode *parent = node->parent;
    unsigned int index;
    if(parent == NULL)
    {
        return NULL;
    }
    index = get_node_index(parent, node);
    /*last one node*/
    if(index == parent->key_count)
    {
        return NULL;
    }
    return parent->child_node[index + 1];
}

static unsigned int bnode_is_leaf(struct bnode *node)
{
    if(node->child_node[0] == NULL)
    {
        my_print("node is leaf!\n");
        return 1;
    }
    return 0;
}

static unsigned int bnode_insert_key(struct bnode *node, unsigned int insert_index,
        unsigned int end_index, KeyType data)
{
    unsigned int i;/*must be int*/
    /*end index is last empty*/
    my_print("insert index: %d, end index: %d, data: %d\n", insert_index,
            end_index, data);
    for(i = (end_index); i > insert_index; i--)
    {
        node->keys[i] = node->keys[i - 1];
    }
    node->keys[insert_index] = data;
    node->key_count++;

    return 0;
}

static unsigned int bnode_insert_node(struct bnode *node, unsigned int insert_index,
        unsigned int end_index, struct bnode *insert_node)
{
    unsigned int i;/*must be int*/
    for(i = (end_index); i > insert_index; i--)
    {
        node->child_node[i] = node->child_node[i - 1];
    }
    node->child_node[i] = insert_node;

    return 0;
}

static int bnode_delete_key(struct bnode *node, unsigned int delete_index)
{
    KeyType ret_data; 
    unsigned int i;
    if(node->key_count == 0)
    {
        my_print("key_count is zero to be delete!\n");
        return -1;
    }
    ret_data = node->keys[delete_index];
    /*i from del index to count - 1*/
    for(i = delete_index; i < (node->key_count - 1); i++)
    {
        node->keys[i] = node->keys[i + 1];
    }
    node->keys[i] = INVALID_KEY;
    node->key_count --;

    return ret_data;
}

static int bnode_delete_node(struct bnode *node, unsigned int delete_index,
        unsigned int end_index)
{
    unsigned int i;
    if(node->key_count == 0)
    {
        my_print("key_count is zero to be delete!\n");
        return -1;
    }
    for(i = delete_index; i < end_index; i++)
    {
        node->child_node[i] = node->child_node[i + 1];
    }
    node->child_node[i] = NULL;

    return 0;
}


static int btree_bnode_split(struct bnode *parent, struct bnode *bnode)
{
    unsigned int mid_index;
    unsigned int i;
    unsigned int m;
    unsigned int insert_key_index = 0;
    KeyType mid_data;
    struct bnode *new_node = NULL;
    unsigned int old_node_count = bnode->key_count;

    mid_index = bnode->key_count / 2;
    /*save the data and set invalid*/
    mid_data = bnode->keys[mid_index];
    bnode->keys[mid_index] = INVALID_KEY;
    bnode->key_count--;

    new_node = bnode_create();
    if(new_node == NULL)
    {
        my_print("malloc null!\n");
        return -1;
    }
    /*init new node with split node data*/
    for(i = mid_index + 1, m = 0; i < old_node_count; i++,m++)
    {
        new_node->keys[m] = bnode->keys[i];
        new_node->child_node[KEY_TO_LEFT(m)] = 
            bnode->child_node[KEY_TO_LEFT(i)];
        if(new_node->child_node[KEY_TO_LEFT(m)])
        {
            new_node->child_node[KEY_TO_LEFT(m)]->parent = new_node;
        }
        new_node->key_count ++;

        bnode->keys[i] = INVALID_KEY;
        bnode->child_node[KEY_TO_LEFT(i)] = NULL;
        bnode->key_count--;
    }
    /*put the last child node in the new*/
    new_node->child_node[m] = bnode->child_node[i];
    if(new_node->child_node[m])
    {
        new_node->child_node[m]->parent = new_node;
    }
    new_node->parent = parent;
    bnode->child_node[i] = NULL;

    /*TODO:find the insert place in parent*/
    if(parent)
    {
        unsigned int node_index = get_node_index(parent, bnode);
        if(node_index > TREE_ORDER)
        {
            my_print("child node not in the parent!\n");
            return -1;
        }
        insert_key_index = NODE_TO_RIGHT(node_index);
        /*move parent key to insert*/
        my_print("insert index: %d, parent count: %d\n", insert_key_index, parent->key_count);
        if(parent->key_count > 0)
        {
            for(i = parent->key_count - 1; i >= insert_key_index; i--)
            {
                parent->keys[i + 1] = parent->keys[i];
                parent->child_node[KEY_TO_RIGHT(i + 1)] = 
                    parent->child_node[KEY_TO_RIGHT(i)];
                /*unsigned int*/
                if(i == 0)
                    break;
            }
        }
        parent->child_node[KEY_TO_LEFT(insert_key_index)] = bnode;
    }
    else
    {
        /*no parent, the create */
        struct bnode *new_child = NULL;
        new_child = bnode_create();
        if(new_child == NULL)
        {
            my_print("malloc null!\n");
            return -1;
        }
        for(i = 0, m = 0; i < mid_index; i++,m++)
        {
            new_child->keys[m] = bnode->keys[i];
            new_child->child_node[KEY_TO_LEFT(m)] = 
                bnode->child_node[KEY_TO_LEFT(i)];
            if(new_child->child_node[KEY_TO_LEFT(m)])
            {
                new_child->child_node[KEY_TO_LEFT(m)]->parent = new_child;
            }
            new_child->key_count ++;

            bnode->keys[i] = INVALID_KEY;
            bnode->child_node[KEY_TO_LEFT(i)] = NULL;
            bnode->key_count--;
        }
        /*put the last child node in the new*/
        new_child->child_node[m] = bnode->child_node[i];
        if(new_child->child_node[(m)])
        {
            new_child->child_node[(m)]->parent = new_child;
        }
        bnode->child_node[i] = NULL;

        MY_ASSERT(bnode->key_count == 0);

        insert_key_index = 0;
        parent = bnode;
        new_child->parent = parent;
        new_node->parent = parent;
        /*TODO: the btree node now is new parent!!!*/
        parent->child_node[KEY_TO_LEFT(insert_key_index)] = new_child;
    }

    parent->keys[insert_key_index] = mid_data;
    parent->child_node[KEY_TO_RIGHT(insert_key_index)] = new_node;
    parent->key_count++;

    if(parent->key_count >= TREE_ORDER)
    {
        return btree_bnode_split(parent->parent, parent);
    }

    return 0;
} 
int btree_insert(struct bnode *btree, KeyType data)
{
    struct bnode *find_node = NULL;
    unsigned int index;
    if(btree == NULL)        
    {
        my_print("btree is empty!\n");
        return -1;
    }
    /*the first data insert*/
    if(btree->key_count == 0)
    {
        btree->keys[btree->key_count ++] = data;
        return 0;
    }
    my_print("insert data %d\n", data);
    index = 0;
    find_node = btree;
    while(1)
    {
        KeyType exist_data;
        exist_data = find_node->keys[index];
        /*need a soldier which must bigger than data*/
        my_print("exist data %u\n", exist_data);
        if(data <= exist_data)
        {
            unsigned int i;
            /*little key data*/
            if(NULL != find_node->child_node[KEY_TO_LEFT(index)])
            {
                /*init state*/
                find_node = find_node->child_node[KEY_TO_LEFT(index)];
                index = 0;
                printf("index %d left node not null!\n", index);
                continue;
            }
            /*leaf node, insert the data*/
            for(i = find_node->key_count - 1; i >= index; i--)
            {
                find_node->keys[i + 1] = find_node->keys[i];
            }
            find_node->keys[index] = data;
            find_node->key_count ++;
            printf("insert index %d data %u key count: %d\n", index,data,
                    find_node->key_count);
            if(find_node->key_count >= TREE_ORDER)
            {
                btree_bnode_split(find_node->parent, find_node);
                /*split the find_node*/
            }
            break;
        }
        else
        {
            index ++;
        }
    }

    return 0;
}

static int delete_rebalance(struct bnode *change_node)
{
    struct bnode *left_brother = NULL;
    struct bnode *right_brother = NULL;
    struct bnode *parent = change_node->parent;
    unsigned int change_index;
    if(parent == NULL)
    {
        return 0;
    }

    left_brother = get_left_brother(change_node);
    right_brother = get_right_brother(change_node);
    change_index = get_node_index(parent, change_node);

    if(left_brother && left_brother->key_count > MIN_KEY_NUM)
    {
        /*borrow the parent key, put the left right key up to parent*/
        bnode_insert_key(change_node, 0, change_node->key_count, 
                parent->keys[NODE_TO_LEFT(change_index)]);
        /*left brother last node insert to change node first one*/
        bnode_insert_node(change_node, 0, change_node->key_count,
                left_brother->child_node[left_brother->key_count]);
        /*delete left brother last node*/
        bnode_delete_node(left_brother, left_brother->key_count,
                left_brother->key_count);
        parent->keys[NODE_TO_LEFT(change_index)] = 
            left_brother->keys[left_brother->key_count - 1];
        bnode_delete_key(left_brother, left_brother->key_count - 1);
    }
    else if(right_brother && right_brother->key_count > MIN_KEY_NUM)
    {
        bnode_insert_key(change_node, change_node->key_count, 
                change_node->key_count, 
                parent->keys[NODE_TO_RIGHT(change_index)]);
        bnode_insert_node(change_node, change_node->key_count,
                change_node->key_count, right_brother->child_node[0]);
        bnode_delete_node(right_brother, 0, right_brother->key_count);
        parent->keys[NODE_TO_RIGHT(change_index)] = left_brother->keys[0];
        bnode_delete_key(left_brother, 0);
    }
    /*combine with brother*/
    else if(right_brother)
    {
        
    }
    else if(left_brother)
    {
    
    }
}

int btree_delete(struct bnode *btree, KeyType del_data)
{
    struct bnode *del_data_node = NULL;
    unsigned int del_key_index;
    unsigned int del_node_index;
    if(NULL == btree || btree->key_count == 0)
    {
        my_print("del data[%d],but btree is null!\n", del_data);
        return -1;
    }
    /*find the node contains the del data*/
    del_data_node = btree_search(btree, del_data);
    if(NULL == del_data_node)
    {
        my_print("del data[%d],but not found it!\n", del_data);
        return -1;
    }
    del_key_index = get_data_index(del_data_node, del_data);
    /************function can be reenter!!************/
    /*direct del it*/
    /*FIXME: one function according the leaf and no leaf*/
    my_print("find del node %p key index %d\n", del_data_node, del_key_index);
    if(bnode_is_leaf(del_data_node)) 
    {
        /*only one node, delete directly*/
        if((del_data_node->key_count > MIN_KEY_NUM) ||
                (del_data_node->parent == NULL))
        {
            (void)bnode_delete_key(del_data_node, del_key_index);
            return 0;
        }
        else
        {
            unsigned int i,j;
            struct bnode *left_brother = get_left_brother(del_data_node);            
            struct bnode *right_brother = get_right_brother(del_data_node);
            struct bnode *parent = del_data_node->parent;
            del_node_index = get_node_index(del_data_node->parent, del_data_node);
            /*First, find the brother node and key number > MIN_KEY_NUM*/
            if(left_brother && left_brother->key_count > MIN_KEY_NUM)
            {
                /*borrow the parent key, put the left right key up to parent*/
                my_print("borrow left brother count %d keys\n", left_brother->key_count);
                bnode_insert_key(del_data_node, 0, del_key_index, 
                       parent->keys[NODE_TO_LEFT(del_node_index)]);
                parent->keys[NODE_TO_LEFT(del_node_index)] = 
                    left_brother->keys[left_brother->key_count - 1];
                bnode_delete_key(left_brother, left_brother->key_count - 1);
            }
            else if(right_brother && right_brother->key_count > MIN_KEY_NUM)
            {
                /**/
                my_print("borrow right brother count %d keys\n", right_brother->key_count);
                bnode_delete_key(del_data_node, del_key_index);
                bnode_insert_key(del_data_node, del_key_index, del_key_index, 
                        parent->keys[NODE_TO_RIGHT(del_node_index)]);
                parent->keys[NODE_TO_RIGHT(del_node_index)] = right_brother->keys[0];
                bnode_delete_key(right_brother, 0);
            }
            /*has left brother, but the brother has less MIN_KEY_NUM keys, combine it*/
            else if(left_brother)
            {
                /*combine the parent key between brother and del node*/
                bnode_insert_key(left_brother, left_brother->key_count, 
                        left_brother->key_count, 
                        parent->keys[NODE_TO_LEFT(del_node_index)]);
                for(i = 0; i < del_data_node->key_count; i++)
                {
                    if(del_data_node->keys[i] == del_data)
                        continue;
                    left_brother->keys[left_brother->key_count ++] = 
                        del_data_node->keys[i];
                }
                /*free del node*/
                tree_free(del_data_node);
                /*move the parent right node and keys*/
                bnode_delete_node(parent, del_node_index, parent->key_count);
                bnode_delete_key(parent, NODE_TO_LEFT(del_node_index));
                if(parent->key_count < MIN_KEY_NUM)
                {
                    /*TODO*/
                }
            }
            else if(right_brother)
            {
                /*combine the parent key between brother and del node*/
                bnode_insert_key(right_brother, 0, right_brother->key_count,
                        parent->keys[NODE_TO_RIGHT(del_node_index)]);
                for(i = del_data_node->key_count - 1;; i--)
                {
                    if(del_data_node->keys[i] == del_data)
                    {
                        if(i == 0)
                            break;
                        continue;
                    }
                    bnode_insert_key(right_brother, 0, right_brother->key_count,
                            del_data_node->keys[i]);
                    if(i == 0)
                        break;
                }
                /*free del node*/
                tree_free(del_data_node);
                /*move the parent right node and keys*/
                bnode_delete_node(parent, del_node_index, parent->key_count);
                bnode_delete_key(parent, NODE_TO_RIGHT(del_node_index));
                if(parent->key_count < MIN_KEY_NUM)
                {
                    /*TODO*/
                }
                    
            }
        }
    }
    /*not leaf*/
    else
    {
        /*
         * if(bnode_is_leaf(del_data_node))
         * {
         *     return leaf_node_delete();
         * }
         * */
        struct bnode *left_child = del_data_node->child_node[KEY_TO_LEFT(del_key_index)];        
        struct bnode *right_child = del_data_node->child_node[KEY_TO_RIGHT(del_key_index)];        
        int i,j;
        if(left_child->key_count > MIN_KEY_NUM ||
                right_child->key_count > MIN_KEY_NUM)
        {
            if(left_child->key_count > right_child->key_count)
            {
                /*FIXME: call to delete node and index*/
            }
            else
            {
                /*FIXME: call to delete node and index*/
            }
        }
    }

    return 0;
}

struct bnode *btree_create(KeyType *data_array, unsigned int length)
{
    struct bnode *btree = NULL;

    return btree;
}

int main()
{
    struct bnode *btree = bnode_create();    
    if(btree == NULL)
    {
        my_print("btree mem alloc fail!\n");
        return 0;
    }
    btree_insert(btree, 1);
    btree_insert(btree, 10);
    btree_insert(btree, 30);
    btree_insert(btree, 50);
    btree_insert(btree, 70);
    btree_insert(btree, 7);
    btree_insert(btree, 9);
    btree_insert(btree, 5);
    btree_insert(btree, 11);
    btree_insert(btree, 12);
    btree_insert(btree, 13);
    btree_insert(btree, 14);
    btree_insert(btree, 15);
    btree_insert(btree, 16);
    btree_insert(btree, 29);
    btree_insert(btree, 17);
    btree_insert(btree, 18);
    btree_insert(btree, 21);
    btree_insert(btree, 24);
    btree_insert(btree, 25);
    /*
    btree_insert(btree, 55);
    btree_insert(btree, 58);
    */
    btree_dump(btree, 0);
    btree_delete(btree, 29);
    btree_dump(btree, 0);
    btree_delete(btree, 16);
    btree_dump(btree, 0);
    btree_delete(btree, 17);
    btree_dump(btree, 0);
    btree_insert(btree, 26);
    btree_insert(btree, 27);
    btree_insert(btree, 28);
    btree_dump(btree, 0);
    btree_delete(btree, 18);
    btree_dump(btree, 0);
    return 0;
}
