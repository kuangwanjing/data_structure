#include "measure_tree.h"

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
