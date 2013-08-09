GCCFLAGS= -Wall -Wno-comment -Wunused -pedantic 
LINKERFLAGS= -lpthread
DIR_SRC = src
DIR_HDR = include
DIR_OBJ = obj

all:	wifi.bin

obj:	${DIR_SRC}/*.c
	gcc -o ${DIR_OBJ}/*.o -I ${DIR_HDR} -c ${GCCFLAGS} ${DIR_SRC}/*.c

wifi.bin:	obj
	gcc  -o wifi.bin ${LINKERFLAGS} ${DIR_OBJ}/*.o

clean:	
	rm -f ${DIR_OBJ}/*
	rm -f ${DIR_SRC}/*
	rm -f wifi.bin
