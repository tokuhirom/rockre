%{

#include "rockre.h"
#include <assert.h>
#include <memory>

/**

aaa|bbb

*/

using namespace RockRE;

#define YYSTYPE RockRE::Node
#define YY_NAME(n) rockre_parser_##n
#define YY_XTYPE RockRE::ParserContext

#define PARSER (&(G->data))

namespace RockRE {
  class ParserInput {
    friend class ParserContext;
    std::string src_;
    size_t pos_;
  public:
    ParserInput(const std::string &s) {
      src_ = s;
      pos_ = 0;
    }
  };

  class ParserContext {
    Node root_;
    ParserInput* input_;
  public:
    void input(ParserInput* input) {
      input_ = input;
    }
    void root(const Node& root) {
      root_ = root;
    }
    Node root() const {
      return root_;
    }
    char yy_input(char* buf) {
      if (input_->src_.length() == input_->pos_) {
        return 0;
      } else {
        *buf = input_->src_[input_->pos_];
        input_->pos_ = input_->pos_+1;
        return 1;
      }
    }
  };
};

#define YY_INPUT(buf, result, max_size, D)		\
    result = D.yy_input(buf);

// TODO: <[abc]>
// TODO: ( a | b )

#define S Node

%}

rockre = - a:pattern - end-of-file { $$ = a; G->data.root(a); }

end-of-file = !'\0'

pattern =
    a:terms - '|' - b:pattern {
      $$ = S(RockRE::NODE_OR, a, b);
    }
    | terms

terms =
    a:term { $$ = a; } (
        - b:term {
            if (a.type() != RockRE::NODE_LIST) {
                a = S(RockRE::NODE_LIST, a);
            }
            a.push_child(b);
        }
    )* { $$ = a; }

term =
    head
    | tail
    | raw
    | '(' - a:pattern - ')' { $$ = S(RockRE::NODE_CAPTURE, a); }
    | '[' - a:pattern - ']' { $$ = S(RockRE::NODE_GROUP, a); }
    | '.' { $$ = S(RockRE::NODE_ANYCHAR); }

raw =
  a:raw_part { $$ = a; } (
    b:raw_part {
      a = S(RockRE::NODE_STRING, a.string() + b.string());
    }
  )* { $$ = a; }

raw_part =
    < [^ .|\\)(\[\]]+ > { $$ = S(RockRE::NODE_STRING, yytext, yyleng); }
    | esc '.' { $$ = S(RockRE::NODE_STRING, ".", 1); }
    | esc 't' { $$ = S(RockRE::NODE_STRING, "\t", 1); }

head = "^^" { $$ = S(RockRE::NODE_HEAD); }
tail = "$$" { $$ = S(RockRE::NODE_TAIL); }

- = ( " " )*
esc = "\\"

%%

RockRE::Node RockRE::parse(const std::string str)
{
    RockRE::Node root;
    GREG g;
    YY_NAME(init)(&g);
    RockRE::ParserInput input(str);
    g.data.input(&input);

    if (!YY_NAME(parse)(&g)) {
        fprintf(stderr, "Syntax error\n");
        goto FINALIZE;
    }
    if (g.limit != g.pos) {
        fprintf(stderr, "Syntax error %d\n", g.limit);
        goto FINALIZE;
    }
    root = g.data.root();

FINALIZE:
    YY_NAME(deinit)(&g);
    return root;
}

