#ifndef LHEAP_H_
#define LHEAP_H_ 1

#include "kernel.h"

#include <stddef.h>

/** Leftist heap tree node */
struct lh_node {
    struct lh_node *parent;
    struct lh_node *left;
    struct lh_node *right;
    int npl;
};

/** Leftist heap */
struct lh_heap {
    struct lh_node *node;
};

#define lh_heap(name) struct lh_node name = { NULL }
#define lh_heap_init(root) do { \
        (root)->node = NULL; \
    } while (0)

#define lh_heap_empty(root) ((root)->node == NULL)

#define lh_node_empty(node) (node) == NULL
#define lh_node_init(node)  do { \
        (node)->parent = NULL; \
        (node)->left = (node)->right = NULL; \
        (node)->npl = 0; \
    } while (0)

/**
 * Get the struct for this entry.
 *
 * @param ptr struct list head pointer
 * @param type type of the struct this is embedded in
 * @param member name of the list structure within the struct
 */
#define lh_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * Merge two heaps to a single heap.
 *
 * @param heap1 first heap
 * @param heap2 second heap
 * @param type type of the struct this is embedded in
 * @param member name of the list structure within the struct
 * @param key name of the key item within the struct
 * @param cmp comparison function
 * @return merged heap
 */
#define lh_merge(heap1, heap2, type, member, key, cmp) ({ \
        struct lh_node *merged = heap1, *mergee = heap2; \
        if (merged == NULL) { \
            merged = mergee; \
        } else if (mergee != NULL) { \
            if (cmp(lh_entry(merged, type, member)->key, \
                    lh_entry(mergee, type, member)->key) < 0) { \
                lh_swap(merged, mergee); \
            } \
            while (merged != NULL && mergee != NULL) { \
                if ((merged->right == NULL) || \
                    (cmp(lh_entry(merged->right, type, member)->key, \
                        lh_entry(mergee, type, member)->key) < 0)) { \
                    lh_swap(merged->right, mergee); \
                    merged->right->parent = merged; \
                    if (mergee == NULL) break; \
                } \
                merged = merged->right; \
            } \
            while (merged != NULL) { \
                if(merged->left == NULL || \
                    (merged->right != NULL && \
                        merged->left->npl < merged->right->npl)) { \
                    lh_swap(merged->left, merged->right); \
                } \
                merged->npl = merged->right != NULL ? (merged->right->npl + 1) : 0; \
                if (merged->parent == NULL) break; \
                merged = merged->parent; \
            } \
        } \
        merged; \
    })

/**
 * Swap two heaps.
 *
 * @param heap1 first heap
 * @param heap2 second heap
 */
#define lh_swap(heap1, heap2) ({ \
        struct lh_node *temp; \
        temp = (heap1); \
        (heap1) = (heap2); \
        (heap2) = temp; \
    })

/**
 * Add node to heap.
 *
 * @param heap heap
 * @param type type of the struct this is embedded in
 * @param member name of the list structure within the struct
 * @param key name of the key item within the struct
 * @param item item to insert into the heap
 * @param cmp comparison function
 */
#define lh_insert(heap, type, member, key, item, cmp) \
    (heap)->node = lh_merge((heap->node), (item), type, member, key, cmp)

/**
 * Delete node with minimal key value from heap.
 *
 * @param heap heap
 * @param type type of the struct this is embedded in
 * @param member name of the list structure within the struct
 * @param key name of the key item within the struct
 * @param value value to delete from heap
 * @param cmp comparison function
 */
#define lh_del_min(heap, type, member, key, cmp) \
    (heap)->node = lh_merge((heap->node)->left, (heap->node)->right, type, member, key, cmp)

/**
 * Find node with minimal key in heap.
 *
 * @param heap heap
 * @return node with minimal key
 */
#define lh_min(heap) (heap->node)

/**
 * Delete node from heap.
 *
 * @param heap heap
 * @param type type of the struct this is embedded in
 * @param member name of the list structure within the struct
 * @param key name of the key item within the struct
 * @param item item to delete from heap
 * @param cmp comparison function
 */
#define lh_del(heap, type, member, key, item, cmp) ({ \
        struct lh_node *new, *parent; \
        if ((item)->left != NULL) (item)->left->parent = NULL; \
        if ((item)->right != NULL) (item)->right->parent = NULL; \
        new = lh_merge((item)->left, (item)->right, type, member, key, cmp); \
        (item)->left = (item)->right = NULL; \
        parent = (item)->parent; \
        (item)->parent = NULL; \
        if (parent != NULL) { \
            parent->npl = 0; \
            if (parent->right != (item)) { \
                parent->left = parent->right; \
            } \
            parent->right = NULL; \
            while (parent->parent != NULL) { \
                parent = parent->parent; \
                if (parent->right != NULL && parent->right->npl > parent->left->npl) { \
                    lh_swap(parent->left, parent->right); \
                } \
                parent->npl = parent->right != NULL ? (parent->right->npl + 1) : 0; \
            } \
            (heap)->node = lh_merge((heap)->node, new, type, member, key, cmp); \
        } else { \
            (heap)->node = new; \
        } \
    })

#endif /* !HEAP_H_ */
