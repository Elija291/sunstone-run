CFLAGS ?= -O2 -Wall -Wextra -std=c11
sunstone-run: main.c game.c view.c runner.h
	$(CC) $(CFLAGS) main.c game.c view.c -o $@ $(shell sdl2-config --cflags --libs) -lm
play: sunstone-run
	./sunstone-run
test: game.c view.c test.c runner.h
	$(CC) $(CFLAGS) game.c view.c test.c -o runner-test -lm
	./runner-test
clean:
	rm -f sunstone-run runner-test
.PHONY: play test clean
