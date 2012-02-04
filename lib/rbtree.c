#include "rbtree.h"

/**
 * Red black tree node left rotation.
 *
 * @param node rotated node
 * @param root tree root
 */
static void __rb_rotate_left(struct rb_node *node, struct rb_root *root) {
    struct rb_node *right = node->right;
    struct rb_node *parent = rb_parent(node);

    if ((node->right = right->left))
        rb_set_parent(right->left, node);
    right->left = node;

    rb_set_parent(right, parent);

    if (parent) {
        if (node == parent->left)
            parent->left = right;
        else
            parent->right = right;
    }
    else
        root->node = right;
    rb_set_parent(node, right);
}

/**
 * Red black tree node right rotation.
 *
 * @param node rotated node
 * @param root tree root
 */
static void __rb_rotate_right(struct rb_node *node, struct rb_root *root) {
    struct rb_node *left = node->left;
    struct rb_node *parent = rb_parent(node);

    if ((node->left = left->right))
        rb_set_parent(left->right, node);
    left->right = node;

    rb_set_parent(left, parent);

    if (parent) {
        if (node == parent->right)
            parent->right = left;
        else
            parent->left = left;
    }
    else
        root->node = left;
    rb_set_parent(node, left);
}

/**
 * Insert node into red black tree and check colors.
 *
 * @param node inserted node
 * @param root tree root
 */
void rb_insert_color(struct rb_node *node, struct rb_root *root) {
    struct rb_node *parent, *gparent;

    while ((parent = rb_parent(node)) && rb_is_red(parent)) {
        gparent = rb_parent(parent);

        if (parent == gparent->left) {
            {
                register struct rb_node *uncle = gparent->right;
                if (uncle && rb_is_red(uncle)) {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->right == node) {
                register struct rb_node *tmp;
                __rb_rotate_left(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            __rb_rotate_right(gparent, root);
        } else {
            {
                register struct rb_node *uncle = gparent->left;
                if (uncle && rb_is_red(uncle)) {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->left == node) {
                register struct rb_node *tmp;
                __rb_rotate_right(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            __rb_rotate_left(gparent, root);
        }
    }

    rb_set_black(root->node);
}

/**
 * Erase node from red black tree and check colors.
 *
 * @param node erased node
 * @param parent erased node parent
 * @param root tree root
 */
static void __rb_erase_color(struct rb_node *node, struct rb_node *parent, struct rb_root *root) {
    struct rb_node *other;

    while ((!node || rb_is_black(node)) && node != root->node) {
        if (parent->left == node) {
            other = parent->right;
            if (rb_is_red(other)) {
                rb_set_black(other);
                rb_set_red(parent);
                __rb_rotate_left(parent, root);
                other = parent->right;
            }
            if ((!other->left || rb_is_black(other->left)) &&
                    (!other->right || rb_is_black(other->right))) {
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            } else {
                if (!other->right || rb_is_black(other->right)) {
                    struct rb_node *o_left;
                    if ((o_left = other->left))
                        rb_set_black(o_left);
                    rb_set_red(other);
                    __rb_rotate_right(other, root);
                    other = parent->right;
                }
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                if (other->right)
                    rb_set_black(other->right);
                __rb_rotate_left(parent, root);
                node = root->node;
                break;
            }
        } else {
            other = parent->left;
            if (rb_is_red(other)) {
                rb_set_black(other);
                rb_set_red(parent);
                __rb_rotate_right(parent, root);
                other = parent->left;
            }
            if ((!other->left || rb_is_black(other->left)) &&
                    (!other->right || rb_is_black(other->right))) {
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            } else {
                if (!other->left || rb_is_black(other->left)) {
                    register struct rb_node *o_right;
                    if ((o_right = other->right))
                        rb_set_black(o_right);
                    rb_set_red(other);
                    __rb_rotate_left(other, root);
                    other = parent->left;
                }
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                if (other->left)
                    rb_set_black(other->left);
                __rb_rotate_right(parent, root);
                node = root->node;
                break;
            }
        }
    }
    if (node)
        rb_set_black(node);
}

/**
 * Erase node from red black tree
 *
 * @param node erased node
 * @param root tree root
 */
void rb_erase(struct rb_node *node, struct rb_root *root) {
    struct rb_node *child, *parent;
    int color;

    if (!node->left)
        child = node->right;
    else if (!node->right)
        child = node->left;
    else {
        struct rb_node *old = node, *left;

        node = node->right;
        while ((left = node->left) != NULL)
            node = left;
        child = node->right;
        parent = rb_parent(node);
        color = rb_color(node);

        if (child)
            rb_set_parent(child, parent);
        if (parent == old) {
            parent->right = child;
            parent = node;
        } else
            parent->left = child;

        node->par_color = old->par_color;
        node->right = old->right;
        node->left = old->left;

        if (rb_parent(old)) {
            if (rb_parent(old)->left == old)
                rb_parent(old)->left = node;
            else
                rb_parent(old)->right = node;
        } else
            root->node = node;

        rb_set_parent(old->left, node);
        if (old->right)
            rb_set_parent(old->right, node);
        goto color;
    }

    parent = rb_parent(node);
    color = rb_color(node);

    if (child)
        rb_set_parent(child, parent);
    if (parent) {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    }
    else
        root->node = child;

color:
    if (color == RB_BLACK)
        __rb_erase_color(child, parent, root);
}

/**
 * Returns the first node (in sort order) of the red black tree.
 *
 * @param root tree root
 * @return node of tree
 */
struct rb_node *rb_first(struct rb_root *root) {
    struct rb_node  *n;

    n = root->node;
    if (!n)
        return NULL;
    while (n->left)
        n = n->left;
    return n;
}


/**
 * Returns the last node (in sort order) of the red black tree.
 *
 * @param root tree root
 * @return last node of tree
 */
struct rb_node *rb_last(struct rb_root *root) {
    struct rb_node  *n;

    n = root->node;
    if (!n)
        return NULL;
    while (n->right)
        n = n->right;
    return n;
}

/**
 * Returns the next node (in sort order) of the given node in red black tree.
 *
 * @param node node to look next node for
 * @return next node
 */
struct rb_node *rb_next(struct rb_node *node) {
    struct rb_node *parent;

    if (rb_parent(node) == node)
        return NULL;

    /* if we have a right-hand child, go down and then left as far as we can */
    if (node->right) {
        node = node->right;
        while (node->left)
            node=node->left;
        return node;
    }

    /* no right-hand children - everything down and left is smaller than us,
       so any 'next' node must be in the general direction of  our parent, go
       up the tree; any time the ancestor is a right-hand child of its parent,
       keep going up, first time it's a left-hand child of its parent, said
       parent is our 'next' node */
    while ((parent = rb_parent(node)) && node == parent->right)
        node = parent;

    return parent;
}

/**
 * Returns the previous node (in sort order) of the given node in red black
 * tree.
 *
 * @param node node to look previous node for
 * @return previous node
 */
struct rb_node *rb_prev(struct rb_node *node) {
    struct rb_node *parent;

    if (rb_parent(node) == node)
        return NULL;

    /* if we have a left-hand child, go down and then right as far as we can */
    if (node->left) {
        node = node->left; 
        while (node->right)
            node=node->right;
        return node;
    }

    /* no left-hand children, go up till we find an ancestor which is a
     * right-hand child of its parent */
    while ((parent = rb_parent(node)) && node == parent->left)
        node = parent;

    return parent;
}

/**
 * Replace node in red black node.
 *
 * @param victim node to be replaced
 * @param new node that replaces @p victim node
 * @param root tree root
 */
void rb_replace_node(struct rb_node *victim, struct rb_node *new, struct rb_root *root) {
    struct rb_node *parent = rb_parent(victim);

    /* set the surrounding nodes to point to the replacement */
    if (parent) {
        if (victim == parent->left)
            parent->left = new;
        else
            parent->right = new;
    } else {
        root->node = new;
    }
    if (victim->left)
        rb_set_parent(victim->left, new);
    if (victim->right)
        rb_set_parent(victim->right, new);

    /* copy the pointers/colour from the victim to the replacement */
    *new = *victim;
}
