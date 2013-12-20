#include "rockre.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>

using namespace RockRE;

int main(int argc, char** argv)
{
  if (argc==1) {
    abort();
  }

  bool run_mode = false;
  bool dump_irep_mode = false;

  int n = 1;
  while (argv[n][0] == '-') {
    if (argv[n][1] == 'r') {
      run_mode = true;
      n++;
    } else if (argv[n][1] == 'd') {
      dump_irep_mode = true;
      n++;
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
  if (run_mode) {
    RockRE::Irep irep;
    RockRE::codegen(node, irep);
    if (dump_irep_mode) {
      dump_irep(irep);
    }
    bool ret = RockRE::match(std::string(argv[n+1]), irep);
    if (ret) {
      printf("OK\n");
    } else {
      printf("FAIL\n");
    }
  } else {
    node.dump();
  }
  return EXIT_SUCCESS;
}
