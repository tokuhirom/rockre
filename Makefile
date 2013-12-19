all: test_rockre

test: test_rockre
	prove -v test.t

test_rockre: parse.y.c rockre_node.c rockre_string.c test.c
	$(CC) -o test_rockre -Wall -W -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -DYY_DEBUG=1 -g parse.y.c rockre_node.c rockre_string.c test.c

parse.y.c: parse.y
	greg -o parse.y.c parse.y

.PHONY: test all
