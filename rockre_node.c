#include "rockre.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Should we use longjmp if malloc failed.
 */
rockre_node* rockre_node_new(enum rockre_node_type t) {
  rockre_node* node = malloc(sizeof(rockre_node));
  if (!node) {
    return node;
  }
  node->type = t;
  return node;
}

rockre_node* rockre_node_new1(enum rockre_node_type t, rockre_node* child1) {
    rockre_node* node = rockre_node_new(t);
    node->next = child1;
    return node;
}

void rockre_node_push_child(rockre_node* a, rockre_node* b)
{
    rockre_node* c = a->child;
    while (c->next) {
        c = c->next;
    }
    c->next = b;
}

rockre_node* rockre_node_new_string(enum rockre_node_type t, const char* str, size_t len) {
    rockre_node* node = rockre_node_new(t);
    if (!node) {
      return NULL;
    }

    node->string = rockre_string_new(str, len);
    return node;
}

void rockre_node_dump(rockre_node* node, int depth)
{
    switch (node->type) {
    case ROCKRE_NODE_STRING:
        printf("(string \"%s\")", rockre_string_cstr(node->string));
        break;
    case ROCKRE_NODE_TAIL:
        printf("(tail)");
        break;
    case ROCKRE_NODE_HEAD:
        printf("(head)");
        break;
    case ROCKRE_NODE_OR:
        printf("OR!!");
        break;
    case ROCKRE_NODE_LIST:
        printf("LIST!!");
        break;
    default:
        abort();
    }
}


