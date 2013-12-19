%{

#include "rockre.h"
#include <assert.h>

/**

aaa|bbb

*/

#define YYSTYPE rockre_node*
#define YY_NAME(n) rockre_parser_##n
#define YY_XTYPE rockre_parser_context

#define PARSER (&(G->data))
#define NODE(t)   rockre_node_new(t)
#define NODE1(t,a)   rockre_node_new1(t,a)
#define STRING(t,s,l)   rockre_node_new_string(t, s,l)
#define STRING_APPEND(t,o)   rockre_node_string_append(t,o)
#define PUSH_CHILD(a,b) rockre_node_push_child(a,b)

// TODO: longjmp
#define RESQUE(x) \
    assert(x)


typedef struct rockre_parser_input {
    const char* src;
    size_t len;
    size_t pos;
} rockre_parser_input;

typedef struct rockre_parser_context {
    rockre_node *root;
    rockre_parser_input* input;
} rockre_parser_context;

static inline char rockre_input(char *buf, YY_XTYPE D) {
    if (D.input->len == D.input->pos) {
        return 0;
    } else {
        *buf = D.input->src[D.input->pos];
        D.input->pos = D.input->pos+1;
        return 1;
    }
}

#define YY_INPUT(buf, result, max_size, D)		\
    result = rockre_input(buf, D);

// TODO: <[abc]>
// TODO: ( a | b )

%}

rockre = - a:pattern - end-of-file { $$ = a; G->data.root=a; }

end-of-file = !'\0'

pattern =
    a:terms { $$ = a; } (
        - '|' - b:terms {
            if (a->type != ROCKRE_NODE_OR) {
                a = NODE1(ROCKRE_NODE_OR, a);
            }
            PUSH_CHILD(a, b);
        }
    )* { $$ = a; }

terms =
    a:term { $$ = a; } (
        - b:term {
            if (a->type != ROCKRE_NODE_LIST) {
                a = NODE1(ROCKRE_NODE_LIST, a);
            }
            PUSH_CHILD(a, b);
        }
    )* { $$ = a; }

term =
    head
    | tail
    | raw
    | '(' - a:pattern - ')' { $$ = NODE1(ROCKRE_NODE_CAPTURE, a); }
    | '[' - a:pattern - ']' { $$ = NODE1(ROCKRE_NODE_GROUP, a); }
    | '.' { $$ = NODE(ROCKRE_NODE_ANYCHAR); }

raw =
    a:raw_part { $$ = a; } (
        b:raw_part {
            rockre_node*p = rockre_node_string_plus(a, b);
            RESQUE(p);
            rockre_node_free(a);
            rockre_node_free(b);
            a = p;
        }
    )* { $$ = a; }

raw_part =
    < [^ .|\\)(\[\]]+ > { $$ = STRING(ROCKRE_NODE_STRING, yytext, yyleng); }
    | esc '.' { $$ = STRING(ROCKRE_NODE_STRING, ".", 1); }
    | esc 't' { $$ = STRING(ROCKRE_NODE_STRING, "\t", 1); }

head = "^^" { $$ = NODE(ROCKRE_NODE_HEAD); }

tail = "$$" { $$ = NODE(ROCKRE_NODE_TAIL); }

- = ( " " )*
esc = "\\"

%%

rockre_node *rockre_parse(const char *str, size_t len)
{
    rockre_node *root = NULL;
    GREG g;
    YY_NAME(init)(&g);
    g.data.input = malloc(sizeof(rockre_input));
    if (!g.data.input) {
        return NULL; // allocation failed.
    }
    g.data.input->src = str;
    g.data.input->len = len;
    g.data.input->pos = 0;

    if (!YY_NAME(parse)(&g)) {
        fprintf(stderr, "Syntax error\n");
        goto FINALIZE;
    }
    if (g.limit != g.pos) {
        fprintf(stderr, "Syntax error %d\n", g.limit);
        goto FINALIZE;
    }
    root = g.data.root;

FINALIZE:
    free(g.data.input);
    YY_NAME(deinit)(&g);
    return root;
}

