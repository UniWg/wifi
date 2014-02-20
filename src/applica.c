#include "general_include.h"

/* ------------------------------------------------------------------------- 
* Nome			: 
* Descrizione	: Thread principale di ogni applicazione
* Par. Ritorno  : NULL
* Par. Formali  :
			- nsp : numero dell'applicazione
---------------------------------------------------------------------------- */
void* main_app_thread (void* nap) {
	int na = *(int*)nap-1;	/* togliamo 1 così ns è allineato con l'indice dell'array */
	char* fb;
	char messaggio [100] = "MESSAGGIO da stazione 1 a stazione 2";
 
	printf (_Capp "Applicazione %d attiva\n" _CColor_Off,na+1);

	/* Creaimo la lista locale di trasmissione */
	LTL [na] = fifo_create ();

	/* Se siamo la stazione 1 allora mettiamo un pacchetto nella LTL */
	if (na+1 == 1) {
		/* Prepariamo il pacchetto ... */
		fb = make_pseduo_frame (messaggio,strlen (messaggio),1,1,2);
		
		/* ... e lo mettiamo in lista di trasmissione */
		pthread_mutex_lock (&mutexLTL [na]);
		fifo_push (LTL [na],fb);
		pthread_mutex_unlock (&mutexLTL [na]);
	}

	return 0;
}

/* ------------------------------------------------------------------------- */
void start_app_thread (void) {
	/* Lancia i thread di ogni applicazione.
		Ai thread verrà  passato solo un numero progressivo (1..4) che troverá una
		associazione implicita con il numero delle stazioni. La struttura
		dati relativa è accessibile a livello globale */
	int r,i;
	int *n;

	for (i=1; i<=_nsta; i++) {
		n = (int*) malloc (sizeof(int));
		*n = i;
		r = pthread_create (&mc_thread_g, NULL, main_app_thread, n);
		if (r) {
			printf ("Errore nella creazione del thread dell'applicazione numero %d\n", i);
			printf ("Codice di errore riportato da pthread_create(): %d\n",r);
			exit(-1);
		}
	}
}
