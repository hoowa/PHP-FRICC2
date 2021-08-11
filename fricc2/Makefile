all: fricc2

fricc2: fricc2.c
	gcc -Wall -O2 -o fricc2 fricc2.c ../friccload2/zencode.c -lz -static

debug:
	gcc -Wall -g -O0 -o fricc2 fricc2.c ../friccload2/zencode.c -lz

clean:
	/bin/rm -vf *.o *.so fricc2 test.t

#valgrind  --tool=memcheck  --leak-check=full --track-origins=yes  ./fricc2 -o t /usr/bin/xz
