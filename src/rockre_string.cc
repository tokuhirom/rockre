#include "rockre_private.h"
#include <stdlib.h>
#include <string.h>

static rockre_string* string_new(size_t len)
{
  rockre_string *p = malloc(sizeof(rockre_string));
  if (!p) {
    return NULL;
  }

  memset(p, 0, sizeof(rockre_string));
  p->ptr = malloc(len);
  if (!p->ptr) {
    free(p);
    return NULL;
  }
  p->len = len;
  return p;
}

// Should we copy the string?
rockre_string* rockre_string_new(const char* ptr, size_t len)
{
  rockre_string* p = string_new(len);
  if (!p) {
    return NULL;
  }

  memcpy((void*)p->ptr, (const void*)ptr, len);
  return p;
}

void rockre_string_free(rockre_string* str) {
    free(str);
}

const char *rockre_string_cstr(rockre_string* str) {
    return str->ptr;
}

rockre_string* rockre_string_plus(rockre_string* str1, rockre_string* str2) {
  rockre_string*p = string_new(str1->len + str2->len);
  if (!p) {
    return NULL;
  }

  memcpy(p->ptr, str1->ptr, str1->len);
  memcpy(p->ptr + str1->len, str2->ptr, str2->len);
  return p;
}

