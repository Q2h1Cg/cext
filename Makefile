.PHONY: test
test:
	@cc -std=c99 -Wall -Wextra -Werror -pthread test.c
	@valgrind ./a.out
	@rm -f a.out
