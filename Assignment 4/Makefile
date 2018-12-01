# build an executable named myprog from myprog.c
all: q1 q2 program1  program2 program3

q1: q1.c
	gcc -g -Wall -o q1 q1.c -lpthread
	
q2: q2.c
	gcc -g -Wall -o q2 q2.c -lpthread

program1: q3_threaded.c
	gcc -g -Wall -w -o q3_threaded q3_threaded.c -lpthread

program2: q3_normal.c
	gcc -g -Wall -w -o q3_normal q3_normal.c

program3: q3_forked.c
	gcc -g -Wall -w -o q3_forked q3_forked.c

#clean: 
#	$(RM) q3_threaded q3_normal q3_forked

