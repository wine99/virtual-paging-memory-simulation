main: main.o util.o libmemory.a
	gcc -o main main.o util.o -static libmemory.a
	rm *.o

libmemory.a: ../src/memory/memory.c ../src/memory/memory.h
	gcc -c ../src/memory/memory.c
	ar -rcs libmemory.a memory.o
	rm memory.o

main.o: ../src/main.c ../src/main.h ../src/memory_api.h
	gcc -c ../src/main.c

util.o: ../src/util/util.c ../src/util/util.h
	gcc -c ../src/util/util.c

clean:
	rm main libmemory.a *.o
