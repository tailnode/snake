snake:main.c
	gcc -o snake main.c -lncurses -lpthread
clean:
	rm snake -f
debug:
	gcc -o snake main.c -lncurses -lpthread -g
