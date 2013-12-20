#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <iostream>
#include "rockre.h"

#define NEXT pc++; goto START
#define JMP goto START

// We should care the next branch
#define FAIL \
  do { \
    if (threads.size() == 0) { \
      return false; \
    } else { \
      sp = threads.back().sp; \
      pc = threads.back().pc; \
      threads.pop_back(); \
      JMP; \
    } \
  } while(0)

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


namespace RockRE {
  struct Thread {
    size_t pc;
    size_t sp;
    Thread(size_t s, size_t p) {
      pc = p;
      sp = s;
    }
  };
};

static bool m(const std::string str, const Irep &irep, bool is_head, size_t pc)
{
  // string pointer
  size_t sp = 0;

  std::vector<Thread> threads;

  // todo: direct threaded code
START:
  // printf("trace pc:%zu, sp:%zu\n", pc, sp);
  switch (irep[pc].op()) {
  case OP_CHAR:
    if (str[sp] == irep[pc].c()) {
      sp++;
      NEXT;
    } else {
      FAIL;
    }
  case OP_ANYCHAR:
    if (str.length() > sp) {
      sp++;
      NEXT;
    } else {
      FAIL;
    }
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
  case OP_JMP:
    pc += irep[pc].a();
    JMP;
  case OP_SPLIT:
    threads.emplace_back(sp, pc + irep[pc].b());
    pc += irep[pc].a();
    JMP;
  case OP_FINISH:
    return true;
  case OP_SAVE:
    NEXT;
  case OP_MATCH:
    NEXT;
  }
}

bool RockRE::match(const std::string str, const Irep& irep)
{
  int i = 0;
  while (i < str.length()) {
    bool r = m(str.substr(i), irep, i==0, 0);
    if (r) {
      return true;
    }
    ++i;
  }
  return false;
}
