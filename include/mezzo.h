#ifndef _MEZZO_H
#define _MEZZO_H

/* ------------------------------------------------------------------------- */
/* --- Costanti --- */
#define _max_connection 10	/* Numero massimo di connessioni contemporanee */

/* ------------------------------------------------------------------------- */
/* --- Variabili globali --- */

pthread_t mc_thread_g;	/* identificativo del thread del mezzo condiviso */
int mezzofd_g;			/* fd del mezzo condiviso */

/* ------------------------------------------------------------------------- */
/* --- Strutture dati --- */

typedef struct stato {
	int nready;			/* risultato al termine della select */
	fd_set Rset;		/* fd da controllare il lettura */
	fd_set Wset;		/* fd da controllare in scrittura */
	int connfd;			/* fd utilizzato per il collegamento al server */
	int clientfd [FD_SETSIZE];	/* fd dei client connessi (sono le nostre stazioni) */
} stato_t;
/* ------------------------------------------------------------------------- */
/* --- Prototipi delle funzioni --- */

void start_mc_thread (void);
/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: fa partire il thread del mezzo condiviso
---------------------------------------------------------------------------- */



#endif
