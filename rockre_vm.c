#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "rockre.h"

#define NEXT pc++; goto START

// We should care the next branch
#define FAIL return false


/**
 * compile a|b:
 *
 *   SPLIT L1, L2
 * L1:
 *   STRING a
 *   JMP L3
 * L2:
 *   STRING b
 * L3
 */

bool rockre_vm_run(const char *str, size_t len, rockre_irep* irep)
{
  // programming counter
  rockre_code* pc = irep->ops;

  // string pointer
  const char *sp = str;

START:
  switch (pc->opcode) {
  case ROCKRE_OP_CHAR:
    if (*sp == pc->c) {
      sp++;
    }
    NEXT;
  case ROCKRE_OP_STRING:
    abort();
    NEXT;
  case ROCKRE_OP_HEAD:
    // ^^
    if (sp != str) {
      FAIL;
    }
    NEXT;
  case ROCKRE_OP_TAIL:
    if (sp != str+len) {
      FAIL;
    }
    NEXT;
  case ROCKRE_OP_CAPTURE:
    abort();
  case ROCKRE_OP_SPLIT:
    abort();
    NEXT;
  case ROCKRE_OP_FINISH:
    return true;
  }
}
