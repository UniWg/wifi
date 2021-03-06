GCCFLAGS= -Wall -pedantic 
LINKERFLAGS= -pthread
DIR_SRC = src
DIR_HDR = include
DIR_OBJ = obj

all:	wifi.bin

# Compilazione dei moduli (creazione file oggetto)

applica: ${DIR_SRC}/applica.c
	gcc -o ${DIR_OBJ}/applica.o -I ${DIR_HDR} -c ${GCCFLAGS} ${DIR_SRC}/applica.c

applica_sub: ${DIR_SRC}/applica_sub.c
	gcc -o ${DIR_OBJ}/applica_sub.o -I ${DIR_HDR} -c ${GCCFLAGS} ${DIR_SRC}/applica_sub.c

main:	${DIR_SRC}/main.c
	gcc -o ${DIR_OBJ}/main.o -I ${DIR_HDR} -c ${GCCFLAGS} ${DIR_SRC}/main.c

mezzo:	${DIR_SRC}/mezzo.c
	gcc -o ${DIR_OBJ}/mezzo.o -I ${DIR_HDR} -c ${GCCFLAGS} ${DIR_SRC}/mezzo.c
	
mezzo_sub:	${DIR_SRC}/mezzo_sub.c
	gcc -o ${DIR_OBJ}/mezzo_sub.o -I ${DIR_HDR} -c ${GCCFLAGS} ${DIR_SRC}/mezzo_sub.c

pseudo_frame: ${DIR_SRC}/pseudo_frame.c
	gcc -o ${DIR_OBJ}/pseudo_frame.o -I ${DIR_HDR} -c ${GCCFLAGS} ${DIR_SRC}/pseudo_frame.c

stazione: ${DIR_SRC}/stazione.c
	gcc -o ${DIR_OBJ}/stazione.o -I ${DIR_HDR} -c ${GCCFLAGS} ${DIR_SRC}/stazione.c
	
stazione_sub: ${DIR_SRC}/stazione_sub.c
	gcc -o ${DIR_OBJ}/stazione_sub.o -I ${DIR_HDR} -c ${GCCFLAGS} ${DIR_SRC}/stazione_sub.c
	
utils: ${DIR_SRC}/utils.c
	gcc -o ${DIR_OBJ}/utils.o -I ${DIR_HDR} -c ${GCCFLAGS} ${DIR_SRC}/utils.c

# Linker

wifi.bin:	applica applica_sub main mezzo mezzo_sub pseudo_frame stazione stazione_sub utils
	gcc -o wifi.bin ${LINKERFLAGS} ${DIR_OBJ}/*.o

# Cancellazione dei file di compilazione

clean:	
	rm -f ${DIR_OBJ}/*
	rm -f wifi.bin
	
	
	
