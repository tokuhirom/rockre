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

void rockre_node_free(rockre_node* node) {
  free(node);
}

rockre_node* rockre_node_new1(enum rockre_node_type t, rockre_node* child1) {
    rockre_node* node = rockre_node_new(t);
    node->child = child1;
    return node;
}

void rockre_node_push_child(rockre_node* a, rockre_node* b)
{
  if (a->child) {
    rockre_node* c = a->child;
    while (c->next) {
      c = c->next;
    }
    c->next = b;
  } else {
    a->child = b;
  }
}

rockre_node* rockre_node_new_string(enum rockre_node_type t, const char* str, size_t len) {
    rockre_node* node = rockre_node_new(t);
    if (!node) {
      return NULL;
    }

    node->string = rockre_string_new(str, len);
    return node;
}

rockre_node* rockre_node_string_plus(rockre_node* node1, rockre_node* node2) {
  rockre_string* str = rockre_string_plus(node1->string, node2->string);
  if (!str) {
    return NULL;
  }

  rockre_node* node = rockre_node_new_string(ROCKRE_NODE_STRING, str->ptr, str->len);
  rockre_string_free(str);
  return node;
}

void rockre_node_dump(rockre_node* node, int depth)
{
  switch (node->type) {
  case ROCKRE_NODE_STRING:
    printf("(string \"%s\")", rockre_string_cstr(node->string));
    return;
  case ROCKRE_NODE_TAIL:
    printf("(tail)");
    return;
  case ROCKRE_NODE_HEAD:
    printf("(head)");
    return;
  case ROCKRE_NODE_ANYCHAR:
    printf("(anychar)");
    return;
  case ROCKRE_NODE_OR:
    {
      printf("(or ");
      rockre_node* n = node->child;
      while (n) {
        rockre_node_dump(n, depth+1);
        if (n->next) {
          printf(" ");
        }
        n = n->next;
      }
      printf(")");
      return;
    }
  case ROCKRE_NODE_LIST:
    {
      printf("(list ");
      rockre_node* n = node->child;
      while (n) {
        rockre_node_dump(n, depth+1);
        if (n->next) {
          printf(" ");
        }
        n = n->next;
      }
      printf(")");
      return;
    }
  case ROCKRE_NODE_GROUP:
    {
      printf("(group ");
      rockre_node_dump(node->child, depth+1);
      printf(")");
      return;
    }
  case ROCKRE_NODE_CAPTURE:
    {
      printf("(capture ");
      rockre_node_dump(node->child, depth+1);
      printf(")");
      return;
    }
  }
  abort();
}


