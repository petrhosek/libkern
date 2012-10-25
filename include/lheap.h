/*
 * This file is part of libkern.
 *
 * libkern is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libkern is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libkern.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LHEAP_H_
#define LHEAP_H_

#include "kernel.h"

#include <stddef.h>

/** Leftist heap tree node */
struct lh_node {
    struct lh_node *lh_parent;
    struct lh_node *lh_left;
    struct lh_node *lh_right;
    int lh_npl;
};

/** Leftist heap */
struct lh_heap {
    struct lh_node *lh_node;
};

#define lh_heap(name) struct lh_node name = { NULL }
#define lh_heap_init(root) do { \
        (root)->lh_node = NULL; \
    } while (0)

#define lh_heap_empty(root) ((root)->lh_node == NULL)

#define lh_node_empty(node) (node) == NULL
#define lh_node_init(node)  do { \
        (node)->lh_parent = NULL; \
        (node)->lh_left = (node)->lh_right = NULL; \
        (node)->lh_npl = 0; \
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
                if ((merged->lh_right == NULL) || \
                    (cmp(lh_entry(merged->lh_right, type, member)->key, \
                        lh_entry(mergee, type, member)->key) < 0)) { \
                    lh_swap(merged->lh_right, mergee); \
                    merged->lh_right->lh_parent = merged; \
                    if (mergee == NULL) break; \
                } \
                merged = merged->lh_right; \
            } \
            while (merged != NULL) { \
                if(merged->lh_left == NULL || \
                    (merged->lh_right != NULL && \
                        merged->lh_left->lh_npl < merged->lh_right->lh_npl)) { \
                    lh_swap(merged->lh_left, merged->lh_right); \
                } \
                merged->lh_npl = merged->lh_right != NULL ? (merged->lh_right->lh_npl + 1) : 0; \
                if (merged->lh_parent == NULL) break; \
                merged = merged->lh_parent; \
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
    (heap)->lh_node = lh_merge((heap->lh_node), (item), type, member, key, cmp)

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
    (heap)->lh_node = lh_merge((heap->lh_node)->lh_left, (heap->lh_node)->lh_right, type, member, key, cmp)

/**
 * Find node with minimal key in heap.
 *
 * @param heap heap
 * @return node with minimal key
 */
#define lh_min(heap) (heap->lh_node)

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
        if ((item)->lh_left != NULL) (item)->lh_left->lh_parent = NULL; \
        if ((item)->lh_right != NULL) (item)->lh_right->lh_parent = NULL; \
        new = lh_merge((item)->lh_left, (item)->lh_right, type, member, key, cmp); \
        (item)->lh_left = (item)->lh_right = NULL; \
        parent = (item)->lh_parent; \
        (item)->lh_parent = NULL; \
        if (parent != NULL) { \
            parent->lh_npl = 0; \
            if (parent->lh_right != (item)) { \
                parent->lh_left = parent->lh_right; \
            } \
            parent->lh_right = NULL; \
            while (parent->lh_parent != NULL) { \
                parent = parent->lh_parent; \
                if (parent->lh_right != NULL && parent->lh_right->lh_npl > parent->lh_left->lh_npl) { \
                    lh_swap(parent->lh_left, parent->lh_right); \
                } \
                parent->lh_npl = parent->lh_right != NULL ? (parent->lh_right->lh_npl + 1) : 0; \
            } \
            (heap)->lh_node = lh_merge((heap)->lh_node, new, type, member, key, cmp); \
        } else { \
            (heap)->lh_node = new; \
        } \
    })

#endif // HEAP_H_
