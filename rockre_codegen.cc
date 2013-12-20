#include "rockre.h"
#include <stdlib.h>
#include <iostream>
#include <assert.h>

using namespace RockRE;

const char* RockRE::op_name(OPType t)
{
  switch (t) {
  case OP_SPLIT:
    return "SPLIT";
  case OP_HEAD:
    return "HEAD";
  case OP_TAIL:
    return "TAIL";
  case OP_CAPTURE:
    return "CAPTURE";
  case OP_FINISH:
    return "FINISH";
  case OP_CHAR:
    return "CHAR";
  case OP_JMP:
    return "JMP";
  }
}

void RockRE::dump_irep(const Irep& irep)
{
  std::cout << "-- irep --" << std::endl;
  int i = 0;
  for (auto c: irep) {
    std::cout << i << " " <<  op_name(c.op());
    switch (c.op()) {
    case OP_SPLIT:
      std::cout << " " << (int)c.a() << ", " << (int)c.b();
      break;
    case OP_JMP:
      std::cout << " " << (int)c.a();
      break;
    default:
      break; // nop
    }
    std::cout << std::endl;
    ++i;
  }
  std::cout << "-- /irep --" << std::endl;
}

static void gen(Irep& irep, const Node& node)
{
  switch (node.type()) {
  case NODE_STRING:
    {
      for (const char& c: node.string()) {
        irep.emplace_back(OP_CHAR, c);
      }
      return;
    }
  case NODE_LIST:
    {
      for (auto child: node.children()) {
        gen(irep, child);
      }
      return;
    }
  case NODE_OR:
    {
      // a|b
      // 
      // L0:
      //   SPLIT L1, L2
      // L1:
      //   match a
      //   JMP L3
      // L2:
      //   match b
      // L3:

      size_t label0 = irep.size();
      irep.emplace_back(OP_SPLIT);
      auto split = irep.size()-1;

      // L1:
      size_t label1 = irep.size();
      gen(irep, node.children()[0]);
      irep.emplace_back(OP_JMP);
      auto jmp_l3 = irep.size()-1;

      // L2:
      size_t label2 = irep.size();
      gen(irep, node.children()[1]);

      // L3
      size_t label3 = irep.size();

      // fix pos.
      irep[split].a(label1 - label0);
      irep[split].b(label2 - label0);

      irep[jmp_l3].a(label3 - label2 + 1);

      return;
    }
  case NODE_HEAD:
    irep.emplace_back(OP_HEAD);
    return;
  case NODE_TAIL:
    irep.emplace_back(OP_TAIL);
    return;
  default:
    abort();
  }
  abort();
}

void RockRE::codegen(const RockRE::Node& node, RockRE::Irep& irep)
{
  gen(irep, node);
  irep.emplace_back(OP_FINISH);
}

