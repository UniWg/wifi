#include "general_include.h"

/* ------------------------------------------------------------------------- */

void null_print (const char *format, ...) {
	/* non fa nulla. Serve per non eseguire le stampe di debug */
}

/* ------------------------------------------------------------------------- */

void inizializza (void) {
	/* Inizilizza le stazioni : nome, mac, ... */
	inizializza_stazioni ();
}

/* ------------------------------------------------------------------------- */

void start_thread (int ricetta) {
	/* Thread del mezzo condiviso */
	start_mc_thread ();
	
	/* Threads delle stazioni */
	start_sta_thread ();
	
	/* Threads delle applicazioni */
	start_app_thread (ricetta);
}

/* ------------------------------------------------------------------------- */

int main_menu (void) {
	int i;

	printf ("\n");
	printf ("	 ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~\n");
	printf ("	| Simulazione di una rete wifi Ad-Hoc |\n");
	printf ("	 ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~\n");
	printf ("\n");
	printf (" 1 - Ping Pong\n");
	printf (" 2 - Underground\n");
	printf (" 3 - Pain Chain\n");
	printf (" 4 - Crazy Ball\n");
	printf (" 5 - Fast & Furious\n");
	printf (" 6 - Fast & Furious 2\n");
	printf (" 7 - Simple Crash\n");
	printf (" 8 - Mega Crash\n");
	printf ("99 - Exit\n");
	printf ("\n");
	printf ("Seleziona : ");
  	scanf ("%d",&i);
  	
  	return (i);
}

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

int main (int argv, char** argc) {
	int selezione;
	
	selezione = main_menu ();

	if ((selezione>0) && (selezione<=8)) {
		DEBUG_PRINT "MAIN: Inizilizzazione variabili e strutture\n" END_DP;
		inizializza ();

		DEBUG_PRINT "MAIN: Inizilizzazione thread\n" END_DP;
		start_thread (selezione);

		/* Il main termina, ma lascia in vita gli altri threads */
		pthread_exit (NULL);
	}

	return (0);
}


