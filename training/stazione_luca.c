#include "general_include.h"

const int _area_stax [] = {_area_sta1,_area_sta2,_area_sta3,_area_sta4};
const int _campo_stax [] = {_campo_sta1,_campo_sta2,_campo_sta3,_campo_sta4};
const int _sta_di_stax [] = {_sta_di_sta1,_sta_di_sta2,_sta_di_sta3,_sta_di_sta4};
const char _mac_stax [][17] = {_mac_sta1,_mac_sta2,_mac_sta3,_mac_sta4};

/* ------------------------------------------------------------------------- */
void* main_sta_thread (void* nsp) {
	int len, n, nwrite, nready, save_errno, ns = *(int*)nsp-1;	/* togliamo 1 così ns è allineato con l'indice dell'array */
	char mac [18];
	pframe_t f;
	pframe_t* pf;
	char* fb;
	char* messaggio_test = "Messaggio di test";
	char buf [_max_frame_buffer_size];
	timev_t t;
	
	/* Attendiamo un paio di secondi in modo da dare il tempo al mezzo condiviso 
		di mettersi in ascolto. Dopodichè possiamo effettuare la connessione anche noi */
	sleep (2);
	
	for (int i=0; i<_nsta; i++) {
        collega_stazioni(i);
    }
    
    while (1) {
          FD_ZERO(&Rset);
          FD_ZERO(&Wset);
          FD_SET(stafd_g[ns], &Rset);
          /* Impostiamo il timeout */
          t.tv_sec = 1; t.tv_usec = 0;
          nready = select( _nsta+1, &Rset, &Wset, NULL, &t);
          save_errno = errno;
          errno = save_errno;
          
          if (nready<0) {
            printf (_Cerror "Stazione : Errore nella select \n" _CColor_Off);
			fflush (stderr);
			exit (-1);
		  }
          
          /* Controllo se il timeout è scaduto */
          if (t<=0) {
                    /* Mi chiedo se ho una trasmissione in corso */
          }
          else if (/* ricevuto dati */){
               
          }
    }
}    
	
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

/* ------------------------------------------------------------------------- */
void collega_stazioni (int ns) {
    sain_t mezzo, local;
    int ris;
    
    /* Apriamo il socket -- IPV4, TCP */
	if ((stafd_g [ns] = socket (AF_INET, SOCK_STREAM, 0))<0) {
		printf (_Cerror "Stazione : Errore nell'apertura del socket\n" _CColor_Off);
		exit (-1);
	}
	DEBUG_STA "STA: Socket connesso\n" END_STA
	
	memset (&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr (INADDR_ANY); 
	local.sin_port = htons (0);
	
	/* Associamo la struttura dati al socket */
	ris = bind (stafd_g [ns], (sa_t*) &local, sizeof (local));
	if (ris<0) {
               printf (_Cerror"Stazione: bind() failed, Err: %d \"%s\"\n" _CColor_Off, errno, strerror (errno));
               exit(1);
    }
	DEBUG_STA "STA: Bind eseguita con successo\n" END_STA
	
	memset (&mezzo, 0, sizeof (mezzo));
	mezzo.sin_family = AF_INET;
	mezzo.sin_addr.s_addr =	inet_addr (_indirizzoIP);
	mezzo.sin_port = htons (_portaIP);
	
	/* Richiesta di connessione */
	ris = connect (stafd_g [ns], (sa_t*) &mezzo, sizeof (mezzo));
	if (ris < 0)  {
		printf (_Cerror"Stazione : connect() failed, Err: %d \"%s\"\n" _CColor_Off, errno, strerror(errno));
		exit(1);
	}
	DEBUG_STA "STA: Stazione connessa al mezzo condiviso\n" END_STA
	fflush(stdout);

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

