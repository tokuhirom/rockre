#ifndef ROCKRE_H_
#define ROCKRE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct rockre;
typedef struct rockre rockre;

struct rockre_regexp;
typedef struct rockre_regexp rockre_regexp;

typedef struct rockre_region {
  int num_regs;
  int *beg;
  int *end;
} rockre_region;

rockre* rockre_new();
void rockre_destroy(rockre*);
rockre_regexp* rockre_compile(rockre*, const char*regexp, size_t regexp_len);
const char* rockre_errstr(rockre*);
rockre_region* rockre_region_new(rockre*);
void rockre_region_destroy(rockre*, rockre_region*);
bool rockre_partial_match(rockre*, rockre_regexp*, rockre_region*, const char* str, size_t str_len);
bool rockre_full_match(rockre*, rockre_regexp*, rockre_region*, const char* str, size_t str_len);
bool rockre_dump_node(rockre* r, const char* regexp, size_t regexp_len);
void rockre_dump_irep(rockre* r, rockre_regexp* rr);

#ifdef __cplusplus
};
#endif

#endif // ROCKRE_H_
