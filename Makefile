default: test

.PHONY: clean test

CC ?= gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic
SRCS := src/cutil.c tests/tests.c
HEADERS := src/cutil.h src/typedef.h src/types.h

clean:
	rm -rf target && mkdir -p target

test: ${SRCS} ${HEADERS}
	mkdir -p target
	${CC} ${CFLAGS} -g ${SRCS} -fsanitize=address,undefined -o target/tests && target/tests
