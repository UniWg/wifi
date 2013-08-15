#include "general_include.h"

/* ------------------------------------------------------------------------- */

void null_print (const char *format, ...) {
	/* non fa nulla. Serve per non eseguire le stampa di debug */
}

/* ------------------------------------------------------------------------- */

void inizializza (void) {
	/* Inizilizza le stazioni : nome, mac, ... */
	inizializza_stazioni ();
}

/* ------------------------------------------------------------------------- */

void start_thread (void) {
	/* Thread del mezzo condiviso */
	start_mc_thread ();
	
	/* Threads delle stazioni */
	start_sta_thread ();
}

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

int main (int argv, char** argc) {

	DEBUG_PRINT "Inizilizzazione variabili e strutture\n" END_DP;
	inizializza ();

	DEBUG_PRINT "Inizilizzazione thread\n" END_DP;
	start_thread ();

	/* Il main termina, ma lascia in vita gli altri threads */
	pthread_exit (NULL);

	return (0);
}


