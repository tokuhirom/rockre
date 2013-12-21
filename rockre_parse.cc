#include "rockre.h"

#define TEST(test,node) \
  if (test(node)) { \
    return true; \
  }

#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

// #define YY_DEBUG

#ifdef YY_DEBUG
#  define STATUS printf("[parser status] %s:%d %zu\n", __func__, __LINE__, sp_)
#  define PARSE_OK printf("[parser done] %s:%d %zu\n", __func__, __LINE__, sp_)
#else
#  define STATUS
#  define PARSE_OK
#endif

namespace RockRE {
  class Parser {
    std::string src_;
    size_t sp_; // string pos
    std::string errstr_;

  public:
    const std::string errstr() const {
      return errstr_;
    }

    // compunit = - a:pattern - end-of-file { printf("AHH\n"); $$ = a; G->data.root(a); }
    bool parse(const std::string src, Node& node) {
      src_ = src;
      sp_  = 0;

      skip_sp();
      STATUS;
      bool ok = parse_compunit(node);
      STATUS;
      skip_sp();

      if (ok && sp_ == src_.length()) {
        return true;
      } else {
        if (errstr_.size() == 0) {
          errstr_ = "Can't parse.";
        }
        return false;
      }
    }

    bool parse_empty(Node& node) {
      STATUS;
      if (sp_+1 == src_.length()) {
        PARSE_OK;
        return true;
      } else {
        return false;
      }
    }

    bool parse_compunit(Node& node) {
      skip_sp();
      TEST(parse_pattern, node);
      TEST(parse_empty, node);
      return false;
    }

    // pattern = terms - | - pattern
    // pattern = terms
    bool parse_pattern(Node& node) {
      STATUS;

      Node a;
      if (parse_terms(a)) {
        skip_sp();
        if (src_[sp_] == '|') {
          sp_++;
          skip_sp();
          Node b;
          if (parse_pattern(b)) {
            node = Node(NODE_ALT, a, b);
            PARSE_OK;
            return true;
          } else {
            errstr_ = "Missing pattern after |";
            return false;
          }
        } else {
          // Just terms.
          node = a;
          PARSE_OK;
          return true;
        }
      } else {
        return false;
      }
    }

    // terms = term term*
    bool parse_terms(Node& node) {
      Node a;
      if (parse_term(a)) {
        STATUS;
        Node b;
        while (parse_term(b)) {
          STATUS;
          if (a.type() != NODE_LIST) {
            a = Node(NODE_LIST, a);
          }
          a.push_child(b);
        }
        node = a;
        return true;
      } else {
        return false;
      }
    }

    bool parse_term(Node& node) {
      skip_sp();
      STATUS;

      TEST(parse_head,    node);
      TEST(parse_tail,    node);
      STATUS;
      TEST(parse_raw,     node);
      STATUS;
      TEST(parse_capture, node);
      TEST(parse_group,   node);
      TEST(parse_anychar, node);
      return false;
    }

    // skip spaces.
    void skip_sp() {
      while (src_[sp_] == ' ') {
        sp_++;
      }
    }

    bool parse_paren(Node& node, const char lparen, const char rparen, NodeType type) {
      if (EXPECT(1, std::string(&lparen, 1))) {
        sp_++;

        Node body;
        parse_pattern(body);
        skip_sp();
        if (EXPECT(1, std::string(&rparen, 1))) {
          node = Node(type, body);
          sp_++;
          PARSE_OK;
          return true;
        } else {
          errstr_ = std::string("Unexpected end-of-string before ") + rparen;
          return false;
        }
      } else {
        return false;
      }
    }

    // group = [ pattern ]
    bool parse_group(Node& node) {
      STATUS;
      return parse_paren(node, '[', ']', NODE_GROUP);
    }

    // capture = ( pattern )
    bool parse_capture(Node& node) {
      STATUS;
      return parse_paren(node, '(', ')', NODE_CAPTURE);
    }

    // ( \t | \. | [^.|)([]?.]_ )
    bool parse_raw(Node& node)
    {
      std::string buf;
      while (src_.length() != sp_) {
        switch (src_[sp_]) {
        case '\\':
          switch (src_[sp_+1]) {
          case 't':
            buf += "\t";
            sp_++; sp_++;
            break;
          default:
            buf += src_[sp_+1];
            sp_++; sp_++;
            break;
          }
          break;
        case '|':
        case ')':
        case '(':
        case '[':
        case ']':
        case '?':
        case '.':
        case '^':
        case '$':
        case ' ':
          goto DONE;
        default:
          buf += src_[sp_];
          sp_++;
          break;
        }
      }
      STATUS;
      DONE:
      if (buf.size() > 0) {
        node.type(NODE_STRING);
        node.string(buf);
        return true;
      } else {
        return false;
      }
    }

    // .
    bool parse_anychar(Node& node)
    {
      if (EXPECT(1, ".")) {
        node.type(NODE_ANYCHAR);
        sp_++;
        return true;
      } else {
        return false;
      }
    }

    // $$
    bool parse_tail(Node& node)
    {
      if (EXPECT(2, "$$")) {
        node.type(RockRE::NODE_TAIL);
        sp_+=2;
        return true;
      } else {
        return false;
      }
    }

    bool EXPECT(size_t n, const std::string s) {
      if (src_.length() - sp_ < n) {
#ifdef YY_DEBUG
        printf("[EXPECT] skip: sp:%zu len: %lu, n:%zu, %s\n", sp_, src_.length(), n, s.c_str());
#endif
        return false;
      } else {
#ifdef YY_DEBUG
        printf("[EXPECT] sp:%zu %s\n", sp_, s.c_str());
#endif
        return src_.substr(sp_, n) == s;
      }
    }

    // ^^
    bool parse_head(Node& node)
    {
      STATUS;
      if (EXPECT(2, "^^")) {
        PARSE_OK;
        node.type(RockRE::NODE_HEAD);
        sp_+=2;
        return true;
      } else {
        return false;
      }
    }
  };
};

bool RockRE::parse(const std::string str, Node & node, std::string &errstr)
{
  Parser parser;
  bool retval = parser.parse(str, node);
  errstr = parser.errstr();
  return retval;
}
