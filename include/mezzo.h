#ifndef _MEZZO_H
#define _MEZZO_H

/* ------------------------------------------------------------------------- */
/* --- Costanti --- */
#define _max_connection 10	/* Numero massimo di connessioni contemporanee */

/* ------------------------------------------------------------------------- */
/* --- Variabili globali --- */

pthread_t mc_thread_g;	/* identificativo del thread del mezzo condiviso */

/* ------------------------------------------------------------------------- */
/* --- Prototipi delle funzioni --- */

void start_mc_thread (void);
/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: fa partire il thread del mezzo condiviso
---------------------------------------------------------------------------- */



#endif
