.PHONY: test
test:
	@cc -std=c99 -Wall -Wextra -Werror test.c
	@valgrind ./a.out
	@rm -f a.out
