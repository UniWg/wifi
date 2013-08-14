#include "general_include.h"

/* ------------------------------------------------------------------------- */
void* main_thread (void* param) {
	while (1) {
		printf ("MEZZO CONDIVISO !!!\n");
		sleep (1);
	}
	
	return (0);
}

/* ------------------------------------------------------------------------- */
void start_mc_thread (void) {
	int r;

	r = pthread_create (&mc_thread,NULL,main_thread,NULL);
	if (r) {
		printf ("Errore nella creazione del thread del mezzo condiviso\n");
		printf ("Codice di errore riportato da pthread_create(): %d\n",r);
		exit(-1);
	}
}

/* ------------------------------------------------------------------------- */
