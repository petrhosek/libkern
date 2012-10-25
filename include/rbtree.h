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

#ifndef RBTREE_H_
#define RBTREE_H_

#include "kernel.h"

#include <stddef.h>

/** Red Black tree node */
struct rb_node {
    unsigned long rb_parent_color;
#define RB_RED      0
#define RB_BLACK    1
    struct rb_node *rb_right;
    struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));

/** Red Black tree root */
struct rb_root {
    struct rb_node *rb_node;
};

#define rb_parent(r) ((struct rb_node *)((r)->rb_parent_color & ~3))
#define rb_color(r) ((r)->rb_parent_color & 1)
#define rb_is_red(r) (!rb_color(r))
#define rb_is_black(r) rb_color(r)
#define rb_set_red(r) do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r) do { (r)->rb_parent_color |= 1; } while (0)

/**
 * Set node parent color in red black tree.
 *
 * @param node given node
 * @param par parent node
 */
static inline void rb_set_parent(struct rb_node *node, struct rb_node *par) {
    node->rb_parent_color = (node->rb_parent_color & 3) | (unsigned long)par;
}

/**
 * Set node color in red black tree.
 *
 * @param node given node
 * @param color color (red/black)
 */
static inline void rb_set_color(struct rb_node *node, int color) {
    node->rb_parent_color = (node->rb_parent_color & ~1) | color;
}

#define RB_ROOT (struct rb_root) { NULL, }

#define RB_EMPTY_ROOT(root) ((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node) (rb_parent(node) == node)
#define RB_CLEAR_NODE(node) (rb_set_parent(node, node))

/**
 * Initialize the tree node structure.
 *
 * @param node given node
 */
static inline void rb_init_node(struct rb_node *rb) {
    rb->rb_parent_color = 0;
    rb->rb_right = NULL;
    rb->rb_left = NULL;
    RB_CLEAR_NODE(rb);
}

/* Externals are commented with implementation */
extern void rb_insert_color(struct rb_node *node, struct rb_root *root);
extern void rb_erase(struct rb_node *node, struct rb_root *root);

typedef void (*rb_augment_f)(struct rb_node *node, void *data);

extern void rb_augment_insert(struct rb_node *node, rb_augment_f func, void *data);
extern struct rb_node *rb_augment_erase_begin(struct rb_node *node);
extern void rb_augment_erase_end(struct rb_node *node, rb_augment_f func, void *data);

extern struct rb_node *rb_next(struct rb_node *node);
extern struct rb_node *rb_prev(struct rb_node *node);
extern struct rb_node *rb_first(struct rb_root *root);
extern struct rb_node *rb_last(struct rb_root *root);

extern void rb_replace_node(struct rb_node *victim, struct rb_node *new,  struct rb_root *root);

/**
 * Link node with given node in red black tree.
 *
 * @param node node to link
 * @param parent node parent
 * @param rb_link node to link in
 */
static inline void rb_link_node(struct rb_node *node, struct rb_node *parent,
                struct rb_node **rb_link) {
    node->rb_parent_color = (unsigned long)parent;
    node->rb_left = node->rb_right = NULL;

    *rb_link = node;
}

/**
 * Get the struct for this entry.
 *
 * @param ptr struct list head pointer
 * @param type type of the struct this is embedded in
 * @param member name of the list structure within the struct
 */
#define rb_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * Look for value in red black tree.
 *
 * @param root tree root
 * @param type type of the struct this is embedded in
 * @param member name of the list structure within the struct
 * @param key name of the key item within the struct
 * @param value value to look for in the tree
 * @param cmp comparison function
 * @return found node or NULL
 */
#define rb_find(root, type, member, key, value, cmp) ({ \
        bool found = false; \
        struct rb_node *node = root->rb_node; \
        while (node) { \
            int result = cmp(rb_entry(node, type, member)->key, value); \
            if (result < 0) { \
                node = node->rb_left; \
            } else if (result > 0) { \
                node = node->rb_right; \
            } else { \
                found = true; \
                break; \
            } \
        } \
        found ? rb_entry(node, type, member) : NULL; \
    })

/**
 * Add node to red black tree.
 *
 * @param root tree root
 * @param type type of the struct this is embedded in
 * @param member name of the list structure within the struct
 * @param key name of the key item within the struct
 * @param item item to insert into the tree
 * @param cmp comparison function
 */
#define rb_insert(root, type, member, key, item, cmp) ({ \
        bool insert = true; \
        struct rb_node **new = &(root->rb_node), *parent = NULL; \
        while (*new) { \
            int result = cmp(rb_entry(*new, type, member)->key, \
                rb_entry(item, type, member)->key); \
            parent = *new; \
            if (result < 0) { \
                new = &((*new)->rb_left); \
            } else if (result > 0) { \
                new = &((*new)->rb_right); \
            } else { \
                insert = false; \
                break; \
            } \
        } \
        if (insert) { \
            rb_link_node(item, parent, new); \
            rb_insert_color(item, root); \
        } \
    })

/**
 * Delete node with given value from red black tree.
 *
 * @param root tree root
 * @param type type of the struct this is embedded in
 * @param member name of the list structure within the struct
 * @param key name of the key item within the struct
 * @param value value to delete from tree
 * @param cmp comparison function
 */
#define rb_delete(root, type, member, key, value, cmp) ({ \
        struct rb_node *node = rb_find(root, type, member, key, value, cmp); \
        if (node) { \
            rb_erase(node, root); \
        } \
    })

/**
 * Iterate over a red black tree.
 *
 * @param pos struct tree node to use as a loop counter
 * @param root root for your tree
 */
#define rb_for_each(pos, root) \
    for (pos = rb_first(root); pos; pos = rb_next(pos))

/**
 * Iterate over a red black tree backwards.
 *
 * @param pos struct tree node to use as a loop counter
 * @param root root for your tree
 */
#define rb_for_each_prev(pos, root) \
    for (pos = rb_last(root); pos; pos = rb_prev(pos))

/**
 * Iterate over a red black tree safe against removal of list entry
 *
 * @param pos struct tree node to use as a loop counter
 * @param n another struct list head to use as temporary storage
 * @param root the root for your tree
 */
#define rb_for_each_safe(pos, n, root) \
    for (pos = rb_first(root); pos && ({ n = rb_next(pos); 1; }); \
         pos = n)

/**
 * Iterate over red black tree of given type.
 *
 * @param tpos type pointer to use as a loop cursor
 * @param pos node pointer to use as a loop cursor
 * @param root root for your tree
 * @param member name of the tree structure within the struct
 */
#define rb_for_each_entry(tpos, pos, root, member) \
    for (pos = rb_first(root); \
         pos && ({ tpos = rb_entry(pos, typeof(*tpos), member); 1;}); \
         pos = rb_next(pos))

/**
 * Iterate over list of given type safe against removal of list entry.
 *
 * @param tpos type pointer to use as a loop cursor
 * @param pos struct tree node to use as a loop counter
 * @param n another type pointer to use as temporary storage
 * @param root root for your tree
 * @param member name of the tree structure within the struct
 */
#define rb_for_each_entry_safe(tpos, pos, n, root, member) \
    for (pos = rb_first(root); \
         pos && ({ n = rb_next(pos); 1; }) && ({ tpos = rb_entry(pos, typeof(*tpos), member); 1;}); \
         pos = n)

#endif // RBTREE_H_
