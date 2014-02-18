#include "general_include.h"

const int _area_stax [] = {_area_sta1,_area_sta2,_area_sta3,_area_sta4};
const int _campo_stax [] = {_campo_sta1,_campo_sta2,_campo_sta3,_campo_sta4};
const int _sta_di_stax [] = {_sta_di_sta1,_sta_di_sta2,_sta_di_sta3,_sta_di_sta4};
const char _mac_stax [][17] = {_mac_sta1,_mac_sta2,_mac_sta3,_mac_sta4};

/* ------------------------------------------------------------------------- */	
void inizializza_stazioni (void) {
	int i;
	char* mac = (char*) malloc (sizeof (char) * 6);
	
	/* Inizializza la struttura dati di ogni stazione */
	for (i=0; i<_nsta; i++) {
		stazione_g [i].indice = i+1;
		mac2str (_mac_stax [i],stazione_g [i].mac);	
		stazione_g [i].campo = _sta_di_stax [i];
		stazione_g [i].area = _area_stax [i];
	}

	free (mac);
}
/* ------------------------------------------------------------------------- 
* Nome			: luca
* Descrizione	: Inizializzo il frame per RTS
* Par. Ritorno  : NULL
* Par. Formali  :
			- f	: frame del pacchetto da spedire
---------------------------------------------------------------------------- */
pframe_t frame_RTS (pframe_t f){
	int len, nwrite;
	char* fb; 		/* Frame buffer (con campo dati a zero) */
	
	/* Impostiamo i campi del frame da spedire */
	bzero (&f,sizeof (f));
	f.data = 0;		/* frame di controllo */
	f.tods = 1;		/* destinato al mezzo condiviso */
	f.scan = 1;		/* scansione - utilizzata per richiedere il collegamento al mezzo */
	f.duration = 5;	
	f.packetl = _pframe_other_len;			/* Lunghezza base del pacchetto (non ci sono dati) */
	cpmac (_mac_mezzo,f.addr1);				/* mac address del mezzo */
	strncpy (f.addr2,stazione_g [ns].mac,6);	/* mac address della stazione che sta trasmettendo */
	f.crc = _crc_ok;

	/* Covertiamo la struttura in array di byte */
	fb = set_frame_buffer ((pframe_t*)&f);

	/* Spedizione messaggio */
	len = f.packetl;
	nwrite=0;
	
	while( (n = write(stafd_g [ns], &(fb[nwrite]), len-nwrite)) >0 )
		nwrite+=n;
	if(n<0) {
		char msgerror[1024];
		sprintf(msgerror,_Cerror"Stazione %d :  write() failed [err %d] "_CColor_Off, ns, errno);
		perror(msgerror);
		fflush(stdout);
	}
	
}
	




/* ------------------------------------------------------------------------- 
* Nome			: luca
* Descrizione	: Inizializzo la select
* Par. Ritorno  : NULL
* Par. Formali  :
			- s	: struttura di stato
---------------------------------------------------------------------------- */
void setup_select(stato_t *s){
     
    FD_ZERO (&(*s).Rset);
    FD_ZERO (&(*s).Wset);
    FD_SET ((*s).connfd, &(*s).Rset);
}
/* ------------------------------------------------------------------------- 
* Nome			: luca
* Descrizione	: Inizializza i socket delle stazioni e li collega al mezzo 
                  condiviso 
* Par. Ritorno  : NULL
* Par. Formali  :
			- ns: numero della stazione
---------------------------------------------------------------------------- */
void collega_stazione (int ns) {
    sain_t mezzo, local;
    int ris;
    
    /* Apriamo il socket -- IPV4, TCP */
	if ((stafd_g [ns] = socket (AF_INET, SOCK_STREAM, 0))<0) {
		printf (_Cerror "Stazione %d : Errore nell'apertura del socket\n" _CColor_Off, ns);
		exit (-1);
	}
	DEBUG_STA "STA: Socket connesso\n" END_STA
	
	/*memset (&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr (INADDR_ANY); 
	local.sin_port = htons (0); */
	
	/* Associamo la struttura dati al socket */
	/*if (bind (stafd_g [ns], (sa_t*) &local, sizeof (local))<0) {
               printf (_Cerror"Stazione %d: bind() failed, Err: %d \"%s\"\n" _CColor_Off, ns, errno, strerror (errno));
               exit(1);
    }
	DEBUG_STA "STA: Bind eseguita con successo\n" END_STA*/
	
	memset (&mezzo, 0, sizeof (mezzo));
	mezzo.sin_family = AF_INET;
	mezzo.sin_addr.s_addr =	inet_addr (_indirizzoIP);
	mezzo.sin_port = htons (_portaIP);
	
	/* Richiesta di connessione */
	ris = connect (stafd_g [ns], (sa_t*) &mezzo, sizeof (mezzo));
	if (ris < 0)  {
		printf (_Cerror"Stazione %d : connect() failed, Err: %d \"%s\"\n" _CColor_Off, ns, errno, strerror(errno));
		exit(1);
	}
	DEBUG_STA "STA: Stazione connessa al mezzo condiviso\n" END_STA
	fflush(stdout);

}
/* ------------------------------------------------------------------------- 
* Nome			: luca
* Descrizione	: Ciclo principale della stazione
* Par. Ritorno  : NULL
* Par. Formali  :
			- s	: struttura di stato
			- t : tempo di risveglio
			- ns : numero della stazione
---------------------------------------------------------------------------- */
void vita_stazione(stato_t *s, timev_t *t, int ns) {
     int numero_eventi, save_errno, errno;
     pframe_t f;
	 int len, nwrite;
	 char* fb; 		/* Frame buffer (con campo dati a zero) */
	 char buf [_maxbuflen];
     
     
   	do {
        setup_select(s);

		numero_eventi = select ((*s).fdtop+1,&(*s).Rset,&(*s).Wset,NULL,t);
		/* SOLO PER DEBUG */
		save_errno = errno;
		printf ("nready=%d\n", numero_eventi);
		errno = save_errno;
		/* FINE SOLO PER DEBUG */

	} while ((numero_eventi<0) && (errno==EINTR));
	
	if (numero_eventi < 0) {
		printf (_Cerror "Stazione %d : Errore nella select\n" _CColor_Off, ns);
		fflush (stderr);
		exit (-1);
	}	
          
          if (numero_eventi == 0) {
          /* Il timeout è scaduto */
                    if (write(stafd_g [ns], &(fb[nwrite]), len-nwrite )>0) {  /* Mi chiedo se ho una trasmissione in corso */
                                
                    }
					else if ((f.packetl > 0) && ()){ /* Ho dei dati da spedire e il mezzo condiviso è disponibile*/
							/**/
					
					}
          }
          else if (recv(stafd_g [ns], &buf, _maxbuflen, 0) > 0){
               /* Ho ricevuto dei dati */              
               if (f.crc == 0) { /* I dati sono corrotti? */
                         /* Pacchetto non spedito. Errore, tocca liberare il canale */
                         
               }
			   else if (){

			   }
          }
}
/* ------------------------------------------------------------------------- 
* Nome			: luca
* Descrizione	: Thread principale di ogni stazione
* Par. Ritorno  : NULL
* Par. Formali  :
			- nsp : numero della stazione
---------------------------------------------------------------------------- */
void* main_sta_thread (void* nsp) {
	int ns = *(int*)nsp-1;	/* togliamo 1 così ns è allineato con l'indice dell'array */
    stato_t stato;
    timev_t t;
	
	/* Attendiamo un paio di secondi in modo da dare il tempo al mezzo condiviso 
		di mettersi in ascolto. Dopodichè possiamo effettuare la connessione anche noi */
	sleep (2);

	t.tv_sec = 1; t.tv_usec = 0;
	/* Colleghiamo le stazioni al mezzo condiviso */
    collega_stazione(ns);
    
    while(1){
             vita_stazione(&stato, &t, ns);
    }
    
}    
	
/* ------------------------------------------------------------------------- */
void start_sta_thread (void) {
	/* Lancia i thread di ogni stazione.
		Ai thread verrà  passato solo il numero di stazione. La struttura
		dati relativa è accessibile a livello globale */
	int r,i;
	int *n;

	for (i=1; i<=_nsta; i++) {
		n = (int*) malloc (sizeof(int));
		*n = i;
		r = pthread_create (&mc_thread_g, NULL, main_sta_thread, n);
		if (r) {
			printf ("Errore nella creazione del thread della stazione numero %d\n", i);
			printf ("Codice di errore riportato da pthread_create(): %d\n",r);
			exit(-1);
		}
	}
}

/* ------------------------------------------------------------------------- */

