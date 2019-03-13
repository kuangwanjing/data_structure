#include <stdio.h>

/*definition and implementation of endpoint tree*/
#define BLOCKSIZE 256
#define INF 1000000
#define NINF -1000000

typedef struct tr_n_t {
    int key;
    struct tr_n_t * left;
    struct tr_n_t * right;
    int height;
} tree_node_t;

tree_node_t * create_tree(void); 
int insert(tree_node_t *, int);
int _delete(tree_node_t *, int , tree_node_t *); 
void print(tree_node_t *);
void rebalance(tree_node_t *);
void left_rotation(tree_node_t *);
void right_rotation(tree_node_t *);

tree_node_t * currentblock = NULL;
int size_left;
tree_node_t * free_list = NULL;
int nodes_taken = 0;
int nodes_returned = 0;

tree_node_t * get_node() {
    tree_node_t * tmp;
    nodes_taken += 1;
    if (free_list != NULL) {
        tmp = free_list;
        free_list = free_list->right;
    } else {
        if (currentblock == NULL || size_left == 0) {
            currentblock =
                (tree_node_t * ) malloc(BLOCKSIZE * sizeof(tree_node_t));
            size_left = BLOCKSIZE;
        }
        tmp = currentblock++;
        size_left -= 1;
    }
    return (tmp);
}

void return_node(tree_node_t * node) {
    node->right = free_list;
    free_list = node;
    nodes_returned += 1;
}

tree_node_t *create_tree(void) {
    tree_node_t * tmp_node;
    tmp_node = get_node();
    tmp_node->left = NULL;
    return (tmp_node);
}

int insert(tree_node_t * tree, int new_key) {
    int* val;
    // tree now is empty
    if (tree->left == NULL) {
        val = (int *) malloc(sizeof(int));
        *val = 1;
        tree->left = (tree_node_t *)(val); // one ocurrence of this key
        tree->key = new_key;
        tree->right = NULL;
        tree->height = 1;
        return (0);
    }
    // reaching the leaf node
    if (tree->right == NULL) {
        // check whether the key has been inserted into the tree.
        if (tree->key == new_key) {
            val = (int *) tree->left;
            *val = *val + 1;
            return (0);
        }
        val = (int *) malloc(sizeof(int));
        *val = 1;
        tree_node_t *old_leaf, *new_leaf;
        old_leaf = get_node();
        old_leaf->left = tree->left;
        old_leaf->key = tree->key;
        old_leaf->right = NULL;
        old_leaf->height = 1;
        new_leaf = get_node();
        new_leaf->left = (tree_node_t *)(val);
        new_leaf->key = new_key;
        new_leaf->right = NULL;
        new_leaf->height = 1;
        if( old_leaf->key < new_key ) {   
            tree->left = old_leaf;
            tree->right = new_leaf;
            tree->key = new_key;
        } else {   
            tree->left = new_leaf;
            tree->right = old_leaf;
        } 
        tree->height = 2;
        return (0);
    }
    // the index of the key lies in the left subtree for the key is no more than the left subtree's sum
    if (new_key < tree->key) {
        insert(tree->left, new_key);
    } else {
    // otherwise the key lies in the right subtree, deduct the searching key with the sum of the left subtree.
        insert(tree->right, new_key);
    }
    // remain balancing
    rebalance(tree);
    return (0);
}

int _delete(tree_node_t *tree, int delete_key, tree_node_t *parent) {  
    tree_node_t *other_node;
    int *val;
    if (tree->left == NULL) {
        return -1;
    }
    if (tree->right == NULL) {
        if (tree->key != delete_key) {
            return -1;
        }
        val = (int *)tree->left;
        if (*val > 1) {
            *val = *val - 1;
            return 0;
        }
        if (parent != NULL) {
            other_node = parent->left == tree ? parent->right : parent->left;
            parent->key = other_node->key;
            parent->left = other_node->left;
            parent->right = other_node->right;
            parent->height = other_node->height;
        }
        return_node(tree);
        return_node(other_node);
        return 0;
    }
    // the index of the key lies in the left subtree for the key is no more than the left subtree's sum
    if (delete_key < tree->key) {
        _delete(tree->left, delete_key, tree);
    } else {
    // otherwise the key lies in the right subtree, deduct the searching key with the sum of the left subtree.
        _delete(tree->right, delete_key, tree);
    }
    // update the tree key by adding the sums together
    if (tree->right != NULL) {
        // remain balancing
        rebalance(tree);
    }
    return 0;
}

int tree_max(tree_node_t *tree) {
    if (tree->left == NULL) {
        return NINF;
    }
    tree_node_t *tmp_node;
    tmp_node = tree;
    while(tmp_node->right != NULL) {
        if(tmp_node->right != NULL) {
            tmp_node = tmp_node->right;
        } else {
            tmp_node = tmp_node->left;
        }
    }
    return tmp_node->key;
}

int tree_min(tree_node_t *tree) {
    if (tree->left == NULL) {
        return INF;
    }
    tree_node_t *tmp_node;
    tmp_node = tree;
    while(tmp_node->right != NULL) {
        if(tmp_node->left != NULL) {
            tmp_node = tmp_node->left;
        } else {
            tmp_node = tmp_node->right;
        }
    }
    return tmp_node->key;
}

void rebalance(tree_node_t *tree) {
    int tmp_height; 
    if (tree->left->height - tree->right->height == 2) {
        if (tree->left->left->height - tree->right->height == 1) {
            right_rotation(tree);
            tree->right->height = tree->right->left->height + 1;
            tree->height = tree->right->height + 1;
        } else {
            left_rotation(tree->left);
            right_rotation(tree);
            tmp_height = tree->left->left->height;
            tree->left->height = tmp_height + 1;
            tree->right->height = tmp_height + 1;
            tree->height = tmp_height + 2;
        } 
    } else if (tree->left->height - tree->right->height == -2) {
        if (tree->right->right->height - tree->left->height == 1) {
            left_rotation(tree);
            tree->left->height = tree->left->right->height + 1;
            tree->height = tree->left->height + 1;
        } else {
            right_rotation(tree->right);
            left_rotation(tree);
            tmp_height = tree->right->right->height;
            tree->left->height = tmp_height + 1;
            tree->right->height = tmp_height + 1;
            tree->height = tmp_height + 2;
        }
    } else {
        if (tree->left->height > tree->right->height)
            tree->height = tree->left->height + 1;
        else
            tree->height = tree->right->height + 1;
    }
}

void left_rotation(tree_node_t * n) {
    tree_node_t *tmp_node;
    int tmp_key;
    tmp_node = n->left;
    tmp_key = n->key;
    n->key = n->right->key;
    n->left = n->right;
    n->right = n->left->right;
    n->left->right = n->left->left;
    n->left->left = tmp_node;
    n->left->key = tmp_key;
}

void right_rotation(tree_node_t *n) {  
    tree_node_t *tmp_node;
    int tmp_key;
    tmp_node = n->right;
    tmp_key = n->key;
    n->key = n->left->key;
    n->right = n->left;
    n->left = n->right->left;
    n->right->left = n->right->right;
    n->right->right  = tmp_node;
    n->right->key = tmp_key;
}

void print(tree_node_t *tree) {
    if (tree->left == NULL) {
        printf("NULL\n");
        return;
    }
    if (tree->right == NULL) {
        printf("reaching leaf %d with value: %d\n", tree->key, *(int *)tree->left);
        return;
    }
    print(tree->left);
    printf("internal node with key and height: %d, %d\n", tree->key, tree->height);
    print(tree->right);
}
/*end of definition and implementation of basic tree*/

/*defintion and implementation of measure tree*/
/*
typedef struct m_tree_t {
    tree_node_t *tree; 
} m_tree_t;

typedef struct m_t {
    int leftmin;
    int rightmax;
    int measure;
    int endpoint1;
    int endpoint2;
} m_t;

#define INF 1000000
#define NINF -1000000

typedef int object_t;
m_tree_t *create_m_tree();
void insert_interval(m_tree_t *, int, int);
void delete_interval(m_tree_t *, int, int);
int query_length(m_tree_t *);

m_tree_t *create_m_tree() {
    m_tree_t *mtree = (m_tree_t *)malloc(sizeof(m_tree_t));
    mtree->tree = create_tree();
    m_t *t = (m_t *)malloc(sizeof(m_t));
    t->leftmin = INF;
    t->rightmax = NINF;
    t->measure = 0;
    mtree->tree->other = (void*) t;
    return mtree;
}
*/
/*end of defintion and implementation of measure tree*/

int main() {
    //m_tree_t *measure_tree = create_m_tree(); 
    //printf("%p\n", measure_tree->tree->other);
    tree_node_t *t = create_tree();
    insert(t, 2);
    insert(t, 3);
    insert(t, 4);
    insert(t, 4);
    insert(t, 4);
    insert(t, 4);
    //print(t);
    _delete(t, 4, NULL);
    //print(t);
    insert(t, 1);
    printf("the max is %d\n", tree_max(t));
    printf("the min is %d\n", tree_min(t));
}
