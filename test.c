#include "rockre.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc==2) {
        const char*regexp = argv[1];
        rockre_node* node = rockre_parse(regexp, strlen(regexp));
        assert(node);
        rockre_node_dump(node, 0);
        return EXIT_SUCCESS;
    }
    abort();
}
