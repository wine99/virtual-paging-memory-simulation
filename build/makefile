ifeq ($(OS),Windows_NT)
	RM = del
else
	RM = rm
endif

memory: memory.o util.o
	gcc -g -o memory memory.o util.o

memory.o: ../memory.c ../memory.h
	gcc -g -c ../memory.c

util.o: ../util.c ../util.h
	gcc -g -c ../util.c

clean:
	del *.exe del *.o
