#include "rockre_private.h"
#include "nanoutf8.h"
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
  for (std::vector<Code>::const_iterator c=irep.begin(); c!=irep.end(); c++) {
    std::cout << i << " " <<  op_name(c->op());
    switch (c->op()) {
    case OP_SPLIT:
      std::cout << " " << (int)c->a() << ", " << (int)c->b();
      break;
    case OP_CHAR:
      {
        char buf[8];
        size_t len = nanoutf8_encode(c->c(), buf);
        buf[len] = '\0';
        std::cout << " " << buf;
        break;
      }
    case OP_JMP:
      printf(" %d", c->a());
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
        irep.push_back(Code(OP_CHAR, node.ch()));
        return;
      case NODE_GROUP:
      case NODE_LIST:
        {
          for (std::vector<Node>::const_iterator child=node.children().begin();
              child != node.children().end();
              child++) {
            gen(irep, *child);
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
          irep.push_back(Code(OP_SPLIT));
          ssize_t split = irep.size()-1;

          // L1:
          size_t label1 = irep.size();
          gen(irep, node.children()[0]);
          irep.push_back(Code(OP_JMP));
          ssize_t jmp_l3 = irep.size()-1;

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
          irep.push_back(Code(OP_SPLIT));
          ssize_t split = irep.size()-1;

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
      case NODE_QUESTQUEST:
        {
          /**
           * e??
           *
           * L0: SPLIT L2, L1
           * L1: e
           * L2:
           */
          // L0:
          size_t label0 = irep.size();
          irep.push_back(Code(OP_SPLIT));
          ssize_t split = irep.size()-1;

          // L1:
          size_t label1 = irep.size();
          gen(irep, node.children()[0]);

          // L2:
          size_t label2 = irep.size();

          // fix pos.
          irep[split].a(label2 - label0);
          irep[split].b(label1 - label0);
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
          irep.push_back(Code(OP_SPLIT));
          ssize_t split = irep.size()-1;

          // L1:
          size_t label1 = irep.size();
          gen(irep, node.children()[0]);
          irep.push_back(Code(OP_JMP));
          ssize_t jmp = irep.size()-1;

          // L2:
          size_t label2 = irep.size();

          // fix pos.
          irep[split].a(label1 - label0);
          irep[split].b(label2 - label0);
          irep[jmp].a((int)label0 - (int)label2 + 1);
          return;
        }
      case NODE_ASTERQUEST: // *?
        {
          /**
           * e*?
           *
           * L0: SPLIT L2, L1
           * L1: e
           *     jmp L0
           * L2:
           */

          // L0:
          size_t label0 = irep.size();
          irep.push_back(Code(OP_SPLIT));
          ssize_t split = irep.size()-1;

          // L1:
          size_t label1 = irep.size();
          gen(irep, node.children()[0]);
          irep.push_back(Code(OP_JMP));
          ssize_t jmp = irep.size()-1;

          // L2:
          size_t label2 = irep.size();

          // fix pos.
          irep[split].a(label2 - label0);
          irep[split].b(label1 - label0);
          irep[jmp].a((int)label0 - (int)label2 + 1);
          return;
        }
      case NODE_PLUS:
        {
          /**
           * e+
           *
           * L1: e
           *     SPLIT L1, L2
           * L2:
           */
          // L1:
          size_t label1 = irep.size();
          gen(irep, node.children()[0]);
          irep.push_back(Code(OP_SPLIT));
          ssize_t split = irep.size()-1;
          size_t label2 = irep.size();

          irep[split].a(label1 - label2 + 1);
          irep[split].b(1);
          return;
        }
      case NODE_PLUSQUEST:
        {
          /**
           * e+?
           *
           * L1: e
           *     SPLIT L2, L1
           * L2:
           */
          // L1:
          size_t label1 = irep.size();
          gen(irep, node.children()[0]);
          irep.push_back(Code(OP_SPLIT));
          ssize_t split = irep.size()-1;
          size_t label2 = irep.size();

          irep[split].a(1);
          irep[split].b(label1 - label2 + 1);
          return;
        }
      case NODE_ANYCHAR:
        irep.push_back(Code(OP_ANYCHAR));
        return;
      case NODE_LINEHEAD:
        irep.push_back(Code(OP_LINEHEAD));
        return;
      case NODE_LINETAIL:
        irep.push_back(Code(OP_LINETAIL));
        return;
      case NODE_HEAD:
        irep.push_back(Code(OP_HEAD));
        return;
      case NODE_TAIL:
        irep.push_back(Code(OP_TAIL));
        return;
      case NODE_CAPTURE:
        {
          int n = capture_no_;
          capture_no_++;

          irep.push_back(Code(OP_SAVE));
          irep.back().a(n);

          gen(irep, node.children()[0]);

          irep.push_back(Code(OP_MATCH));
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
  irep.push_back(Code(OP_FINISH));
}

