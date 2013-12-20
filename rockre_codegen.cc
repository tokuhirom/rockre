#include "rockre.h"
#include <stdlib.h>

using namespace RockRE;

static void gen(Irep& irep, const Node& node)
{
  switch (node.type()) {
  case NODE_STRING:
    {
      for (const char& c: node.string()) {
        irep.push(OP_CHAR, c);
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
      //   SPLIT L1, L2
      // L1:
      //   match a
      //   JMP L3
      // L2:
      //   match b
      // L3:
      /*
      std::shared_ptr<RockRE::Irep> x(new RockRE::Irep);
      std::shared_ptr<RockRE::Irep> y(new RockRE::Irep);
      irep->push(OP_SPLIT, x, y);
      return;
      */
        abort();
    }
  case NODE_HEAD:
    irep.push(OP_HEAD);
    return;
  case NODE_TAIL:
    irep.push(OP_TAIL);
    return;
default:
    abort();
  }
  abort();
}

void RockRE::codegen(const RockRE::Node& node, RockRE::Irep& irep)
{
  gen(irep, node);
  irep.push(OP_FINISH);
}

