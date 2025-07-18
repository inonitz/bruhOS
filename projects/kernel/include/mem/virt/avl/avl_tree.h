/*
 * avl_tree.h - intrusive, nonrecursive AVL tree data structure (self-balancing
 *		binary search tree), header file
 *
 * Written in 2014-2016 by Eric Biggers <ebiggers3@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide via the Creative Commons Zero 1.0 Universal Public Domain
 * Dedication (the "CC0").
 *
 * This software is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the CC0 for more details.
 *
 * You should have received a copy of the CC0 along with this software; if not
 * see <http://creativecommons.org/publicdomain/zero/1.0/>.
 * NOTE (Added By User, not Author): https://github.com/ebiggers/avl_tree/blob/master/avl_tree.h
 */

#pragma once
#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_

#include <stddef.h>
#include <stdint.h>
#include <std/enable_bool.h>
#include <std/bool.h>


#ifdef __GNUC__
#  define AVL_INLINE inline __attribute__((always_inline))
#else
#  define AVL_INLINE inline
#  warning "AVL tree functions may not be inlined as intended"
#endif

/* Node in an AVL tree.  Embed this in some other data structure.  */
struct avl_tree_node {

	/* Pointer to left child or NULL  */
	struct avl_tree_node *left;

	/* Pointer to right child or NULL  */
	struct avl_tree_node *right;

	/* Pointer to parent combined with the balance factor.  This saves 4 or
	 * 8 bytes of memory depending on the CPU architecture.
	 *
	 * Low 2 bits:  One greater than the balance factor of this subtree,
	 * which is equal to height(right) - height(left).  The mapping is:
	 *
	 * 00 => -1
	 * 01 =>  0
	 * 10 => +1
	 * 11 => undefined
	 *
	 * The rest of the bits are the pointer to the parent node.  It must be
	 * 4-byte aligned, and it will be NULL if this is the root node and
	 * therefore has no parent.  */
	uintptr_t parent_balance;
};

/* Cast an AVL tree node to the containing data structure.  */
#define avl_tree_entry(entry, type, member) \
	((type*) ((char *)(entry) - offsetof(type, member)))

/* Returns a pointer to the parent of the specified AVL tree node, or NULL if it
 * is already the root of the tree.  */
static AVL_INLINE struct avl_tree_node *
avl_get_parent(const struct avl_tree_node *node)
{
	return (struct avl_tree_node *)(node->parent_balance & ~3);
}

/* Marks the specified AVL tree node as unlinked from any tree.  */
static AVL_INLINE void
avl_tree_node_set_unlinked(struct avl_tree_node *node)
{
	node->parent_balance = (uintptr_t)node;
}

/* Returns true iff the specified AVL tree node has been marked with
 * avl_tree_node_set_unlinked() and has not subsequently been inserted into a
 * tree.  */
static AVL_INLINE bool_t
avl_tree_node_is_unlinked(const struct avl_tree_node *node)
{
	return node->parent_balance == (uintptr_t)node;
}

/* (Internal use only)  */
extern void
avl_tree_rebalance_after_insert(struct avl_tree_node **root_ptr,
				struct avl_tree_node *inserted);

/*
 * Looks up an item in the specified AVL tree.
 *
 * @root
 *	Pointer to the root of the AVL tree.  (This can be NULL --- that just
 *	means the tree is empty.)
 *
 * @cmp_ctx
 *	First argument to pass to the comparison callback.  This generally
 *	should be a pointer to an object equal to the one being searched for.
 *
 * @cmp
 *	Comparison callback.  Must return < 0, 0, or > 0 if the first argument
 *	is less than, equal to, or greater than the second argument,
 *	respectively.  The first argument will be @cmp_ctx and the second
 *	argument will be a pointer to the AVL tree node of an item in the tree.
 *
 * Returns a pointer to the AVL tree node of the resulting item, or NULL if the
 * item was not found.
 *
 * Example:
 *
 * struct int_wrapper {
 *	int data;
 *	struct avl_tree_node index_node;
 * };
 *
 * static int _avl_cmp_int_to_node(const void *intptr,
 *				   const struct avl_tree_node *nodeptr)
 * {
 *	int n1 = *(const int *)intptr;
 *	int n2 = avl_tree_entry(nodeptr, struct int_wrapper, index_node)->data;
 *	if (n1 < n2)
 *		return -1;
 *	else if (n1 > n2)
 *		return 1;
 *	else
 *		return 0;
 * }
 *
 * bool_t contains_int(struct avl_tree_node *root, int n)
 * {
 *	struct avl_tree_node *result;
 *
 *	result = avl_tree_lookup(root, &n, _avl_cmp_int_to_node);
 *	return result ? true : false;
 * }
 */
static AVL_INLINE struct avl_tree_node *
avl_tree_lookup(const struct avl_tree_node *root,
		const void *cmp_ctx,
		int (*cmp)(const void *, const struct avl_tree_node *))
{
	const struct avl_tree_node *cur = root;

	while (cur) {
		int res = (*cmp)(cmp_ctx, cur);
		if (res < 0)
			cur = cur->left;
		else if (res > 0)
			cur = cur->right;
		else
			break;
	}
	return (struct avl_tree_node*)cur;
}

/* Same as avl_tree_lookup(), but uses a more specific type for the comparison
 * function.  Specifically, with this function the item being searched for is
 * expected to be in the same format as those already in the tree, with an
 * embedded 'struct avl_tree_node'.  */
static AVL_INLINE struct avl_tree_node *
avl_tree_lookup_node(const struct avl_tree_node *root,
		     const struct avl_tree_node *node,
		     int (*cmp)(const struct avl_tree_node *,
				const struct avl_tree_node *))
{
	const struct avl_tree_node *cur = root;

	while (cur) {
		int res = (*cmp)(node, cur);
		if (res < 0)
			cur = cur->left;
		else if (res > 0)
			cur = cur->right;
		else
			break;
	}
	return (struct avl_tree_node*)cur;
}

/*
 * Inserts an item into the specified AVL tree.
 *
 * @root_ptr
 *	Location of the AVL tree's root pointer.  Indirection is needed because
 *	the root node may change as a result of rotations caused by the
 *	insertion.  Initialize *root_ptr to NULL for an empty tree.
 *
 * @item
 *	Pointer to the `struct avl_tree_node' embedded in the item to insert.
 *	No members in it need be pre-initialized, although members in the
 *	containing structure should be pre-initialized so that @cmp can use them
 *	in comparisons.
 *
 * @cmp
 *	Comparison callback.  Must return < 0, 0, or > 0 if the first argument
 *	is less than, equal to, or greater than the second argument,
 *	respectively.  The first argument will be @item and the second
 *	argument will be a pointer to an AVL tree node embedded in some
 *	previously-inserted item to which @item is being compared.
 *
 * If no item in the tree is comparatively equal (via @cmp) to @item, inserts
 * @item and returns NULL.  Otherwise does nothing and returns a pointer to the
 * AVL tree node embedded in the previously-inserted item which compared equal
 * to @item.
 *
 * Example:
 *
 * struct int_wrapper {
 *	int data;
 *	struct avl_tree_node index_node;
 * };
 *
 * #define GET_DATA(i) avl_tree_entry((i), struct int_wrapper, index_node)->data
 *
 * static int _avl_cmp_ints(const struct avl_tree_node *node1,
 *			    const struct avl_tree_node *node2)
 * {
 *	int n1 = GET_DATA(node1);
 *	int n2 = GET_DATA(node2);
 *	if (n1 < n2)
 *		return -1;
 *	else if (n1 > n2)
 *		return 1;
 *	else
 *		return 0;
 * }
 *
 * bool_t insert_int(struct avl_tree_node **root_ptr, int data)
 * {
 *	struct int_wrapper *i = malloc(sizeof(struct int_wrapper));
 *	i->data = data;
 *	if (avl_tree_insert(root_ptr, &i->index_node, _avl_cmp_ints)) {
 *		// Duplicate.
 *		free(i);
 *		return false;
 *	}
 *	return true;
 * }
 */
static AVL_INLINE struct avl_tree_node *
avl_tree_insert(struct avl_tree_node **root_ptr,
		struct avl_tree_node *item,
		int (*cmp)(const struct avl_tree_node *,
			   const struct avl_tree_node *))
{
	struct avl_tree_node **cur_ptr = root_ptr, *cur = NULL;
	int res;

	while (*cur_ptr) {
		cur = *cur_ptr;
		res = (*cmp)(item, cur);
		if (res < 0)
			cur_ptr = &cur->left;
		else if (res > 0)
			cur_ptr = &cur->right;
		else
			return cur;
	}
	*cur_ptr = item;
	item->parent_balance = (uintptr_t)cur | 1;
	avl_tree_rebalance_after_insert(root_ptr, item);
	return NULL;
}

/* Removes an item from the specified AVL tree.
 * See implementation for details.  */
extern void
avl_tree_remove(struct avl_tree_node **root_ptr, struct avl_tree_node *node);

/* Nonrecursive AVL tree traversal functions  */

extern struct avl_tree_node *
avl_tree_first_in_order(const struct avl_tree_node *root);

extern struct avl_tree_node *
avl_tree_last_in_order(const struct avl_tree_node *root);

extern struct avl_tree_node *
avl_tree_next_in_order(const struct avl_tree_node *node);

extern struct avl_tree_node *
avl_tree_prev_in_order(const struct avl_tree_node *node);

extern struct avl_tree_node *
avl_tree_first_in_postorder(const struct avl_tree_node *root);

extern struct avl_tree_node *
avl_tree_next_in_postorder(const struct avl_tree_node *prev,
			   const struct avl_tree_node *prev_parent);

/*
 * Iterate through the nodes in an AVL tree in sorted order.
 * You may not modify the tree during the iteration.
 *
 * @child_struct
 *	Variable that will receive a pointer to each struct inserted into the
 *	tree.
 * @root
 *	Root of the AVL tree.
 * @struct_name
 *	Type of *child_struct.
 * @struct_member
 *	Member of @struct_name type that is the AVL tree node.
 *
 * Example:
 *
 * struct int_wrapper {
 *	int data;
 *	struct avl_tree_node index_node;
 * };
 *
 * void print_ints(struct avl_tree_node *root)
 * {
 *	struct int_wrapper *i;
 *
 *	avl_tree_for_each_in_order(i, root, struct int_wrapper, index_node)
 *		printf("%d\n", i->data);
 * }
 */
#define avl_tree_for_each_in_order(child_struct, root,			\
				   struct_name, struct_member)		\
	for (struct avl_tree_node *_cur =				\
		avl_tree_first_in_order(root);				\
	     _cur && ((child_struct) =					\
		      avl_tree_entry(_cur, struct_name,			\
				     struct_member), 1);		\
	     _cur = avl_tree_next_in_order(_cur))

/*
 * Like avl_tree_for_each_in_order(), but uses the reverse order.
 */
#define avl_tree_for_each_in_reverse_order(child_struct, root,		\
					   struct_name, struct_member)	\
	for (struct avl_tree_node *_cur =				\
		avl_tree_last_in_order(root);				\
	     _cur && ((child_struct) =					\
		      avl_tree_entry(_cur, struct_name,			\
				     struct_member), 1);		\
	     _cur = avl_tree_prev_in_order(_cur))

/*
 * Like avl_tree_for_each_in_order(), but iterates through the nodes in
 * postorder, so the current node may be deleted or freed.
 */
#define avl_tree_for_each_in_postorder(child_struct, root,		\
				       struct_name, struct_member)	\
	for (struct avl_tree_node *_cur =				\
		avl_tree_first_in_postorder(root), *_parent;		\
	     _cur && ((child_struct) =					\
		      avl_tree_entry(_cur, struct_name,			\
				     struct_member), 1)			\
	          && (_parent = avl_get_parent(_cur), 1);		\
	     _cur = avl_tree_next_in_postorder(_cur, _parent))

#endif /* _AVL_TREE_H_ */
