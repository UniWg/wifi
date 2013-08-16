#include "general_include.h"

/* ------------------------------------------------------------------------- */

void inizializza_socket (void) {
/* Inizializza il socket che accetterà le connessioni delle stazioni */
	sain_t mcaddr;
	int fd,optval;
		
	/* Apriamo il socket -- IPV4, TCP */
	if ((fd = socket (AF_INET,SOCK_STREAM,0))<0) {
		printf ("Mezzo Condiviso : Errore nell'apertura del socket\n");
		exit (-1);
	}
	DEBUG_MC "MC: Socket connesso\n" END_MC
	
	/* Modifichiamo le opzioni del socket in modo che possa accettare 
		connessioni multiple sulla stessa porta */
	optval = 1;
	if (setsockopt (fd,SOL_SOCKET,SO_REUSEADDR,(char*)&optval,sizeof (optval)) < 0) {
		printf ("Mezzo Condiviso : errore nella setsockopt : %d %s\n",errno,strerror (errno));
		exit (-1);
	}
	
	/* Azzeriamo la struttura dati ... */
	bzero (&mcaddr,sizeof (mcaddr));
	/* ... poi impostiamo i parametri per la bind */
	mcaddr.sin_family = AF_INET;
	mcaddr.sin_addr.s_addr = inet_addr (_indirizzoIP);
	mcaddr.sin_port = htons (_portaIP);
	if (bind (fd,(sa_t*)&mcaddr,sizeof (mcaddr))<0) {
		printf ("Mezzo Condiviso : Errore nella bind\n");
		close (fd);
		exit (-1);
	}
	DEBUG_MC "MC: BIND completata\n" END_MC
	
	/* Infine ci mettiamo in ascolto, in attesa di connessioni */
	if (listen (fd,_max_connection) < 0) {
		printf ("Mezzo Condiviso : Errore nella listen\n");
		close (fd);
		exit (-1);
	}
	DEBUG_MC "MC: LISTEN completata. Siamo in ascolto\n" END_MC
	DEBUG_MC "---------------------------------------\n" END_MC
}

/* ------------------------------------------------------------------------- */
void* main_mc_thread (void* param) {

	inizializza_socket ();
	
	while (1) {
		printf ("MEZZO CONDIVISO !!!\n");
		sleep (1);
	}
	
	return (0);
}

/* ------------------------------------------------------------------------- */
void start_mc_thread (void) {
	int r;

	r = pthread_create (&mc_thread_g,NULL,main_mc_thread,NULL);
	if (r) {
		printf ("Errore nella creazione del thread del mezzo condiviso\n");
		printf ("Codice di errore riportato da pthread_create(): %d\n",r);
		exit(-1);
	}
}

/* ------------------------------------------------------------------------- */
