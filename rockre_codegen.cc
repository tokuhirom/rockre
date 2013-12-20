#include "rockre.h"

using namespace RockRE;

static std::shared_ptr<Irep> gen(std::shared_ptr<Node> node)
{
  switch (node->type()) {
  case NODE_STRING:
    {
      std::shared_ptr<Irep> irep = std::make_shared<Irep>();
      for (const char& c: node->string()) {
        irep->push(OP_CHAR, c);
      }
      return irep;
    }
  default:
    abort();
  }
}

std::shared_ptr<Irep> RockRE::codegen(const std::shared_ptr<RockRE::Node> node)
{
  std::shared_ptr<RockRE::Irep> irep = gen(node);
  irep->push(OP_FINISH);
  return irep;
}

