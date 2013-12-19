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
#define PUSH_CHILD(a,b) rockre_node_push_child(a,b)


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

%}

rockre = a:terms end-of-file { $$ = a; G->data.root=a; }

end-of-file = !'\0'

pattern =
    a:terms (
        '|' b:terms { $$ = NODE(ROCKRE_NODE_OR); }
    )*

terms =
    a:term { $$ = a; } (
        b:term {
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

raw = < [a-zA-Z0-9]+ > { $$ = STRING(ROCKRE_NODE_STRING, yytext, yyleng); }

head = "^^" { $$ = NODE(ROCKRE_NODE_HEAD); }

tail = "$$" { $$ = NODE(ROCKRE_NODE_TAIL); }

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

