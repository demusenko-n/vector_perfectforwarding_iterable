COMPILER = g++
GFLAGS = -std=c++20
SOURCES = main.cpp my_vector.h my_vector_exception.h
EXEC_FILENAME = prog.exe

all: compile
run: compile
	@./${EXEC_FILENAME}
compile:
	${COMPILER} ${SOURCES} -o ${EXEC_FILENAME} -w -std=c++20
clear:
	rm -f *.o ${EXEC_FILENAME}