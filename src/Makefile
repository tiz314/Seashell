Seashell: main cli history config
	gcc -o seashell main.o cli.o history.o config.o
main: main.c history cli config
	gcc -Wall -c main.c -g
cli: cli.c
	gcc -Wall -c cli.c -g
history: history.c
	gcc -Wall -c history.c -g
config: config.c
	gcc -Wall -c config.c -g

clean: 
	rm seashell
	rm *.o
