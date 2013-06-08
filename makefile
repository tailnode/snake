snake:main.c define.h
	gcc -std=gnu99 -Wall -o snake main.c -lncurses -lpthread
clean:
	rm snake -f
debug:main.c define.h
	gcc -std=gnu99 -Wall -o snake main.c -lncurses -lpthread -g
