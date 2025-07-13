#ifndef __AVL_TREE_FUNC_WRAPPER__
#define __AVL_TREE_FUNC_WRAPPER__
#include "avl_tree.h"
#include <mem/virt/flags.h>



typedef struct __virtual_memory_region
{
    uint64_t       address;
    uint32_t       extent;    // == pages
    uint32_t       wrk_set;   // == actually currently allocated pages
    paging_flags_t flags;
} vm_region_t;


typedef struct __avl_tree_node_wrapper
{
    vm_region_t          record;
    struct avl_tree_node node;
} vas_node_t;




static int32_t __avl_cmp_vm_address(
    const void*                 addressptr,
    const struct avl_tree_node* nodeptr
) {
    const uint64_t addr  = *(const uint64_t*)addressptr;
    vm_region_t* eval  = &avl_tree_entry(nodeptr, vas_node_t, node)->record;
    int32_t      expr0 = (addr < eval->address) * -1;
    int32_t      expr1 = (addr > eval->address)     ;
    return expr0 + expr1;
}


static int32_t __avl_cmp_vm_sort_region(
    const struct avl_tree_node *node1,
    const struct avl_tree_node *node2
) {
    // if      left_smaller: go left. 
    // else if right_bigger: go right
    // else    fuck off
    vm_region_t* n1           = &avl_tree_entry(node1, vas_node_t, node)->record;
    vm_region_t* n2           = &avl_tree_entry(node2, vas_node_t, node)->record;
    int32_t      left_smaller = (n1->address + n1->extent < n2->address) * -1;
    int32_t      right_bigger = (n2->address + n2->extent < n1->address)     ;

    return left_smaller + right_bigger; 
}


static vas_node_t* lookup_by_address(vas_node_t*  root_node, uint64_t    address  ) {
    return (vas_node_t*)avl_tree_lookup(&root_node->node, &address, __avl_cmp_vm_address);
}
static vas_node_t* insert_node      (vas_node_t** root_node, vas_node_t* to_insert) {
    // if nullptr: inserted successfully. else, the returned node address should be freed.
    return (vas_node_t*)avl_tree_insert((struct avl_tree_node**)root_node, 
        &to_insert->node, 
        __avl_cmp_vm_sort_region
    );
}
static void        remove_node      (vas_node_t** root_node, vas_node_t* to_remove) {
    avl_tree_remove((struct avl_tree_node**)root_node, &to_remove->node);    
}


#endif
