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
int tree_max(tree_node_t *);
int tree_min(tree_node_t *);
