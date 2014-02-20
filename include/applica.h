#ifndef _APPLICA_H
#define _APPLICA_H

#define _Capp _CBPurple				/* Colore dei messaggi delle applicazioni */

list2* LTL [_nsta];					/* Lista di Trasmissione Locale  ... */
pthread_mutex_t mutexLTL [_nsta];	/* ... e relative mutex */

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: crea i thread delle applicazioni
---------------------------------------------------------------------------- */
void start_app_thread (void);


#endif
