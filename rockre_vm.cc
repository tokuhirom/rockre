#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "rockre.h"

#define NEXT pc++; goto START

// We should care the next branch
#define FAIL return false

using namespace RockRE;

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

static bool m(const std::string str, const Irep &irep, bool is_head)
{
  // program counter
  const Code* pc = irep.codes();

  // string pointer
  size_t sp = 0;

START:
  switch (pc->op()) {
  case OP_CHAR:
    if (str[sp] == pc->c()) {
      sp++;
    } else {
      FAIL;
    }
    NEXT;
  case OP_HEAD:
    // ^^
    if (sp != 0 || !is_head) {
      FAIL;
    }
    NEXT;
  case OP_TAIL:
    if (sp != str.length()) {
      FAIL;
    }
    NEXT;
  case OP_CAPTURE:
    abort();
  case OP_SPLIT:
    abort();
    NEXT;
  case OP_FINISH:
    return true;
  }
}

bool RockRE::match(const std::string str, const Irep& irep)
{
  int i = 0;
  while (i < str.length()) {
    bool r = m(str.substr(i), irep, i==0);
    if (r) {
      return true;
    }
    ++i;
  }
  return false;
}
