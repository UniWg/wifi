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
		printf (_Cerror "Mezzo Condiviso : Errore nell'apertura del socket\n" _CColor_Off);
		exit (-1);
	}
	DEBUG_MC "MC: Socket connesso\n" END_MC
	
	/* Modifichiamo le opzioni del socket in modo che possa accettare 
		connessioni multiple sulla stessa porta */
	optval = 1;
	if (setsockopt (mezzofd_g,SOL_SOCKET,SO_REUSEADDR,(char*)&optval,sizeof (optval)) < 0) {
		printf (_Cerror "Mezzo Condiviso : errore nella setsockopt : %d %s\n" _CColor_Off,errno,strerror (errno));
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
		printf (_Cerror "Mezzo Condiviso : Errore nella bind\n" _CColor_Off);
		close (mezzofd_g);
		exit (-1);
	}
	DEBUG_MC "MC: BIND completata\n" END_MC
	
	/* Infine ci mettiamo in ascolto, in attesa di connessioni */
	if (listen (mezzofd_g,_max_connection) < 0) {
		printf (_Cerror "Mezzo Condiviso : Errore nella listen\n" _CColor_Off);
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
	(*s).fdtop = mezzofd_g;
	(*s).connfd = -1;
	for (i=0; i<FD_SETSIZE; i++) {		/* FD_SETSIZE = 1024 (a noi ne bastano 4) */
		(*s).clientfd [i] = -1;			/* fd disponibile */
	}
	/* Anche se mezzofd è accessibile a livello globale lo memorizziamo
		in modo da avere tutte le informazioni nella struttura */
	(*s).mezzofd = mezzofd_g;
}

/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: restituisce true se il mac address passato in input corrisponde
					al mac di una stazione
* Par. Formali  : 
			- mac : mac address nel formato 6 byte
---------------------------------------------------------------------------- */
char is_sta (char* mac) {
	int i,j;
	char valido;

	for (i=0;i<_nsta;i++) {
		valido = TRUE;
		for (j=0;j<6;j++) {
			if (stazione_g [i].mac [j] != mac [j])	{
				valido = FALSE;
				break;
			}
		}
		if (valido)	return (TRUE);
	}
	return (FALSE);
}

/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: dato il numero di stazione restituisce il relativo file descriptor (lato mezzo condiviso)
* Par. Ritorno	: numero di descrittore (lato mezzo condiviso)
* Par. Formali  : 
			- sta : numero della stazione
			- s : struttura dati contenente il riferimento numero stazione - descrittore
---------------------------------------------------------------------------- */
int nsta2fd (char sta,stato_t *s) {
	int i;
	
	/* Le stazioni sono numerate da 1 a _nsta */
	for (i=0;i<FD_SETSIZE;i++) {
		if ((*s).nsta [i]== sta) {
			return ((*s).clientfd [i]);
		}
	}
	return (-1);
}

/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: attende la connessione di tutte le stazioni
* Par. Formali  : 
			- stato : indirizzo della struttura che mantiene lo stato della select
---------------------------------------------------------------------------- */
void wait_for_sta_connection (stato_t *s) {
	struct timeval t;
	int numero_eventi,save_errno,i,j,client_fd,nsta,n,len,nwrite;
	socklen_t struct_len; 
	sain_t client_addr;
	pframe_t* f; 
	char mac [18];
	char* fb; 
	
	printf (_Cmezzo "------------------------------------------------------------------\n" _CColor_Off);
	printf (_Cmezzo "Mezzo Condiviso : in attesa di connessioni da parte delle stazioni\n" _CColor_Off);
	
	nsta = 0;
	/* Attendiamo la connessione di tutte le stazioni */
	while (nsta<_nsta) {
		do {
			FD_ZERO (&(*s).Rset);
			FD_ZERO (&(*s).Wset);
			FD_SET ((*s).mezzofd, &(*s).Rset);
			/* Impostiamo il timeout di un secondo */
			t.tv_sec = 1; t.tv_usec = 0;
			/* Dobbiamo monitorare anche il file descriptor successivo all'ultimo 
				impegnato in modo da intercettare nuove connessioni */
			numero_eventi = select ((*s).fdtop+1,&(*s).Rset,&(*s).Wset,NULL,&t);
			/* Stampiamo un punto ogni secondo come indicazione di attesa */
			save_errno = errno;
			printf (".");
			errno = save_errno;
		} while ((numero_eventi<0) && (errno==EINTR));
		
		if (numero_eventi < 0) {
			printf (_Cerror "Mezzo Condiviso : Errore nella select di attesa connessioni\n" _CColor_Off);
			fflush (stderr);
			exit (-1);
		}
		
		/* Controlliamo tutti gli eventi che si sono verificati */
		for (i=0;i<numero_eventi;i++) {
			DEBUG_MC "MC: tentativo di connessione di una stazione\n" END_MC
			/* Controllo se il socket del server (sul quale sta ascoltando)
				è stato settato in lettura dalla listen */
			if (FD_ISSET ((*s).mezzofd,&(*s).Rset)) {
				/* Accettiamo la richiesta di connessione del client */
				struct_len = sizeof (client_addr);
				client_fd = accept ((*s).mezzofd, (sa_t*) &client_addr, &struct_len);
				
				if (client_fd<0) {
					 if (errno!=EINTR) {
					 	printf (_Cerror "Mezzo Condiviso : Errore nella accept\n" _CColor_Off);
						exit (-1);
					 }
				}
				else {
					/* Abbiamo una nuova connessione */
					/* Scandiamo il vettore alla ricerca del primo descrittore settato */
					for (j=0;j<FD_SETSIZE;j++) {
						/* Salviamo i dati della stazione nella prima posizione libera dell'array */
						if ((*s).clientfd [j] < 0) {
							/* Leggiamo il frame che ha spedito la stazione */
							do {
								n = recv (client_fd,(*s).clibuf [j].buf,_maxbuflen,0);
							} while ((n<0) && (errno==EINTR));
							
							/* Se la richiesta non arriva da una delle nostre stazioni allora viene scartata */
							f = get_frame_buffer ((*s).clibuf [j].buf);
							if (is_sta ((*f).addr2)) {
								str2mac ((*f).addr2,mac);
								printf (_Cmezzo "mac mittente : %s\n" _CColor_Off,mac);	
								
								/* Memorizziamo i dati della stazione */
								(*s).clientfd [j] = client_fd;
								(*s).clibuf [j].len = 0;
								(*s).clibuf [j].first = 0;
								strncpy ((*s).climac [j],(*f).addr2,6);
								(*s).nsta [j] = mac2nsta ((*f).addr2);
							
								/* Verifichiamo se aggiornare l'indice più alto da controllare */
								if (j > (*s).fdtop)		(*s).fdtop = j;
							
								nsta++;
								printf (_Cmezzo ">>>>> Stazione %d di %d connessa\n" _CColor_Off,nsta,_nsta);	
							}
							else {
								/* QUI BISOGNA CHIUDERE LA CONNESSIONE E METTERE 
									(*s).clientfd [j] = -1
								*/
							}
							fflush (stdout);
							/* Usciamo quando non ci sono più richieste da gestire */
							if (--numero_eventi <= 0)
								break;
						}
					}
				}
			}
		}	
	}
	
	/* Ora che tutte le stazioni si sono collegate, mandiamo il frame di risposta */
	/* Impostiamo i campi del frame da spedire */
	bzero (f,sizeof (pframe_t));
	(*f).data = 0;		/* frame di controllo */
	(*f).tods = 1;		/* destinato al mezzo condiviso */
	(*f).scan = 2;		/* scansione - risposta. Collegamento avvenuto */
	(*f).duration = 5;	
	(*f).packetl = _pframe_other_len;			/* Lunghezza base del pacchetto (non ci sono dati) */
	cpmac (_mac_mezzo,(*f).addr2);				/* mac address del mezzo */
	(*f).crc = _crc_ok;

	for (i=0;i<_nsta;i++) {
		strncpy ((*f).addr1,stazione_g [i].mac,6);	/* mac address della stazione di destinazione */
		/* Covertiamo la struttura in array di byte */
		fb = set_frame_buffer (f);

		/* Spedizione messaggio */
		len = (*f).packetl;
		nwrite=0;
	
		while( (n = write(nsta2fd (i+1,s), &(fb[nwrite]), len-nwrite)) >0 )
			nwrite+=n;
		if(n<0) {
			char msgerror[1024];
			sprintf(msgerror, _Cerror "Mezzo Condiviso :  write() failed [err %d] " _CColor_Off,errno);
			perror(msgerror);
			fflush(stdout);
		}
	}
	
	/* 
	E' STATA SPEDITA LA RISPOSTA A TUTTE LE STAZIONI
	ORA BISOGNA VERIFICARE SE LE STAZIONI L'HANNO RICEVUTA 
	sE TUTTO È CORRETTO, ALLORA LE STAZIONI POSSONO COMINCIARE A TRASMETTERE
	LE STAZIONI RIMANGONO IN ATTESA BLOCCANTE FINCHÈ NON HANNO RICEVUTO LA CONFERMA 
	*/
	
	printf (_Cmezzo "\n------------------------------------------------------------------\n" _CColor_Off);
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
	FD_SET ((*s).mezzofd, &(*s).Rset);
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
	
	
	/*
	  
		NELLA SELECT GESTITA IN main_mc_thread IL CONTROLLO DI CONNESSIONE DI NUOVE
		STAZIONI NON DOVRA' PIU' ESSERE FATTO 
	*/
	while (1) {
		/* Ciclo di controllo della SELECT */
		do {
			select_setup (&stato);
		} while (0);
		
		printf (_Cmezzo "MEZZO CONDIVISO\n" _CColor_Off);
		sleep (1);
	}
	
	return (0);
}

/* ------------------------------------------------------------------------- */
void start_mc_thread (void) {
	int r;

	r = pthread_create (&mc_thread_g,NULL,main_mc_thread,NULL);
	if (r) {
		printf (_Cerror "Errore nella creazione del thread del mezzo condiviso\n" _CColor_Off);
		printf (_Cerror "Codice di errore riportato da pthread_create(): %d\n" _CColor_Off,r);
		exit(-1);
	}
}

/* ------------------------------------------------------------------------- */
