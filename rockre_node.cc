#include "rockre.h"
#include <iostream>

void RockRE::Node::dump_children(const std::string name) const
{
  std::cout << "(" << name << " ";
  size_t i = children_.size();
  for (auto c: children_) {
    c.dump();
    if (i-- != 1) {
      std::cout << " ";
    }
  }
  std::cout << ")";
}

void RockRE::Node::dump() const
{
  switch (type_) {
  case NODE_UNDEF:
    std::cout << "(undef)";
    return;
  case NODE_STRING:
    std::cout << "(string \"" << string_ << "\")";
    return;
  case NODE_TAIL:
    std::cout << "(tail)";
    return;
  case NODE_HEAD:
    std::cout << "(head)";
    return;
  case NODE_LINETAIL:
    std::cout << "(linetail)";
    return;
  case NODE_LINEHEAD:
    std::cout << "(linehead)";
    return;
  case NODE_ANYCHAR:
    std::cout << "(anychar)";
    return;
  case NODE_ALT:
    this->dump_children("or");
    return;
  case NODE_LIST:
    this->dump_children("list");
    return;
  case NODE_GROUP:
    this->dump_children("group");
    return;
  case NODE_CAPTURE:
    this->dump_children("capture");
    return;
  }
  abort();
}


