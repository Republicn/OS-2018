all:
	#Creates static lib
	gcc -c f1.c f2.c
	ar rc libstatic.a f1.o f2.o
	ranlib libstatic.a
	#Create dynamic lib
	gcc -fPIC -c f3.c
	gcc -fPIC -c f4.c
	gcc -shared -o libdynamic.so f3.o f4.o
	#Create dymanic lib 2
	gcc -fPIC -c f5.c
	gcc -fPIC -c f6.c
	gcc -shared -o libdynamic2.so f5.o f6.o
	#Link main with libs
	gcc -c main.c -Wall
	gcc main.o -o main -L. -lstatic -ldynamic -ldl
	#Clean *.o files
	rm -f *.o

run:
	LD_LIBRARY_PATH=. ./main

clean:
	rm -f *.o *.so *.a main