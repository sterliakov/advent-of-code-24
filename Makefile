.PHONY: day run check

CFLAGS := -Wall -Wextra -Wstrict-prototypes -pedantic --std=c17 -O3

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

time: CFLAGS += -DTIMING=1
time: run

check: day
	valgrind -s --leak-check=full --track-origins=yes ./main
