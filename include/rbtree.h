#ifndef RBTREE_H_
#define RBTREE_H_ 1

#include <stddef.h>
#include <kernel.h>

/** Red Black tree node */
typedef struct rb_node {
    unsigned long par_color;
#define RB_RED      0
#define RB_BLACK    1
    struct rb_node *right;
    struct rb_node *left;
};

/** Red Black tree root */
struct rb_root {
    struct rb_node *node;
};

#define rb_parent(r) ((struct rb_node *)((r)->par_color & ~3))
#define rb_color(r) ((r)->par_color & 1)
#define rb_is_red(r) (!rb_color(r))
#define rb_is_black(r) rb_color(r)
#define rb_set_red(r) do { (r)->par_color &= ~1; } while (0)
#define rb_set_black(r) do { (r)->par_color |= 1; } while (0)

#define RB_ROOT (struct rb_root) { NULL, }

#define RB_EMPTY_ROOT(root) ((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node) (rb_parent(node) == node)
#define RB_CLEAR_NODE(node) (rb_set_parent(node, node))

/**
 * Set node parent color in red black tree.
 *
 * @param node given node
 * @param par parent node
 */
static inline void rb_set_parent(struct rb_node *node, struct rb_node *par) {
    node->par_color = (node->par_color & 3) | (unsigned long)par;
}

/**
 * Set node color in red black tree.
 *
 * @param node given node
 * @param color color (red/black)
 */
static inline void rb_set_color(struct rb_node *node, int color) {
    node->par_color = (node->par_color & ~1) | color;
}

/**
 * Link node with given node in red black tree.
 *
 * @param node node to link
 * @param parent node parent
 * @param rb_link node to link in
 */
static inline void rb_link_node(struct rb_node *node, struct rb_node *parent,
                struct rb_node **rb_link) {
    node->par_color = (unsigned long )parent;
    node->left = node->right = NULL;

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
        struct rb_node *node = root->node; \
        while (node) { \
            int result = cmp(rb_entry(node, type, member)->key, value); \
            if (result < 0) { \
                node = node->left; \
            } else if (result > 0) { \
                node = node->right; \
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
        struct rb_node **new = &(root->node), *parent = NULL; \
        while (*new) { \
            int result = cmp(rb_entry(*new, type, member)->key, \
                rb_entry(item, type, member)->key); \
            parent = *new; \
            if (result < 0) { \
                new = &((*new)->left); \
            } else if (result > 0) { \
                new = &((*new)->right); \
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
#define rb_del(root, type, member, key, value, cmp) ({ \
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
    for (pos = rb_first(root), n = rb_next(pos); pos; \
        pos = n, n = rb_next(pos))

/**
 * Iterate over red black tree of given type.
 *
 * @param pos struct tree node to use as a loop counter
 * @param root root for your tree
 * @param member name of the tree structure within the struct
 */
#define rb_for_each_entry(pos, root, member) \
    for (pos = rb_entry(rb_first(root), typeof(*pos), member); \
         pos; \
         pos = rb_entry(rb_next(pos->member), typeof(*pos), member))

/**
 * Iterate over list of given type safe against removal of list entry.
 *
 * @param pos struct tree node to use as a loop counter
 * @param n another type pointer to use as temporary storage
 * @param root root for your tree
 * @param member name of the tree structure within the struct
 */
#define rb_for_each_entry_safe(pos, n, root, member) \
    for (pos = rb_entry(rb_first(root), typeof(*pos), member), \
         n = rb_entry(rb_next(pos->member), typeof(*pos), member); \
         pos; \
         pos = n, n = rb_entry(rb_next(&n->member), typeof(*n), member))

/* Externals are commented with implementation */
extern void rb_insert_color(struct rb_node *node, struct rb_root *root);
extern void rb_erase(struct rb_node *node, struct rb_root *root);

extern struct rb_node *rb_next(struct rb_node *node);
extern struct rb_node *rb_prev(struct rb_node *node);
extern struct rb_node *rb_first(struct rb_root *root);
extern struct rb_node *rb_last(struct rb_root *root);

extern void rb_replace_node(struct rb_node *victim, struct rb_node *new,  struct rb_root *root);

#endif /* !RBTREE_H_ */
