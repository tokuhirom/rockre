all: test_rockre

test: test_rockre
	prove -v test.t

test_rockre: parse.y.c rockre_node.c rockre_string.c test.c rockre_vm.c rockre_codegen.c
	$(CC) -Werror -std=c99 -o test_rockre -Wall -W -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -DYY_DEBUG=1 -g parse.y.c rockre_node.c rockre_string.c rockre_vm.c rockre_codegen.c test.c

parse.y.c: parse.y
	greg -o parse.y.c parse.y

.PHONY: test all
