snake:main.o inputThread.o
	gcc -std=gnu99 -Wall -o snake main.o inputThread.o -lncurses -lpthread
inputThread.o:inputThread.c define.h
	gcc -std=gnu99 -Wall -c inputThread.c
main.o:main.c define.h
	gcc -std=gnu99 -Wall -c main.c
clean:
	rm snake inputThread.o main.o -f
#debug:main.c define.h
#	gcc -std=gnu99 -Wall -o snake main.c -lncurses -lpthread -g
