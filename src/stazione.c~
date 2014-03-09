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
	int fd = (*s).mezzofd;

    /* La select ha bisogno di essere riconfigurata completamente ad ogni ciclo */
    FD_ZERO (&(*s).Rset);
    FD_ZERO (&(*s).Wset);
    FD_SET (stafd_g[ns], &(*s).Rset);
	/* Mettiamo in lettura il descrittore del mezzo */
	FD_SET (fd,&(*s).Rset);

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
    int ris, nwrite, len, n;
	pframe_t f;
	pframe_t *fs;
	char macm [6];		/* mac del mezzo condiviso in formato 6 byte */
	char* fb;
	char buf [_maxbuflen];
	char trovato;
    
    /* Apriamo il socket -- IPV4, TCP */
	if ((stafd_g [ns] = socket (AF_INET, SOCK_STREAM, 0))<0) {
		printf (_Cerror "Stazione %d : Errore nell'apertura del socket\n" _CColor_Off, ns);
		exit (-1);
	}
 
	DEBUG_STA "STA : Socket connesso\n" END_STA 

	/* Verifichiamo se aggiornare l'indice più alto da controllare */
	if (stafd_g [ns] > max_fd)		max_fd = stafd_g [ns];
	
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

	/* Impostiamo i campi del frame da spedire */
	bzero (&f,sizeof (f));
	f.data = 0;									/* frame di controllo */
	f.tods = 1;									/* destinato al mezzo condiviso */
	f.scan = 1;									/* scansione - utilizzata per richiedere il collegamento al mezzo */
	f.duration = 5;	
	f.packetl = _pframe_other_len;				/* Lunghezza base del pacchetto (non ci sono dati) */
	cpmac (_mac_mezzo,f.addr1);					/* mac address del mezzo */
	strncpy (f.addr2,stazione_g [ns].mac,6);	/* mac address della stazione che sta trasmettendo */
	f.crc = _crc_ok;

	/* Covertiamo la struttura in array di byte */
	fb = set_frame_buffer(&f);

	/* Spedizione messaggio */
	len = f.packetl;
	nwrite=0;
	
	while( (n = write(stafd_g [ns], &(fb[nwrite]), len-nwrite)) >0 )
		nwrite+=n;
	if(n<0) {
		char msgerror[1024];
		sprintf(msgerror,_Cerror"Stazione :  write() failed [err %d] "_CColor_Off,errno);
		perror(msgerror);
		fflush(stdout);
	}
	
	/* Ora rimaniamo in attesa bloccante di ricevere il frame di risposta */
	trovato = FALSE;
	do {
		bzero (&buf, _maxbuflen);
		do {
			n = recv (stafd_g [ns], buf, _maxbuflen,0);
		} while ((n<0) && (errno==EINTR));
		/* Abbiamo ricevuto un frame */
		/* Lo spacchettiamo e vediamo se è arrivato dal mezzo condiviso */
		fs = get_frame_buffer (buf);
		mac2str (_mac_mezzo,macm);
		if (strncmp ((*fs).addr2,macm,6) != 0)
			printf (_Csta "Stazione : Ricevuto un frame, ma non dal mezzo condiviso\n" _CColor_Off);
		else {
			/* Vediamo se è proprio il frame di risposta */
			if ((*fs).scan == 2) {
				printf (_Csta "Stazione %d : ricevuto frame di risposta. Pronta per comunicare\n" _CColor_Off,ns+1);
				trovato = TRUE;
			}
			else	printf (_Csta "Stazione : Ricevuto un frame dal mezzo, ma senza il campo scan settato\n" _CColor_Off);
		}
	} while (!trovato);
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
	 int len=0;
	 char tmp_pack [_max_frame_buffer_size];		/* Pacchetto che arriva dal mezzo */
	
   	do {
        setup_select(s, ns);

		(*s).nready = select ((*s).fdtop+1,&(*s).Rset,&(*s).Wset,NULL,&t); 
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

				  /* Prendiamo il pacchetto */
                  len = sta_prendi_pacchetto(s, tmp_pack, reg);

				  /* Se il pacchetto è completo... */
                  if (pacchetto_completo(len, tmp_pack, reg)) {
						(*reg).timePACK = 0;						/* Il pacchetto è completo. Disarmiamo il timeout */
			 			(*reg).nBLR = 0;							
						/* ... bisogna vedere se è il pacchetto è corrotto... */
						if (CRC_zero(reg)) {		
							
							reset_parametri(reg, TRUE);
							
							imposta_tempo_occupazione_MC(_t_busy_error, 0, reg);

						}
						else {
							/* Se il pacchetto non è corrotto aggiorno il tempo di occupazione del canale */
							aggiorna_MC(reg);

							/* ... e bisogna controllare se il pacchetto che è arrivato è nostro */
							if (pacchetto_nostro(reg)) {	
		
								if (is_CTS(reg)) {

									if (spedito_RTS(reg)) {			

										from_LTT_to_BLT(reg);
									}
								}
								else if (is_ACK(reg)) {

									continua_trasmissione (reg);
								}
								else if (is_RTS(reg)) {

									spedisci_CTS(reg, s);
								}
								else { 
									/* Allora il pacchetto che è arrivato è un pacchetto dati */
									aggiungi_pacchetto(reg);
								}
						
							}
							else {
								/* Se siamo qui vuol dire che bisogna scartare il pacchetto */
							

								/*reset_buffer(reg);*/
							}
						}
				}
				
		}
	/* Scaduto timeout della select */
	else {	

			if (scaduto_timeout_PACK (reg)) {
				reset_parametri(reg, FALSE);
				imposta_tempo_occupazione_MC(_t_busy_error, 0, reg);
			}
			else {
				if (trasmissione(reg)) {
					if (scaduto_timeout_CTS(reg)) {
						reset_parametri(reg, FALSE);
						imposta_tempo_occupazione_MC(_t_busy_error, 0, reg);
					}
					else {
						if (scaduto_timeout_ACK(reg)) {

				   			continua_trasmissione (reg);
						}
					}
				}
				else if (!ricezione(reg)) {

					if (mezzo_disponibile(reg)) {

						 if (!buffer_trasmissione_vuoto(reg)) {		

							if (!buffer_allocato(reg)) {

								from_LTL_to_LTT(reg);
							}
								spedisci_RTS(reg, s);
						 }
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
	reg.LTT  = fifo_create();			/* Creiamo la LTT (vuota con sentinella) */				
	reg.LTR  = fifo_create();			/* Creiamo la LTR (vuota con sentinella) */
	reg.t_mc_busy = 0;
	reg.in_trasmissione = FALSE;
	reg.in_ricezione = FALSE;
	reg.RTS = FALSE;
	reg.nBLR = 0;						/* Il buffer di ricezione é inizialmente vuoto */
	reg.nBLT = 0;						/* All'inizio non abbiamo nessun pacchetto trasmesso */
	reg.timeCTS = 0;					/* Se i timer sono a zero allora sono disabilitati */
	reg.timeACK = 0;	
	reg.timePACK = 0;
	reg.nPACK = 0;
	reg.ns = ns;
	reg.lastProg[ns] = -1;
	
	/* Impostiamo il timeout per la select di 100 msec. */
	t.tv_sec = 0; t.tv_usec = 100000;

	/* Colleghiamo le stazioni al mezzo condiviso */
    collega_stazione(ns);
	stato.nready = -1;
	FD_ZERO (&stato.Rset);
	FD_ZERO (&stato.Wset);
	/*stato.stafd = stafd_g [ns];*/
	stato.mezzofd = mezzofd_g;
	stato.fdtop = max_fd;

	sleep (2);
	
	while(1) {
    vita_stazione(&stato, t, ns, &reg);
	}

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

