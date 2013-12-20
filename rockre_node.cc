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
  case RockRE::NODE_UNDEF:
    std::cout << "(undef)";
    return;
  case RockRE::NODE_STRING:
    std::cout << "(string \"" << string_ << "\")";
    return;
  case RockRE::NODE_TAIL:
    std::cout << "(tail)";
    return;
  case RockRE::NODE_HEAD:
    std::cout << "(head)";
    return;
  case RockRE::NODE_ANYCHAR:
    std::cout << "(anychar)";
    return;
  case RockRE::NODE_OR:
    this->dump_children("or");
    return;
  case RockRE::NODE_LIST:
    this->dump_children("list");
    return;
  case RockRE::NODE_GROUP:
    this->dump_children("group");
    return;
  case RockRE::NODE_CAPTURE:
    this->dump_children("capture");
    return;
  }
  abort();
}


