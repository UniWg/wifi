#ifndef _MEZZO_H
#define _MEZZO_H

#include "stazione.h"

/* ------------------------------------------------------------------------- */
/* --- Costanti --- */
#define _n_area 2			/* Numero di aree gestite dal mezzo condiviso A1 -> sta 1,2,4 ** A2 -> sta 3,4 */
#define _max_connection 10	/* Numero massimo di connessioni contemporanee */
#define _Cmezzo _CCyan		/* Colore dei messaggi del mezzo condiviso */

#define _mac_mezzo "A1:B2:C3:D4:E5:F6"

/* ------------------------------------------------------------------------- */
/* --- Strutture dati --- */

typedef struct {
	char buf [_maxbuflen];
	int first;				/* Indice del primo byte da gestire */
	int len;				/* Numero di caratteri da gestire */
	timev_t timestamp;		/* tempo di spedizione del pacchetto (tempo di arrivo + latenza) */
	char stamacdest [6];		/* mac del destinatario */
} stabuf_t;

typedef struct {
	int nready;			/* risultato al termine della select */
	fd_set Rset;		/* fd da controllare il lettura */
	fd_set Wset;		/* fd da controllare in scrittura */
	int fdtop; 			/* fd da monitorare con valore più alto */
	int connfd;			/* fd del client che si è collegato */
	int clientfd [_nsta];		/* fd dei client connessi (lato mezzo condiviso) */
	char climac [_nsta][6];		/* mac dei client connessi */
	stabuf_t clibuf [_nsta];	/* buffer dei client connessi */
	int mezzofd;		/* fd del mezzo condiviso */
} stato_t;

typedef struct {
	char pack [_max_frame_buffer_size];		/* Pacchetto in transito nell'area */
	long durata;							/* TDout = tempo attuale + tempo di trasmissione (in millisec)
											   In pratica viene memorizzate il momento (epoch) in 
											   cui la trasmissione termina */
	char spedita_prima_parte;				/* TRUE se ha spedito n-1 byte */
} area_t;

/* ------------------------------------------------------------------------- */
/* --- Variabili globali --- */

pthread_t mc_thread_g;		/* identificativo del thread del mezzo condiviso */
int mezzofd_g;				/* fd del mezzo condiviso */

/* ------------------------------------------------------------------------- */
/* --- Prototipi delle funzioni --- */

void start_mc_thread (void);
/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: fa partire il thread del mezzo condiviso
---------------------------------------------------------------------------- */



#endif






