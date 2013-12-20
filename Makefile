all: test_rockre

test: test_rockre
	prove -v test.t

test_rockre: parse.y.cc rockre_string.cc test.cc rockre_vm.cc rockre_codegen.cc rockre.h
	$(CXX) -Werror -std=c++11 -o test_rockre -Wall -W -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -DYY_DEBUG=1 -g parse.y.cc rockre_node.cc test.cc rockre_codegen.cc rockre_vm.cc
	# $(CXX) -Werror -std=c++11 -o test_rockre -Wall -W -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -DYY_DEBUG=1 -g parse.y.cc rockre_node.cc rockre_string.cc rockre_vm.cc rockre_codegen.cc test.cc

parse.y.cc: parse.y
	greg -o parse.y.cc parse.y

.PHONY: test all
