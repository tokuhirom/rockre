#include "rockre.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>

using namespace RockRE;

namespace RockRE {
  enum MATCHING_STATE {
    COMPILE_ONLY,
    NEED_MATCH,
  };
  enum MATCHING_TYPE {
    FULL_MATCH,
    PARTIAL_MATCH,
  };
};

int main(int argc, char** argv)
{
  if (argc==1) {
    abort();
  }

  MATCHING_STATE matching_state = COMPILE_ONLY;
  MATCHING_TYPE  matching_type  = FULL_MATCH;
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
  Node node;
  std::string errstr;
  if (!RockRE::parse(std::string(regexp), node, errstr)) {
    std::cerr << "Syntax error : " << errstr << std::endl;
    return EXIT_FAILURE;
  }
  assert(node.type() != NODE_UNDEF);
  switch (matching_state) {
  case NEED_MATCH:
    {
      RockRE::Irep irep;
      RockRE::codegen(node, irep);
      if (dump_irep_mode) {
        dump_irep(irep);
      }
      std::map<int, std::string> captured;
      bool ret = matching_type == PARTIAL_MATCH
          ? RockRE::partial_match(std::string(argv[n+1]), irep, captured)
          : RockRE::full_match(std::string(argv[n+1]), irep, captured);
      if (ret) {
        printf("OK\n");
        for (auto kv: captured) {
          std::cout << kv.first << ":" << kv.second << std::endl;
        }
      } else {
        printf("FAIL\n");
      }
    break;
    }
  case COMPILE_ONLY:
    node.dump();
  }
  return EXIT_SUCCESS;
}
