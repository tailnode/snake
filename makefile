snake:main.c
	gcc -o snake main.c -lncurses
clean:
	rm snake -f
debug:
	gcc -o snake main.c -lncurses -g
