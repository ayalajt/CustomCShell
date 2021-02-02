HEADERS = csapp.h

default: shellex

shellex: shellex.c $(HEADERS)
	gcc -pthread csapp.c shellex.c -o my257sh
all: shellex.c $(HEADERS)
	gcc -pthread csapp.c shellex.c -o my257sh
clean:
	-rm -f my257sh
