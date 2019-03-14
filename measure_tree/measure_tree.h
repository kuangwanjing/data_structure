#include "endpoint_tree.h"

/*defintion and implementation of measure tree*/
typedef struct m_tree_t {
    int key;                // key of the node
    int left_endpoint;      // the left endpoint of the node interval
    int right_endpoint;     // the right endpoint of the node interval
    int measure;            // the measure of associated intervals
    int leftmin;            // minimum left endpoint among all associated intervals 
    int rightmax;           // maximum right endpoint among all associated intervals
    struct m_tree_t *left;  // left subtree
    struct m_tree_t *right; // right subtree
    int height;             // balancing factor
} m_tree_t;

typedef struct m_object_t {
    tree_node_t *left_endpoints;
    tree_node_t *right_endpoints;
} m_object_t;

m_tree_t *create_m_tree();
void insert_interval(m_tree_t *, int, int);
void delete_interval(m_tree_t *, int, int);
int query_length(m_tree_t *);
