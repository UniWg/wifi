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
* Descrizione	: Inizializzo la select
* Par. Ritorno  : NULL
* Par. Formali  :
			- s	: struttura di stato
---------------------------------------------------------------------------- */
void setup_select(stato_sta_t *s, int ns){

    /* La select ha bisogno di essere riconfigurata completamente ad ogni ciclo */
    FD_ZERO (&(*s).Rset);
    FD_ZERO (&(*s).Wset);
    FD_SET (stafd_g[ns], &(*s).Rset);

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
    sain_t mezzo;
    int ris;
    
    /* Apriamo il socket -- IPV4, TCP */
	if ((stafd_g [ns] = socket (AF_INET, SOCK_STREAM, 0))<0) {
		printf (_Cerror "Stazione %d : Errore nell'apertura del socket\n" _CColor_Off, ns);
		exit (-1);
	}
 
	DEBUG_STA "STA : Socket connesso\n" END_STA 
	
	/*memset (&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr (INADDR_ANY); 
	local.sin_port = htons (0);*/ 
	
	/* Associamo la struttura dati al socket */
	/*if (bind (stafd_g [ns], (sa_t*) &local, sizeof (local))<0) {
               printf (_Cerror"Stazione %d: bind() failed, Err: %d \"%s\"\n" _CColor_Off, ns, errno, strerror (errno));
               exit(1);
    }
	DEBUG_STA "STA: Bind eseguita con successo\n" END_STA
	*/
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
			- ns: numero della stazione
			- reg : registro della stazione
---------------------------------------------------------------------------- */
void vita_stazione(stato_sta_t *s, timev_t t, int ns, sta_registry_t* reg) {
     int save_errno, errno;
	 int len;
     char *pack;								/* Pacchetto che arriva dal mezzo */
	 char *buf_loc;								/* Buffer locale in cui accodare dati */



	pack = malloc(_max_frame_buffer_size * sizeof(char));
	buf_loc = malloc(_max_frame_buffer_size * sizeof(char));

	while(1) {	
   	do {
        setup_select(s, ns);

		(*s).nready = select (1000,&(*s).Rset,&(*s).Wset,NULL,&t); /* bisogna mettere un valore appropriato al descrittore massimo */
		/* SOLO PER DEBUG */
		save_errno = errno;
		/*printf ("nready=%d\t", (*s).nready);*/
		errno = save_errno;
		/* FINE SOLO PER DEBUG */

	} while (((*s).nready<0) && (errno==EINTR));
	
	if ((*s).nready < 0) {
		printf (_Cerror "Stazione %d : Errore nella select\n" _CColor_Off, ns);
		fflush (stderr);
		exit (-1);
	}	
          /* È arrivato un pacchetto */ 
          if ((*s).nready > 0) {
                  len = sta_prendi_pacchetto(s, ns, pack, buf_loc);
                  if (complete_frame(len, buf_loc)) {
						if (CRC_zero(pack)) {		/* Il pacchetto è corrotto */ 
							reset_indice();
							if (ricezione(reg)) {
								reset_buffer();
							}
							imposta_tempo_occupazione_MC(_t_busy_error, 0, reg);
						}
						else if (pacchetto_nostro(pack, ns)) {		/* Il pacchetto che è arrivato è nostro */
							if (is_CTS(pack)) {
								if (spedito_RTS(reg)) {
									spedisci_pacchetto(pack, s, ns);
								}
							}
							else if (is_ACK(reg)) {
								continua_trasmissione ();
							}
							else if (is_RTS(reg)) {
								spedisci_CTS();
							}
							else {  /* Allora si tratta di un pacchetto */
								aggiungi_pacchetto(pack);
							}
						}
						else if (is_RTS(reg)) {
							aggiorna_MC();
						}
				}
				else {		/* Il pacchetto non è completo, bisogna accodare i dati nel buffer */
					
				}
		}
	else {		/* Scaduto timeout della select */
			if (trasmissione(reg)) {
				if (scaduto_timeout_ACK()) {
           			continua_trasmissione ();
				}
			}
			else if (buffer_trasmissione_vuoto(buf_loc)) {
				if (!buffer_allocato(buf_loc)) {
					prepara_buffer();
				}
				if (mezzo_disponibile(reg)) {	
					spedisci_RTS(reg, s, ns);
				}

			}
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
    stato_sta_t stato;
    timev_t t;
	sta_registry_t reg;


	/* Attendiamo un paio di secondi in modo da dare il tempo al mezzo condiviso 
		di mettersi in ascolto. Dopodichè possiamo effettuare la connessione anche noi */
	sleep (2);

	/* Inizializziamo il registro della stazione */
	reg.pack [0] = 0; reg.pack [1] = 2; reg.pack [2] = 1;	/* Sequenza che indica pacchetto vuoto */
	reg.t_mc_busy = 0;
	reg.in_trasmissione = FALSE;
	reg.in_ricezione = FALSE;
	reg.RTS = FALSE;

	/* Impostiamo il timeout per la select di 100 msec. */
	t.tv_sec = 0; t.tv_usec = 100000;

	/* Colleghiamo le stazioni al mezzo condiviso */
    collega_stazione(ns);
    
	sleep (2);
	

    vita_stazione(&stato, t, ns, &reg);

	return 0;

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

