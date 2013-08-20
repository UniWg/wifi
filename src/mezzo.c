#include "general_include.h"

/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: inizializza il socket del mezzo condiviso in modo che sia
					pronto ad accettare connessioni
---------------------------------------------------------------------------- */
void inizializza_socket (void) {
/* Inizializza il socket che accetterà le connessioni delle stazioni */
	sain_t mcaddr;
	int optval;
		
	/* Apriamo il socket -- IPV4, TCP */
	if ((mezzofd_g = socket (AF_INET,SOCK_STREAM,0))<0) {
		printf ("Mezzo Condiviso : Errore nell'apertura del socket\n");
		exit (-1);
	}
	DEBUG_MC "MC: Socket connesso\n" END_MC
	
	/* Modifichiamo le opzioni del socket in modo che possa accettare 
		connessioni multiple sulla stessa porta */
	optval = 1;
	if (setsockopt (mezzofd_g,SOL_SOCKET,SO_REUSEADDR,(char*)&optval,sizeof (optval)) < 0) {
		printf ("Mezzo Condiviso : errore nella setsockopt : %d %s\n",errno,strerror (errno));
		exit (-1);
	}
	
	/* Azzeriamo la struttura dati ... */
	bzero (&mcaddr,sizeof (mcaddr));
	/* ... poi impostiamo i parametri per la bind */
	mcaddr.sin_family = AF_INET;
	/* Inidirizzo IP e porta di ascolto */
	mcaddr.sin_addr.s_addr = inet_addr (_indirizzoIP);
	mcaddr.sin_port = htons (_portaIP);
	if (bind (mezzofd_g,(sa_t*)&mcaddr,sizeof (mcaddr))<0) {
		printf ("Mezzo Condiviso : Errore nella bind\n");
		close (mezzofd_g);
		exit (-1);
	}
	DEBUG_MC "MC: BIND completata\n" END_MC
	
	/* Infine ci mettiamo in ascolto, in attesa di connessioni */
	if (listen (mezzofd_g,_max_connection) < 0) {
		printf ("Mezzo Condiviso : Errore nella listen\n");
		close (mezzofd_g);
		exit (-1);
	}
	DEBUG_MC "MC: LISTEN completata. Siamo in ascolto\n" END_MC
	DEBUG_MC "---------------------------------------\n" END_MC
}

/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: inizializza i campi della struttura dati
* Par. Formali  : 
			- s : indirizzo della struttura dati da inizializzare
---------------------------------------------------------------------------- */
void init_stato (stato_t *s) {
	int i;
	
	(*s).nready = -1;
	FD_ZERO (&(*s).Rset);
	FD_ZERO (&(*s).Wset);
	(*s).connfd = -1;
	for (i=0; i<FD_SETSIZE; i++) {		/* FD_SETSIZE = 1024 */
		(*s).clientfd [i] = -1;			/* fd disponibile */
	}
}

/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: inizializza la select 
* Par. Formali  : 
			- s : indirizzo della struttura che mantiene lo stato della select
---------------------------------------------------------------------------- */
void select_setup (stato_t *s) {
	/* La select ha bisogno di essere riconfigurata completamente ad ogni ciclo */
	FD_ZERO (&(*s).Rset);
	FD_ZERO (&(*s).Wset);
	
}

/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: attende la connessione di tutte le stazioni
* Par. Formali  : 
			- stato : indirizzo della struttura che mantiene lo stato della select
---------------------------------------------------------------------------- */
void wait_for_sta_connection (stato_t *stato) {
	printf ("------------------------------------------------------------------\n");
	printf ("Mezzo Condiviso : in attesa di connessioni da parte delle stazioni\n");
	
	
	/*
	  SONO QUA
		BISOGNA FARE UN PRIMO CICLO IN CUI SI ATTENDONO LE CONNESSIONI DALLE STAZIONI
		
		NELLA SELECT GESTITA IN main_mc_thread IL CONTROLLO DI CONNESSIONE DI NUOVE
		STAZIONI NON DOVRA' PIU' ESSERE FATTO 
	*/
		
	do {
		select_setup (stato);
	} while (0);
	
	printf ("------------------------------------------------------------------\n");
}

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Thread principale del mezzo condiviso
* Par. Ritorno  : NULL
* Par. Formali  : param = NULL
---------------------------------------------------------------------------- */
void* main_mc_thread (void* param) {
	stato_t stato;
	
	/* Inizializziamo il socket in modo che sia pronto ad accettare connessioni */
	inizializza_socket ();
	
	/* Prima di procedere tutte le stazioni devono essere collegate */
	init_stato (&stato);
	wait_for_sta_connection (&stato);
	
	while (1) {
		/* Ciclo di controllo della SELECT */
		do {
			select_setup (&stato);
		} while (0);
		
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
