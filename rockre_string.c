#include "rockre.h"
#include <stdlib.h>
#include <string.h>

// Should we copy the string?
rockre_string* rockre_string_new(const char* ptr, size_t len)
{
    rockre_string *p = malloc(sizeof(rockre_string));
    memset(p, 0, sizeof(rockre_string));
    p->ptr = malloc(len);
    if (!p->ptr) {
      free(p);
      return NULL;
    }
    memcpy((void*)p->ptr, (const void*)ptr, len);
    p->len = len;
    return p;
}

void rockre_string_free(rockre_string* str) {
    free(str);
}

const char *rockre_string_cstr(rockre_string* str) {
    return str->ptr;
}

