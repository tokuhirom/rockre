#include "rockre.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char** argv)
{
  if (argc==1) {
    abort();
  }

  bool run_mode = false;

  int n = 1;
  if (argv[n][0] == '-') {
    if (argv[n][1] == 'r') {
      run_mode = true;
      n++;
    }
  }

  const char*regexp = argv[n];
  rockre_node* node = rockre_parse(regexp, strlen(regexp));
  assert(node);
  if (run_mode) {
    rockre_irep* irep = rockre_codegen(node);
    bool ret = rockre_vm_run(argv[n+1], strlen(argv[n+1]), irep);
    rockre_irep_free(irep);
    if (ret) {
      printf("OK\n");
    } else {
      printf("FAIL\n");
    }
  } else {
    rockre_node_dump(node, 0);
  }
  return EXIT_SUCCESS;
}
