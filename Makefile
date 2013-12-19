all: test_rockre

test: test_rockre
	prove -v test.t

test_rockre: rockre.y.c rockre_node.c
	$(CC) -o test_rockre -Wall -W -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -DYY_DEBUG=1 -g rockre.y.c rockre_node.c rockre_string.c test.c

rockre.y.c: rockre.y
	greg -o rockre.y.c rockre.y

.PHONY: test all
