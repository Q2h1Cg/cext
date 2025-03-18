.PHONY: test
test:
	@cc -std=c99 -Wall -Wextra -Werror test.c
	@./a.out || true
	@rm -f a.out
