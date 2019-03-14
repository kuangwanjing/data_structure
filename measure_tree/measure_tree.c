#include "measure_tree.h"

m_tree_t *m_currentblock = NULL;
int m_size_left;
m_tree_t *m_free_list = NULL;
int m_nodes_taken = 0;
int m_nodes_returned = 0;

void m_rebalance(m_tree_t *);
void m_right_rotation(m_tree_t *);
void m_left_rotation(m_tree_t *);
int insert_endpoints(m_tree_t *, int);
int update_interval(m_tree_t *, int, int, int);
void measure(m_tree_t *);

m_tree_t * m_get_node() {
    m_tree_t * tmp;
    m_nodes_taken += 1;
    if (m_free_list != NULL) {
        tmp = m_free_list;
        m_free_list = m_free_list->right;
    } else {
        if (m_currentblock == NULL || m_size_left == 0) {
            m_currentblock = (m_tree_t *) malloc(BLOCKSIZE * sizeof(m_tree_t));
            m_size_left = BLOCKSIZE;
        }
        tmp = m_currentblock++;
        m_size_left -= 1;
    }
    return (tmp);
}

void m_return_node(m_tree_t* node) {
    node->right = m_free_list;
    m_free_list = node;
    m_nodes_returned += 1;
}

m_tree_t *create_m_tree() {
    m_tree_t *tree = m_get_node();
    tree->left = NULL; 
    tree->right = NULL;
    tree->left_endpoint = NINF;
    tree->right_endpoint = INF;
    tree->leftmin = INF;
    tree->rightmax = NINF;
    tree->measure = 0;
    tree->height = 0;
    return tree;
} 

m_object_t *create_endpoints() {
    tree_node_t *l_ep_tree = create_tree(); // create an endpoint tree for left endpoints;
    tree_node_t *r_ep_tree = create_tree(); // create an endpoint tree for right endpoints;
    m_object_t *object = malloc(sizeof(m_object_t));
    object->left_endpoints = l_ep_tree;
    object->right_endpoints = r_ep_tree;
    return object;
}

void release_endpoints(m_object_t * object) {
    if (object != NULL) {
        return_node(object->left_endpoints);
        return_node(object->right_endpoints);
        free(object);
    }
}

void insert_interval(m_tree_t *tree, int a, int b) {
    if (a >= b) {
        return;
    }
    insert_endpoints(tree, a);
    insert_endpoints(tree, b);
    update_interval(tree, a, b, 0);
    update_interval(tree, b, a, 0);
}

void delete_interval(m_tree_t *tree, int a, int b) {
    update_interval(tree, a, b, 1);
    update_interval(tree, b, a, 1);
}

int query_length(m_tree_t *tree) {
    return tree->measure;
}

int insert_endpoints(m_tree_t *tree, int endpoint) {
    // tree now is empty
    if (tree->left == NULL) {
        tree->left = (m_tree_t *)(create_endpoints());
        tree->key = endpoint;
        tree->right = NULL;
        tree->height = 1;
        return (0);
    }
    // reaching the leaf node
    if (tree->right == NULL) {
        // check whether the key has been inserted into the tree.
        if (tree->key == endpoint) {
            return (0);
        }
        m_tree_t *old_leaf, *new_leaf;
        old_leaf = m_get_node();
        old_leaf->key = tree->key;
        old_leaf->left = tree->left;
        old_leaf->right = NULL;
        old_leaf->height = 1;
        new_leaf = m_get_node();
        new_leaf->key = endpoint;
        new_leaf->left = (m_tree_t *)(create_endpoints());
        new_leaf->right = NULL;
        new_leaf->height = 1;
        tree->height = 2;
        if(old_leaf->key < endpoint) {   
            old_leaf->left_endpoint = tree->left_endpoint;
            old_leaf->right_endpoint = endpoint;
            measure(old_leaf);
            new_leaf->left_endpoint = endpoint;
            new_leaf->right_endpoint = tree->right_endpoint;
            tree->key = endpoint;
            tree->left = old_leaf;
            tree->right = new_leaf;
        } else {   
            old_leaf->left_endpoint = tree->key;
            old_leaf->right_endpoint = tree->right_endpoint;
            measure(old_leaf);
            new_leaf->left_endpoint = tree->left_endpoint;
            new_leaf->right_endpoint = tree->key;
            tree->left = new_leaf;
            tree->right = old_leaf;
        } 
        return (0);
    }
    if (endpoint < tree->key) {
        insert_endpoints(tree->left, endpoint);
    } else {
        insert_endpoints(tree->right, endpoint);
    }
    // remain balancing
    m_rebalance(tree);
    return (0);
}

void m_rebalance(m_tree_t *tree) {
    int tmp_height; 
    if (tree->left->height - tree->right->height == 2) {
        if (tree->left->left->height - tree->right->height == 1) {
            m_right_rotation(tree);
            tree->right->height = tree->right->left->height + 1;
            tree->height = tree->right->height + 1;
        } else {
            m_left_rotation(tree->left);
            m_right_rotation(tree);
            tmp_height = tree->left->left->height;
            tree->left->height = tmp_height + 1;
            tree->right->height = tmp_height + 1;
            tree->height = tmp_height + 2;
        } 
    } else if (tree->left->height - tree->right->height == -2) {
        if (tree->right->right->height - tree->left->height == 1) {
            m_left_rotation(tree);
            tree->left->height = tree->left->right->height + 1;
            tree->height = tree->left->height + 1;
        } else {
            m_right_rotation(tree->right);
            m_left_rotation(tree);
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

void m_left_rotation(m_tree_t* n) {
    m_tree_t *tmp_node;
    int tmp_key;
    tmp_node = n->left;
    tmp_key = n->key;
    n->key = n->right->key;
    n->left = n->right;
    n->right = n->left->right;
    n->left->right = n->left->left;
    n->left->left = tmp_node;
    n->left->key = tmp_key;
    n->left->left_endpoint = n->left_endpoint;
    n->left->right_endpoint = n->key;
    measure(n->left);
    measure(n->right);
    measure(n);
}

void m_right_rotation(m_tree_t *n) {  
    m_tree_t *tmp_node;
    int tmp_key;
    tmp_node = n->right;
    tmp_key = n->key;
    n->key = n->left->key;
    n->right = n->left;
    n->left = n->right->left;
    n->right->left = n->right->right;
    n->right->right  = tmp_node;
    n->right->key = tmp_key;
    n->right->left_endpoint = n->key;
    n->right->right_endpoint = n->right_endpoint;
    measure(n->left);
    measure(n->right);
    measure(n);
}

int update_interval(m_tree_t *tree, int cur, int other, int op) {
    // tree now is empty, the state of tree is incorrect
    if (tree->left == NULL) {
        return (-1);
    }
    // reaching the leaf node
    if (tree->right == NULL) {
        // check whether the key has been inserted into the tree.
        if (tree->key != cur) {
            return (-1);
        }
        m_object_t *object = (m_object_t *)tree->left;
        if (op == 0) { // insert an interval
            if (cur < other) {
                insert(object->left_endpoints, cur);
                insert(object->right_endpoints, other);
            } else {
                insert(object->left_endpoints, other);
                insert(object->right_endpoints, cur);
            }
        } else {
            if (cur < other) {
                _delete(object->left_endpoints, cur, NULL);
                _delete(object->right_endpoints, other, NULL);
            } else {
                _delete(object->left_endpoints, other, NULL);
                _delete(object->right_endpoints, cur, NULL);
            }
        }
        measure(tree);
        return (0);
    }
    if (cur < tree->key) {
        update_interval(tree->left, cur, other, op);
    } else {
        update_interval(tree->right, cur, other, op);
    }
    measure(tree);
    return (0);
}

void print_interval_tree(m_tree_t *tree) {
    if (tree == NULL || tree->left == NULL) {
        printf("NULL\n");
        return;
    }
    if (tree->right == NULL) {
        printf("reaching leaf %d with min, max, measure and bound is: %d, %d, %d, (%d, %d)\n", 
                tree->key, tree->leftmin, tree->rightmax, tree->measure, tree->left_endpoint, tree->right_endpoint);
        return;
    }
    print_interval_tree(tree->left);
    printf("internal node %d with leftmin, rightmax, measure and bound is: %d, %d, %d, (%d, %d)\n", 
            tree->key, tree->leftmin, tree->rightmax, tree->measure, tree->left_endpoint, tree->right_endpoint);
    print_interval_tree(tree->right);
}

void measure(m_tree_t *tree) {
    if (tree == NULL) {
        return;
    }
    // 5 cases to calculate measure for a leaf
    if (tree->right == NULL) {
        m_object_t *object = (m_object_t *)tree->left;
        tree->leftmin = tree_min(object->left_endpoints);
        tree->rightmax = tree_max(object->right_endpoints);
        if (tree->leftmin == INF || tree->rightmax == NINF) {
            // no endpoint left in the endpoint trees
            tree->leftmin = tree->left_endpoint;
            tree->rightmax = tree->right_endpoint;
            tree->measure = 0;
        } else if (tree->leftmin <= tree->left_endpoint && tree->rightmax >= tree->right_endpoint) {
            tree->measure = tree->right_endpoint - tree->left_endpoint;
        } else if (tree->left_endpoint <= tree->leftmin && tree->right_endpoint >= tree->rightmax) {
            tree->measure = tree->rightmax - tree->leftmin;
        } else if (tree->leftmin <= tree->left_endpoint && tree->right_endpoint >= tree->rightmax) {
            tree->measure = tree->rightmax - tree->left_endpoint;
        } else {
            tree->measure = tree->right_endpoint - tree->leftmin;
        }
    // 4 cases to calculate measure for an interior node 
    } else {
        tree->leftmin = tree->left->leftmin < tree->right->leftmin ? tree->left->leftmin : tree->right->leftmin;
        tree->rightmax = tree->left->rightmax > tree->right->rightmax ? tree->left->rightmax : tree->right->rightmax;
        if (tree->right->leftmin < tree->left_endpoint && tree->left->rightmax >= tree->right_endpoint) {
            tree->measure = tree->right_endpoint - tree->left_endpoint;
        } else if (tree->right->leftmin >= tree->left_endpoint && tree->left->rightmax >= tree->right_endpoint) {
            tree->measure = (tree->right_endpoint - tree->key) + tree->left->measure;
        } else if (tree->right->leftmin < tree->left_endpoint && tree->left->rightmax < tree->right_endpoint) {
            tree->measure = tree->right->measure + (tree->key - tree->left_endpoint);
        } else {
            tree->measure = tree->left->measure + tree->right->measure;
        }
    }
}
