#ifndef ROCKRE_H_
#define ROCKRE_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum rockre_node_type {
    ROCKRE_NODE_LIST = 1,
    ROCKRE_NODE_OR,
    ROCKRE_NODE_TAIL,
    ROCKRE_NODE_HEAD,
    ROCKRE_NODE_STRING,
} rockre_node_type;

typedef struct rockre_string {
    const char* ptr;
    size_t len;
} rockre_string;

typedef struct rockre_node {
    enum rockre_node_type type;
    union {
        rockre_string* string;
        struct rockre_node* child;
    };
    struct rockre_node*next;
} rockre_node;

/* parser api */
rockre_node *rockre_parse(const char *str, size_t len);

/* node api */
rockre_node* rockre_node_new(enum rockre_node_type t);
rockre_node* rockre_node_new1(enum rockre_node_type t, rockre_node* child1);
void rockre_node_push_child(rockre_node* a, rockre_node* b);
rockre_node* rockre_node_new_string(enum rockre_node_type t, const char* str, size_t len);
void rockre_node_dump(rockre_node* node, int depth);

/**
 * String functions.
 */
const char *rockre_string_cstr(rockre_string* str);
rockre_string* rockre_string_new(const char* ptr, size_t len);
void rockre_string_free(rockre_string* str);

#ifdef __cplusplus
};
#endif

#endif // ROCKRE_H_
