#include "rockre.h"
#include <stdlib.h>
#include <string.h>

static rockre_code* new_code() {
  rockre_code* code = malloc(sizeof(rockre_code));
  memset(code, 0, sizeof(rockre_code));
  return code;
}

static rockre_code* gen(rockre_node* node)
{
  switch (node->type) {
  case ROCKRE_NODE_STRING:
    {
      size_t i;
      rockre_code *v = malloc(sizeof(rockre_code)*node->string->len);
      for (i=0; i<node->string->len; ++i) {
        v[i].opcode = ROCKRE_OP_CHAR;
        v[i].c      = node->string->ptr[i];
      }
      return v;
    }
  default:
    abort();
  }
}

static void push(rockre_irep* irep, rockre_op op, int c, rockre_code* x, rockre_code*y)
{
  irep->nops++;
  irep->ops = realloc(irep->ops, sizeof(rockre_code)*irep->nops);
  if (!irep->ops) {
  }
  irep->ops[irep->nops-1].opcode = op;
  irep->ops[irep->nops-1].c = c;
}

rockre_irep * rockre_codegen(rockre_node* node)
{
  rockre_irep* irep = gen(node);
  push(irep, ROCKRE_OP_FINISH, 0, NULL, NULL);
  return irep;
}

void rockre_irep_free(rockre_irep* irep)
{
  free(irep->ops);
  free(irep);
}

