#include "rockre_private.h"
#include <iostream>
#include <stdlib.h>
#include "nanoutf8.h"

void RockRE::Node::dump_children(const std::string name) const
{
  std::cout << "(" << name << " ";
  size_t i = children_.size();
  for (std::vector<Node>::const_iterator c=children_.begin(); c != children_.end(); c++) {
    c->dump();
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
  case NODE_CHAR:
    {
      char buf[4];
      size_t len = nanoutf8_encode(ch_, buf);
      if (len) {
        std::cout << "(char '" << std::string(buf, len) << "')";
      } else {
        std::cout << "(char invalid-char)";
      }
      return;
    }
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
    this->dump_children("alt");
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
  case NODE_QUEST: // ?
    this->dump_children("quest");
    return;
  case NODE_QUESTQUEST: // ??
    this->dump_children("??");
    return;
  case NODE_ASTER: // *
    this->dump_children("*");
    return;
  case NODE_ASTERQUEST: // *?
    this->dump_children("*?");
    return;
  case NODE_PLUS: // +
    this->dump_children("+");
    return;
  case NODE_PLUSQUEST: // +?
    this->dump_children("+?");
    return;
  }
  abort();
}


