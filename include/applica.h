#ifndef _APPLICA_H
#define _APPLICA_H

#define _Capp _CBPurple				/* Colore dei messaggi delle applicazioni */

#define _sim_ping_pong 1			/* Definiamo le costanti che identificano le simulazioni */
#define _sim_underground 2
#define _sim_pain_chain 3
#define _sim_crazy_ball 4
#define _sim_fast_n_furious 5
#define _sim_fast_n_furious_2 6
#define _sim_simple_crash 7
#define _sim_mega_crash 8

list2* LTL [_nsta];					/* Lista di Trasmissione Locale  ... */
pthread_mutex_t mutexLTL [_nsta];	/* ... e relative mutex */
list2* LRL [_nsta];					/* Lista di Ricezione Locale  ... */
pthread_mutex_t mutexLRL [_nsta];	/* ... e relative mutex */

pthread_t tidentity [_nsta];		/* Contiene gli identificativi dei thread delle applicazioni */	

char prog_crc [_nsta];				/* Progressivi dei pacchetti */
char app_cicli;						/* Contatore di cicli per le simulazioni */

/* ---------------------------------------------------------------------------- */

typedef struct {
	int napp;						/* Numero dell'applicazione (1..4) */
	int ricetta;					/* Numero della simulazione scelta dall'utente */
} applica_t;

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: crea i thread delle applicazioni
* Par. Formali  : 
			- Ricetta : tipo di simulazione da eseguire
---------------------------------------------------------------------------- */
void start_app_thread (int ricetta);


#endif
