#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include "rockre.h"
#include "nanoutf8.h"

#define NEXT pc++; goto START
#define JMP goto START
/// #define VM_DEBUG

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

static bool m(const std::string str, const Irep &irep, bool is_head, std::map<int, std::string>& captured)
{
  // string pointer
  size_t sp = 0;
  size_t pc = 0;

  std::map<int, size_t> saved;

  std::vector<Thread> threads;

  // todo: direct threaded code
START:
  // printf("trace pc:%zu, sp:%zu\n", pc, sp);
  switch (irep[pc].op()) {
  case OP_CHAR:
    if (str[sp] == irep[pc].c()) {
#ifdef VM_DEBUG
      printf("CHAR MATCH OK\n");
#endif
      sp++;
      NEXT;
    } else {
#ifdef VM_DEBUG
      printf("CHAR MATCH FAIL sp:%zx, %x %x\n", sp, str[sp], irep[pc].c());
#endif
      FAIL;
    }
  case OP_ANYCHAR:
    {
      size_t n = nanoutf8_next_size(str[sp]);
#ifdef VM_DEBUG
      printf("%x %zu, %zu %zu\n", str[0], str.length(), sp, n);
#endif
      if (str.length() > sp + n-1) {
#ifdef VM_DEBUG
        printf("ANYCHAR MATCH\n");
#endif
        sp += n;
        NEXT;
      } else {
        FAIL;
      }
    }
  case OP_LINEHEAD:
    abort();
  case OP_LINETAIL:
    abort();
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
    saved[irep[pc].a()] = sp;
    NEXT;
  case OP_MATCH:
    int a = irep[pc].a();
    captured[a] = str.substr(saved[a], sp - saved[a]);
    NEXT;
  }
}

bool RockRE::partial_match(const std::string str, const Irep& irep, std::map<int,std::string> &captured)
{
  size_t i = 0;
  while (i < str.length()) {
    bool r = m(str.substr(i), irep, i==0, captured);
    if (r) {
      return true;
    }
    i += nanoutf8_next_size(str[0]);
  }
  return false;
}

bool RockRE::full_match(const std::string str, const Irep& irep, std::map<int,std::string> &captured)
{
  return m(str, irep, true, captured);
}
