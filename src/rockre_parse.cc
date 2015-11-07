#include "rockre_private.h"
#include "nanoutf8.h"
#include <assert.h>

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
      (void) node;
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

    // pattern = terms - || - pattern
    // pattern = terms
    bool parse_pattern(Node& node) {
      STATUS;

      Node a;
      if (parse_terms(a)) {
        skip_sp();
        if (EXPECT("||")) {
          skip_sp();
          Node b;
          if (parse_pattern(b)) {
            node = Node(NODE_ALT, a, b);
            PARSE_OK;
            return true;
          } else {
            errstr_ = "Missing pattern after ||";
            return false;
          }
        } else if (EXPECT("|")) {
          errstr_ = "Longest match operator '|' is not supported yet.";
          return false;
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

    // terms = postfix postfix*
    bool parse_terms(Node& node) {
      Node a;
      if (parse_postfix(a)) {
        STATUS;
        Node b;
        while (parse_postfix(b)) {
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

    // postfix =
    //     term
    //   | term '?'
    //   | term '*'
    //   | term '*?'
    //   | term '+'
    bool parse_postfix(Node& node) {
      if (parse_term(node)) {
        skip_sp();
        if (rest() > 0) {
          if (EXPECT("??")) {
            node = Node(NODE_QUESTQUEST, node);
            return true;
          } else if (EXPECT("?")) {
            node = Node(NODE_QUEST, node);
            return true;
          } else if (EXPECT("*?")) {
            node = Node(NODE_ASTERQUEST, node);
            return true;
          } else if (EXPECT("*")) {
            node = Node(NODE_ASTER, node);
            return true;
          } else if (EXPECT("+?")) {
            node = Node(NODE_PLUSQUEST, node);
            return true;
          } else if (EXPECT("+")) {
            node = Node(NODE_PLUS, node);
            return true;
          }
        }
        return true; // just term.
      } else {
        return false;
      }
    }

    bool parse_term(Node& node) {
      skip_sp();
      STATUS;

      if (rest() == 0) {
        return false;
      }

      TEST(parse_linehead, node);
      TEST(parse_linetail, node);
      TEST(parse_head,     node);
      TEST(parse_tail,     node);
      TEST(parse_quote,    node);
      STATUS;
      STATUS;
      TEST(parse_char_class,node);
      TEST(parse_capture,  node);
      TEST(parse_group,    node);
      TEST(parse_anychar,  node);
      TEST(parse_raw,      node);
      return false;
    }

    // skip spaces.
    void skip_sp() {
      while (src_[sp_] == ' ') {
        sp_++;
      }
    }

    bool parse_paren(Node& node, const char lparen, const char rparen, NodeType type) {
      if (EXPECT(std::string(&lparen, 1))) {
        Node body;
        parse_pattern(body);
        skip_sp();
        if (EXPECT(std::string(&rparen, 1))) {
          node = Node(type, body);
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

    // char_class = <[ chars ]>
    bool parse_char_class(Node& node) {
      STATUS;
      if (EXPECT("<[")) {
        skip_sp();
        node = Node(NODE_ALT);
        while (rest() > 0) {
          skip_sp();
          if (EXPECT("]>")) {
            PARSE_OK;
            return true;
          } else if (EXPECT("\\")) {
            if (rest() == 0) {
              errstr_ = "Unexpected end-of-string in character class";
              return false;
            }
            node.push_child(Node(NODE_CHAR, src_[sp_]));
            sp_++;
          } else {
            char ch = src_[sp_];
            sp_++;
            node.push_child(Node(NODE_CHAR, ch));
          }
        }
        return false;
      } else {
        return false;
      }
    }

    bool parse_quote_body(Node& node, std::string close_char) {
      STATUS;
      node.type(NODE_LIST);

      while (src_.length() != sp_) {
        if (EXPECT(close_char)) {
          return true;
        } else if (EXPECT("\\")) {
          if (rest() == 0) {
            goto FAIL;
          }

          switch (src_[sp_]) {
          case 't':
            node.push_child(Node(NODE_CHAR, '\t'));
            sp_++;
            break;
          case 'n':
            node.push_child(Node(NODE_CHAR, '\n'));
            sp_++;
            break;
          default:
            node.push_child(Node(NODE_CHAR, src_[sp_]));
            sp_++;
            break;
          }
        } else {
          char ch = src_[sp_];
          sp_++;

          node.push_child(Node(NODE_CHAR, ch));
        }
      }
      FAIL:
      errstr_ = "Unexpected end-of-string in quoted string";
      return false;
    }

    // quote = ' quoted '
    // quote = " quoted "
    bool parse_quote(Node& node) {
      STATUS;
      if (EXPECT("\"")) {
        return parse_quote_body(node, "\"");
      } else if (EXPECT("'")) {
        return parse_quote_body(node, "\'");
      } else {
        return false;
      }
    }

    // ( \t | \. | [^.|)([]?.]_ )
    bool parse_raw(Node& node)
    {
      switch (src_[sp_]) {
      case '\\':
        switch (src_[sp_+1]) {
        case 't':
          node = Node(NODE_CHAR, '\t');
          sp_++; sp_++;
          return true;
        default:
          node = Node(NODE_CHAR, src_[sp_+1]);
          sp_++; sp_++;
          return true;
        }
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
        return false;
      default:
        {
          size_t len;
          bool ok;
          uint32_t ch = nanoutf8_peek_char(src_.c_str() + sp_, src_.length() - sp_, &len, &ok);
          if (!ok) {
            errstr_ = "Invalid utf-8 sequence";
            return false;
          }
          node = Node(NODE_CHAR, ch);
          sp_ += len;
          return true;
        }
      }
    }

    // .
    bool parse_anychar(Node& node)
    {
      if (EXPECT(".")) {
        node.type(NODE_ANYCHAR);
        return true;
      } else {
        return false;
      }
    }

    // $$
    bool parse_linetail(Node& node)
    {
      if (EXPECT("$$")) {
        node.type(RockRE::NODE_LINETAIL);
        return true;
      } else {
        return false;
      }
    }

    // $
    bool parse_tail(Node& node)
    {
      if (EXPECT("$")) {
        node.type(RockRE::NODE_TAIL);
        return true;
      } else {
        return false;
      }
    }

    // ^
    bool parse_head(Node& node)
    {
      STATUS;
      if (EXPECT("^")) {
        PARSE_OK;
        node.type(RockRE::NODE_HEAD);
        return true;
      } else {
        return false;
      }
    }

    // ^^
    bool parse_linehead(Node& node)
    {
      STATUS;
      if (EXPECT("^^")) {
        PARSE_OK;
        node.type(RockRE::NODE_LINEHEAD);
        return true;
      } else {
        return false;
      }
    }

    int rest() {
      return (int)src_.length() - (int)sp_;
    }

    bool EXPECT(const std::string s) {
      int n = s.length();
      assert((long)src_.length() - (long)sp_ >= 0);
      if (rest() >= n) {
#ifdef VM_DEBUG
        printf("[EXPECT] sp:%zu %s\n", sp_, s.c_str());
#endif
        if (src_.substr(sp_, n) == s) {
          sp_ += n;
          return true;
        } else {
          return false;
        }
      } else {
#ifdef VM_DEBUG
        printf("[EXPECT] skip: sp:%zu len: %lu, n:%zu, %s\n", sp_, src_.length(), n, s.c_str());
#endif
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

