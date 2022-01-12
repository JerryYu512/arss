/**
 * MIT License
 * 
 * Copyright © 2021 <wotsen>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @file rbtree.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-18
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <stddef.h>
#include "brsdk/defs/defs.hpp"

namespace brsdk {

namespace ds {

typedef struct rb_node {
    unsigned long  __rb_parent_color;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
} rb_node_t;

typedef struct rb_root {
    struct rb_node *rb_node;
} rb_root_t;

/**
 * @brief 比较接口
 * 
 * @param node1 节点1
 * @param node2 节点2
 * @return 0-相等,-1-node1<node2,1-node1>node2
 */
typedef int (*rb_compare_f)(const rb_node_t* node1, const rb_node_t* node2);

/**
 * @brief 搜索接口
 *
 * @param node 节点
 * @param data 目标数据
 * @param len 目标数据长度 
 * @return 0-成功
 */
typedef int (*rb_search_f)(const rb_node_t* node, const void* data, size_t len);

///< 初始化为父节点
#define rb_parent(r)   ((struct brsdk::ds::rb_node *)((r)->__rb_parent_color & ~3))

///< 根
#define RB_ROOT (struct brsdk::ds::rb_root) { NULL, }

///< 由节点地址获取对象首地址
#define rb_entry(ptr, type, member) container_of(ptr, type, member)

///< 是否是空的数
#define RB_EMPTY_ROOT(root)  ((root)->rb_node == NULL)

/* 'empty' nodes are nodes that are known not to be inserted in an rbree */
#define RB_EMPTY_NODE(node)  \
        ((node)->__rb_parent_color == (unsigned long)(node))
#define RB_CLEAR_NODE(node)  \
        ((node)->__rb_parent_color = (unsigned long)(node))

/**
 * @brief 插入
 * 
 * @param node 节点
 * @param root 根
 */
void rb_insert_color(struct rb_node* node, struct rb_root* root);

/**
 * @brief 删除
 * 
 * @param node 节点
 * @param root 根
 */
void rb_erase(struct rb_node* node, struct rb_root* root);

/**
 * @brief 插入数据
 * 
 * @param root 根
 * @param new_node 新节点
 * @param cmp 比较函数
 * @return int 0-成功
 */
int rb_insert(rb_root_t* root, rb_node_t* new_node, rb_compare_f cmp);

/**
 * @brief 移除数据
 * 
 * @param root 根
 * @param data 移除的数据
 * @param len 数据长度
 * @param cmp 比较函数
 * @return rb_node_t* 移除的节点
 */
rb_node_t* rb_remove(rb_root_t* root, const void* data, size_t len, rb_search_f cmp);
rb_node_t* rb_search(rb_root_t* root, const void* data, size_t len, rb_search_f cmp);

/* Find logical next and previous nodes in a tree */
struct rb_node *rb_next(const struct rb_node *);
struct rb_node *rb_prev(const struct rb_node *);
struct rb_node *rb_first(const struct rb_root *);
struct rb_node *rb_last(const struct rb_root *);

/* Postorder iteration - always visit the parent after its children */
struct rb_node *rb_first_postorder(const struct rb_root *);
struct rb_node *rb_next_postorder(const struct rb_node *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
void rb_replace_node(struct rb_node *victim, struct rb_node *_new,
                struct rb_root *root);

static inline void rb_link_node(struct rb_node * node, struct rb_node * parent,
                struct rb_node ** rb_link)
{
    node->__rb_parent_color = (unsigned long)parent;
    node->rb_left = node->rb_right = NULL;
    *rb_link = node;
}

#define rb_entry_safe(ptr, type, member) \
        ({ BRSDK_TYPEOF(ptr) ____ptr = (ptr); \
         ____ptr ? rb_entry(____ptr, type, member) : NULL; \
         })

/**
 * rbtree_postorder_for_each_entry_safe - iterate over rb_root in post order of
 * given type safe against removal of rb_node entry
 *
 * @pos:    the 'type *' to use as a loop cursor.
 * @n:      another 'type *' to use as temporary storage
 * @root:   'rb_root *' of the rbtree.
 * @field:  the name of the rb_node field within 'type'.
 */
#define rbtree_postorder_for_each_entry_safe(pos, n, root, field) \
    for (pos = rb_entry_safe(brsdk::ds::rb_first_postorder(root), BRSDK_TYPEOF(*pos), field); \
         pos && ({ n = rb_entry_safe(brsdk::ds::rb_next_postorder(&pos->field), \
             BRSDK_TYPEOF(*pos), field); 1; }); \
         pos = n)


} // namespace ds

} // namespace brsdk
