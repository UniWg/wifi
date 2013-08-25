#ifndef _MEZZO_H
#define _MEZZO_H

/* ------------------------------------------------------------------------- */
/* --- Costanti --- */
#define _max_connection 10	/* Numero massimo di connessioni contemporanee */
#define _Cmezzo _CCyan		/* Colore dei messaggi del mezzo condiviso */

#define _mac_mezzo "A1:B2:C3:D4:E5:F6"

/* ------------------------------------------------------------------------- */
/* --- Variabili globali --- */

pthread_t mc_thread_g;	/* identificativo del thread del mezzo condiviso */
int mezzofd_g;			/* fd del mezzo condiviso */

/* ------------------------------------------------------------------------- */
/* --- Strutture dati --- */

typedef struct {
	char buf [_maxbuflen];
	int first;				/* Indice del primo byte da gestire */
	int len;				/* Numero di caratteri da gestire */
} stabuf_t;

typedef struct {
	int nready;			/* risultato al termine della select */
	fd_set Rset;		/* fd da controllare il lettura */
	fd_set Wset;		/* fd da controllare in scrittura */
	int fdtop; 			/* fd da monitorare con valore più alto */
	int connfd;			/* fd del client che si è collegato */
	int clientfd [FD_SETSIZE];	/* fd dei client connessi (sono le nostre stazioni) */
	stabuf_t clibuf [FD_SETSIZE];	/* buffer dei client connessi */
	int mezzofd;		/* fd del mezzo condiviso */
} stato_t;

/* ------------------------------------------------------------------------- */
/* --- Prototipi delle funzioni --- */

void start_mc_thread (void);
/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: fa partire il thread del mezzo condiviso
---------------------------------------------------------------------------- */



#endif






