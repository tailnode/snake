snake:main.c define.h
	gcc -Wall -o snake main.c -lncurses -lpthread
clean:
	rm snake -f
debug:
	gcc -Wall -o snake main.c -lncurses -lpthread -g
