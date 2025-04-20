EXE=translate

$(EXE): main.c
	cc -O3 -Wall -o $(EXE) $<
clean:
	rm -f *.o
	rm -f translate
format:
	clang-format -style=file -i *.c
