#include "rockre.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <alloca.h>

enum MATCHING_STATE {
  COMPILE_ONLY,
  NEED_MATCH,
};
enum MATCHING_TYPE {
  FULL_MATCH,
  PARTIAL_MATCH,
};

int main(int argc, char** argv)
{
  if (argc==1) {
    abort();
  }

  enum MATCHING_STATE matching_state = COMPILE_ONLY;
  enum MATCHING_TYPE  matching_type  = FULL_MATCH;
  bool dump_irep_mode = false;

  int n = 1;
  while (argv[n][0] == '-') {
    if (argv[n][1] == 'f') {
      // full match mode.
      matching_state = NEED_MATCH;
      matching_type  = FULL_MATCH;
      n++;
    } else if (argv[n][1] == 'p') {
      // partial match mode.
      matching_state = NEED_MATCH;
      matching_type  = PARTIAL_MATCH;
      n++;
    } else if (argv[n][1] == 'd') {
      dump_irep_mode = true;
      n++;
    } else {
      fprintf(stderr, "Unknown option: %s\n", argv[n]);
      return EXIT_FAILURE;
    }
  }

  const char*regexp = argv[n];
  switch (matching_state) {
  case NEED_MATCH:
    {
      rockre* rr = rockre_new();
      rockre_regexp* rrr = rockre_compile(rr, regexp, strlen(regexp), false);
      if (!rrr) {
        printf("Syntax error: %s\n", rockre_errstr(rr));
      }
      if (dump_irep_mode) {
        rockre_dump_irep(rr, rrr);
      }
      rockre_region* region = rockre_region_new(rr);
      bool ret;
      char *str = argv[n+1];
      if (matching_type == PARTIAL_MATCH) {
        ret = rockre_partial_match(rr, rrr, region, str, strlen(str));
      } else {
        ret = rockre_full_match(rr, rrr, region, str, strlen(str));
      }
      if (ret) {
        printf("OK\n");
        for (int i=0; i<region->num_regs; ++i) {
          // printf("%d %d\n", region->beg[i], region->end[i]);
          size_t len = region->end[i] - region->beg[i];
          char *s = (char*)alloca(sizeof(char) * len + 1);
          strncpy(s, str + region->beg[i], len);
          s[len] = '\0';
          printf("%d:%s\n", i, s);
        }
      } else {
        printf("FAIL\n");
      }
      rockre_region_destroy(rr, region);
      rockre_destroy(rr);
      break;
    }
  case COMPILE_ONLY:
    {
      rockre* r = rockre_new();
      if (!rockre_dump_node(r, regexp, strlen(regexp))) {
        printf("Syntax error: %s\n", rockre_errstr(r));
      }
      rockre_destroy(r);
      break;
    }
  }
  return EXIT_SUCCESS;
}
