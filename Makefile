.PHONY: day run check

CFLAGS := -Wall -Wpedantic --std=c11

day:
ifndef DAY
	$(error DAY is undefined)
endif
ifndef FILE
	$(error FILE is undefined)
endif
	gcc ${CFLAGS} main.c days/day${DAY}.c -o main \
		-DINFILE="${FILE}" -DDAY=${DAY}

run: day
	./main

check: day
	valgrind -s --leak-check=full ./main
