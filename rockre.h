#ifndef ROCKRE_H_
#define ROCKRE_H_

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum rockre_node_type {
    ROCKRE_NODE_LIST = 1,
    ROCKRE_NODE_OR,
    ROCKRE_NODE_TAIL,
    ROCKRE_NODE_HEAD,
    ROCKRE_NODE_STRING,
    ROCKRE_NODE_CAPTURE,
    ROCKRE_NODE_GROUP,
    ROCKRE_NODE_ANYCHAR,
} rockre_node_type;

typedef enum rockre_op {
  ROCKRE_OP_STRING,
  ROCKRE_OP_HEAD,
  ROCKRE_OP_TAIL,
  ROCKRE_OP_CAPTURE,
  ROCKRE_OP_SPLIT,
  ROCKRE_OP_FINISH,
  ROCKRE_OP_CHAR,
} rockre_op;


typedef struct rockre_string {
    char* ptr;
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

typedef struct rockre_code {
  rockre_op opcode;
  int c;
  struct rockre_code* x;
  struct rockre_code* y;
} rockre_code;

typedef struct rockre_irep {
    rockre_code** ops;
    size_t nops;
} rockre_irep;

/* parser api */
rockre_node *rockre_parse(const char *str, size_t len);

// Code generator API
rockre_irep * rockre_codegen(rockre_node* node);
void rockre_irep_free(rockre_irep* irep);

// VM API
bool rockre_vm_run(const char *str, size_t len, rockre_irep* irep);

/* node api */
rockre_node* rockre_node_new(enum rockre_node_type t);
rockre_node* rockre_node_new1(enum rockre_node_type t, rockre_node* child1);
void rockre_node_push_child(rockre_node* a, rockre_node* b);
rockre_node* rockre_node_new_string(enum rockre_node_type t, const char* str, size_t len);
void rockre_node_dump(rockre_node* node, int depth);
rockre_node* rockre_node_string_plus(rockre_node* node1, rockre_node* node2);
void rockre_node_free(rockre_node* node);

/**
 * String functions.
 */
const char *rockre_string_cstr(rockre_string* str);
rockre_string* rockre_string_new(const char* ptr, size_t len);
rockre_string* rockre_string_plus(rockre_string* str, rockre_string* str2);
void rockre_string_free(rockre_string* str);

#ifdef __cplusplus
};
#endif

#endif // ROCKRE_H_
