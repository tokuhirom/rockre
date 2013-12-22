#ifndef ROCKRE_H_
#define ROCKRE_H_

#include <stddef.h>
#include <string>
#include <vector>
#include <map>

namespace RockRE {

  enum OPType {
    OP_HEAD,
    OP_TAIL,
    OP_CAPTURE,
    OP_SPLIT,
    OP_FINISH,
    OP_CHAR,
    OP_JMP,
    OP_ANYCHAR,
    OP_SAVE,
    OP_MATCH,
    OP_LINEHEAD,
    OP_LINETAIL,
  };

  enum NodeType {
    NODE_UNDEF,
    NODE_LIST = 1,
    NODE_ALT,
    NODE_TAIL,
    NODE_HEAD,
    NODE_CHAR,
    NODE_CAPTURE,
    NODE_GROUP,
    NODE_ANYCHAR,
    NODE_LINETAIL,
    NODE_LINEHEAD,
    NODE_QUEST, // ? - one or zero
  };

  class Node {
    enum NodeType type_;
    uint32_t ch_;
    std::vector<Node> children_;
  public:
    Node(const Node &node)
      : type_(node.type_) {
      ch_ = node.ch_;
      children_ = node.children_;
    }
    Node()
      : type_(NODE_UNDEF), ch_(0) { }
    Node(NodeType t)
      : type_(t), ch_(0) { }
    Node(NodeType t, uint32_t ch)
      : type_(t), ch_(ch) { }
    Node(NodeType t, const Node& child)
      : type_(t) {
      children_.push_back(child);
    }
    Node(NodeType t, Node a, Node b)
      : type_(t) {
          children_.push_back(a);
          children_.push_back(b);
      }
    NodeType type() const {
      return type_;
    }
    void type(NodeType type) {
      type_ = type;
    }
    uint32_t ch() const {
      return ch_;
    }
    const std::vector<Node>& children() const {
      return children_;
    }
    void push_child(Node b) {
      children_.push_back(b);
    }
    void dump() const;
    void dump_children(std::string name) const;
  };

  class Code;

  typedef std::vector<Code> Irep;

  class Code {
  public:
    OPType op_;
    int c_;
    uint8_t a_;
    uint8_t b_;
  public:
    Code(OPType op)
      : op_(op), c_(0) { }
    Code(OPType op, const char c)
      : op_(op), c_(c) { }
    OPType op() const { return op_; }
    char c() const { return c_; }
    uint8_t a() const { return a_; }
    uint8_t b() const { return b_; }
    void a(uint8_t _) { a_ = _; }
    void b(uint8_t _) { b_ = _; }
  };

  /* parser api */
  bool parse(const std::string str, Node & node, std::string& errstr);
  void codegen(const Node& node, Irep& irep);

  const char* op_name(OPType t);
  void dump_irep(const Irep& irep);

  bool partial_match(const std::string str, const Irep& irep, std::map<int,std::string> &captured);
  bool full_match(const std::string str, const Irep& irep, std::map<int,std::string> &captured);
};

#endif // ROCKRE_H_
