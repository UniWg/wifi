#include "general_include.h"

const int _area_stax [] = {_area_sta1,_area_sta2,_area_sta3,_area_sta4};
const int _campo_stax [] = {_campo_sta1,_campo_sta2,_campo_sta3,_campo_sta4};
const int _sta_di_stax [] = {_sta_di_sta1,_sta_di_sta2,_sta_di_sta3,_sta_di_sta4};
const char _mac_stax [][17] = {_mac_sta1,_mac_sta2,_mac_sta3,_mac_sta4};


/* ------------------------------------------------------------------------- */
void collegamento (int ns) {
	/* Ci colleghiamo al mezzo condiviso, gli spediamo un pacchetto e attendiamo la risposta */
	sain_t mezzo;
	int ris,n, nwrite, len;
	char* fb; 				/* Frame buffer (con campo dati a zero) */
	char macm [6];			/* mac del mezzo condiviso in formato 6 byte */
	char buf [_maxbuflen];
	pframe_t f;
	pframe_t* fs;
	BOOL trovato;
	
	/* Apriamo il socket -- IPV4, TCP */
	if ((stafd_g [ns] = socket (AF_INET,SOCK_STREAM,0))<0) {
		printf (_Cerror "Stazione : Errore nell'apertura del socket\n" _CColor_Off);
		exit (-1);
	}
	DEBUG_STA "STA: Socket connesso\n" END_MC
	
	/* Verifichiamo se aggiornare l'indice più alto da controllare */
	if (stafd_g [ns] > max_fd)		max_fd = stafd_g [ns];
	
	/* per completezza qui bisogna aggiungere la bind */
	
	
	/* assign our destination address */
	memset ( &mezzo, 0, sizeof (mezzo) );
	mezzo.sin_family = AF_INET;
	mezzo.sin_addr.s_addr =	inet_addr (_indirizzoIP);
	mezzo.sin_port = htons (_portaIP);

	/* Richiesta di connessione */
	ris = connect (stafd_g [ns], (sa_t*) &mezzo, sizeof (mezzo));
	if (ris < 0)  {
		printf (_Cerror"Stazione : connect() failed, Err: %d \"%s\"\n" _CColor_Off,errno,strerror(errno));
		exit(1);
	}
	DEBUG_STA "STA: Stazione connessa al mezzo condiviso\n" END_MC
	fflush(stdout);
	
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
		sprintf(msgerror,_Cerror"Stazione :  write() failed [err %d] "_CColor_Off,errno);
		perror(msgerror);
		fflush(stdout);
	}
	
	/* Ora rimaniamo in attesa bloccante di ricevere il frame di risposta */
	trovato = FALSE;
	do {
		bzero (&buf,_maxbuflen);
		do {
			n = recv (stafd_g [ns],buf,_maxbuflen,0);
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

/* ------------------------------------------------------------------------- */
void inizializza_stazioni (void) {
	int i;
	char* mac = (char*) malloc (sizeof (char) * 6);
	
	/* Inizializza la struttura dati di ogni stazione */
	for (i=0;i<_nsta;i++) {
		stazione_g [i].indice = i+1;
		mac2str (_mac_stax [i],stazione_g [i].mac);	
		stazione_g [i].campo = _sta_di_stax [i];
		stazione_g [i].area = _area_stax [i];
	}

	free (mac);
}

/* ------------------------------------------------------------------------- */

void sta_select_setup (stato_sta_t *s) {
	int fd = (*s).mezzofd;
	
	/* La select ha bisogno di essere riconfigurata completamente ad ogni ciclo */
	FD_ZERO (&(*s).Rset);
	FD_ZERO (&(*s).Wset);
	FD_SET ((*s).stafd, &(*s).Rset);
	/* Mettiamo in lettura il descrittore del mezzo */
	FD_SET (fd,&(*s).Rset);
}

/* ------------------------------------------------------------------------- */
void vita_della_sta (stato_sta_t* s,timev_t t,sta_registry_t* reg) {
	int numero_eventi,len;
	char tmp_pack [_max_frame_buffer_size];		/* Pacchetto che arriva dal mezzo */

	checkUp (reg);		/* Controlla se ci sono palesi errori di configurazione */

	do {
		sta_select_setup (s);
		numero_eventi = select ((*s).fdtop+1,&(*s).Rset,&(*s).Wset,NULL,&t); 
	} while ((numero_eventi<0) && (errno==EINTR));
	
	if (numero_eventi < 0) {
		printf (_Cerror "Stazione %d : Errore nella select di attesa connessioni\n" _CColor_Off,(*reg).ns+1);
		fflush (stderr);
		exit (-1);
	}
	
	/* #### ARRIVATO PACCHETTO ################################## */
	if (numero_eventi > 0) {
		len = sta_prendi_pacchetto (s,tmp_pack,reg);
		if (pacchetto_completo (tmp_pack,len,reg)) {
			(*reg).timePACK = 0;						/* Il pacchetto è completo. Disarmiamo il timeout */
			(*reg).nBLR = 0;							/* ### verificare se è corretto azzerare nBLR dopo aver completato il pacchetto */
			if (CRC_uguale_0 (reg)) {
				resetta_parametri_del_registro (reg,TRUE); 
				imposta_tempo_occupazione_MC (_t_busy_error,0,reg);
			}
			else {
				aggiorna_mezzo_busy (reg); 
				if (pacchetto_nostro (reg)) {
					if (is_CTS (reg)) {
						if (spedito_RTS (reg)) {
							spedisci_BLT (reg);
						}
					}
					else if (is_ACK (reg)) {
						continua_trasmissione (reg);
					}
					else if (is_RTS (reg,FALSE)) {
						spedisci_CTS (reg);
					}
					else {
						char ultimo_pacchetto = FALSE;
						if (dato_da_scartare (reg)) {
							ultimo_pacchetto  = TRUE;
						}
						else {
							/* La sequenza non é completa. Accodiamo il pacchetto arrivato sulla LTR */
							costruisci_lista_ricezione_temporanea (reg);
							if (ricevuto_ultimo_pacchetto (reg)) {			/* regressivo == 1 */
								accoda_LTR_a_LRL (reg);
								ultimo_pacchetto = TRUE;
							}
						}
						spedisci_ACK (reg,ultimo_pacchetto);
					}
				} 
			}
		}
	}
	/* #### SCADUTO TIMEOUT DELLA SELECT (100ms) ################################## */
	else {
		if (scaduto_timeout_PACK (reg)) {
			resetta_parametri_del_registro (reg,FALSE); 
			imposta_tempo_occupazione_MC (_t_busy_error,0,reg);
		}
		else {
			if (stiamo_trasmettendo (reg)) {
				if (scaduto_timeout_CTS (reg)) {
					resetta_parametri_del_registro (reg,FALSE); 
					imposta_tempo_occupazione_MC (_t_busy_error,0,reg);
				}
				else {
					if (scaduto_timout_ACK (reg)) {
						continua_trasmissione (reg);
					}
				}
			}
			else if (!stiamo_ricevendo (reg)) {		
				if (mezzo_disponibile (reg)) {
					if (ho_dei_dati_da_spedire (reg)) {			
						if (!lista_temporanea_trasmissione_allocata (reg)) {
							trasferisci_sequenza_da_LTL_in_LTT (reg);
						}
						spedisci_RTS (reg);					
					}
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------- */

void* main_sta_thread (void* nsp) {
	int ns = *(int*)nsp-1;	/* togliamo 1 così ns è allineato con l'indice dell'array */
	int i;
	stato_sta_t s;
	timev_t t;
	sta_registry_t reg;

	/* Attendiamo un paio di secondi in modo da dare il tempo al mezzo condiviso 
		di mettersi in ascolto. Dopodichè possiamo effettuare la connessione anche noi */
	sleep (2);
	
	collegamento (ns);			/* fase di collegamento al mezzo condiviso */
	s.nready = -1;
	FD_ZERO (&s.Rset);
	FD_ZERO (&s.Wset);
	s.stafd = stafd_g [ns];
	s.mezzofd = mezzofd_g;
	s.fdtop = max_fd;
	
	/* Inizializziamo il registro della stazione */
	reg.t_mc_busy = 0;
	reg.in_trasmissione = FALSE;
	reg.in_ricezione = FALSE;
	reg.RTS = FALSE;
	reg.ns = ns;
	reg.LTT = fifo_create ();			/* Creiamo la LTT (vuota con sentinella) */
	reg.LTR = fifo_create ();			/* Creiamo la LTR (vuota con sentinella) */
	reg.nBLR = 0;						/* Il buffer di ricezione é inizialmente vuoto */
	reg.nBLT = 0;						/* All'inizio non abbiamo nessun pacchetto trasmesso */
	reg.timeCTS = 0;					/* Se i timer sono a zero allora sono disabilitati */
	reg.timeACK = 0;	
	reg.timePACK = 0;
	reg.nPACK = 0;
	for (i=0;i<_nsta;i++) 
		reg.lastProg [i] = -1;

	/* Impostiamo il timeout per la select di 100msec */
	t.tv_sec = 0; t.tv_usec = 100000;

	while (1) {
		vita_della_sta (&s,t,&reg);
	}

	return (0);
}

/* ------------------------------------------------------------------------- */
void start_sta_thread (void) {
	/* Lancia i thread di ogni stazione 
		Ai thread verrà passato solo il numero di stazione. La struttura
		dati relativa è accessibile a livello globale */
	int r,i;
	int *n;

	for (i=1;i<=_nsta;i++) {
		n = (int*) malloc (sizeof(int));
		*n = i;
		r = pthread_create (&mc_thread_g,NULL,main_sta_thread,n);
		if (r) {
			printf ("Errore nella creazione del thread della stazione numero %d\n",i);
			printf ("Codice di errore riportato da pthread_create(): %d\n",r);
			exit(-1);
		}
	}
}

/* ------------------------------------------------------------------------- */



