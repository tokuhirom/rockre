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
  case OP_LINEHEAD:
    return "LINEHEAD";
  case OP_LINETAIL:
    return "LINETAIL";
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
  case OP_ANYCHAR:
    return "ANYCHAR";
  case OP_SAVE:
    return "SAVE";
  case OP_MATCH:
    return "MATCH";
  }
  abort();
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
      printf(" %d", c.a());
      break;
    default:
      break; // nop
    }
    std::cout << std::endl;
    ++i;
  }
  std::cout << "-- /irep --" << std::endl;
}

namespace RockRE {
  class CodeGenerator {
    size_t capture_no_;

    void gen(Irep& irep, const Node& node)
    {
      switch (node.type()) {
      case NODE_CHAR:
        irep.emplace_back(OP_CHAR, node.ch());
        return;
      case NODE_GROUP:
      case NODE_LIST:
        {
          for (auto child: node.children()) {
            gen(irep, child);
          }
          return;
        }
      case NODE_ALT:
        {
          /**
           * a|b
           *
           * L0: SPLIT L1, L2
           * L1: a
           *     JMP L3
           * L2: b
           * L3:
           */

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
      case NODE_QUEST:
        {
          /**
           * e?
           *
           * L0: SPLIT L1, L2
           * L1: e
           * L2:
           */
          // L0:
          size_t label0 = irep.size();
          irep.emplace_back(OP_SPLIT);
          auto split = irep.size()-1;

          // L1:
          size_t label1 = irep.size();
          gen(irep, node.children()[0]);

          // L2:
          size_t label2 = irep.size();

          // fix pos.
          irep[split].a(label1 - label0);
          irep[split].b(label2 - label0);
          return;
        }
      case NODE_ASTER: // *
        {
          /**
           * e*
           *
           * L0: SPLIT L1, L2
           * L1: e
           *     jmp L0
           * L2:
           */

          // L0:
          size_t label0 = irep.size();
          irep.emplace_back(OP_SPLIT);
          auto split = irep.size()-1;

          // L1:
          size_t label1 = irep.size();
          gen(irep, node.children()[0]);
          irep.emplace_back(OP_JMP);
          auto jmp = irep.size()-1;

          // L2:
          size_t label2 = irep.size();

          // fix pos.
          irep[split].a(label1 - label0);
          irep[split].b(label2 - label0);
          irep[jmp].a((int)label0 - (int)label2 + 1);
          return;
        }
      case NODE_ANYCHAR:
        irep.emplace_back(OP_ANYCHAR);
        return;
      case NODE_LINEHEAD:
        irep.emplace_back(OP_LINEHEAD);
        return;
      case NODE_LINETAIL:
        irep.emplace_back(OP_LINETAIL);
        return;
      case NODE_HEAD:
        irep.emplace_back(OP_HEAD);
        return;
      case NODE_TAIL:
        irep.emplace_back(OP_TAIL);
        return;
      case NODE_CAPTURE:
        {
          int n = capture_no_;
          capture_no_++;

          irep.emplace_back(OP_SAVE);
          irep.back().a(n);

          gen(irep, node.children()[0]);

          irep.emplace_back(OP_MATCH);
          irep.back().a(n);

          return;
        }
      case NODE_UNDEF:
        abort();
      }
      abort();
    }

  public:
    void generate(Irep& irep, const Node& node)
    {
      capture_no_ = 0;
      gen(irep, node);
    }
  };
};


void RockRE::codegen(const RockRE::Node& node, RockRE::Irep& irep)
{
  CodeGenerator cg;
  cg.generate(irep, node);
  irep.emplace_back(OP_FINISH);
}

