#include "general_include.h"

const int _area_stax [] = {_area_sta1,_area_sta2,_area_sta3,_area_sta4};
const int _campo_stax [] = {_campo_sta1,_campo_sta2,_campo_sta3,_campo_sta4};
const int _sta_di_stax [] = {_sta_di_sta1,_sta_di_sta2,_sta_di_sta3,_sta_di_sta4};
const char _mac_stax [][17] = {_mac_sta1,_mac_sta2,_mac_sta3,_mac_sta4};


/* ------------------------------------------------------------------------- */
void packet_test (int ns) {
	/* funzione di prova da CANCELLARE */
	
	
	/* Ci colleghiamo al mezzo condiviso, gli spediamo un pacchetto e attendiamo la risposta */
	sain_t mezzo;
	int ris,n, nwrite, len;
	char* fb; 		/* Frame buffer (con campo dati a zero) */
	char macm [6];		/* mac del mezzo condiviso in formato 6 byte */
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
	int i;
	/* La select ha bisogno di essere riconfigurata completamente ad ogni ciclo */
	FD_ZERO (&(*s).Rset);
	FD_ZERO (&(*s).Wset);
	FD_SET ((*s).stafd, &(*s).Rset);
	/* Mettiamo in lettura e scrittura il descrittore del mezzo */
	for (i=0;i<_nsta;i++) {
		int fd = (*s).mezzofd;
		FD_SET (fd,&(*s).Rset);
		/*FD_SET (fd,&(*s).Wset);*/
	}
}

/* ------------------------------------------------------------------------- */

void vita_della_sta (stato_sta_t* s,timev_t t,int ns,sta_registry_t* reg) {
	int numero_eventi,len;
	char tmp_pack [_max_frame_buffer_size];		/* Pacchetto che arriva dal mezzo */

	do {
		sta_select_setup (s);
		numero_eventi = select ((*s).fdtop+1,&(*s).Rset,&(*s).Wset,NULL,&t);
	} while ((numero_eventi<0) && (errno==EINTR));
	
	if (numero_eventi < 0) {
		printf (_Cerror "Stazione %d : Errore nella select di attesa connessioni\n" _CColor_Off,ns+1);
		fflush (stderr);
		exit (-1);
	}
	
	
	/****** IMPORTANTE 
		TUTTE LE VOLTE CHE SI TRASMETTE VERSO IL MEZZO BISOGNA VERIFICARE LA DISPONIBILITÁ (EPOCH) long t_mc_busy;
	*/
	
	/* #### ARRIVATO PACCHETTO ################################## */
	if (numero_eventi > 0) {
		len = prendi_pacchetto (s,tmp_pack);
		if (pacchetto_completo (tmp_pack,len,reg)) {
			if (CRC_uguale_0 (pack)) {
				resetta_indice_progressivo (); /* questo non so esattamente se é utile */
				if (stiamo_ricevendo (reg)) {
					resetta_buffer_ricezione (reg);
				}
				imposta_tempo_occupazione_MC (_t_busy_error,0);
			}
			else if (pacchetto_nostro (reg)) {
				if (is_CTS (reg)) {
					if (spedito_RTS (reg)) {
						spedisci_pacchetto_a_mezzo ();
					}
				}
				else if (is_ACK (reg)) {
					continua_trasmissione ();
				}
				else if (is_RTS (reg)) {
					spedisci_CTS ();
				}
				else {
					/* É arrivato un pacchetto dati */
					accoda_pacchetto_a_buffer_ricezione_temporaneo (reg);
				}
			}
			else if (is_RTS (reg)) {
				aggiorna_mezzo_busy (); /* qui dentro si puó usare la funzion imposta_tempo_occupazione_MC */
			}
		}
		else {
			/* Il pacchetto non é completo. Accodiamo i dati arrivati ora a quelli arrivati in precedenza */
			costruisci_pacchetto (tmp_pack,len,pack);
			if (ricevuto_ultimo_pacchetto ()) {			/* regressivo == 1 */
				accoda_buffer_temporaneo_a_quello_di_ricezione (reg);
			}
			spedisci_ACK ();
		}
	}
	/* #### SCADUTO TIMEOUT DELLA SELECT (100ms) ################################## */
	else {
		if (stiamo_trasmettendo (reg)) {
			if (scaduto_timout_ACK ()) {
				continua_trasmissione ();
			}
		}
		else if (!stiamo_ricevendo (reg)) {
			if (buffer_locale_di_trasmissione_pieno ()) {
				if (buffer_locale_trasmissione_allocato ()) {
					if (mezzo_disponibile ()) {
						invia_RTS ();						/* Questo deve impostare - trasmissione in corso - */
					}
				} 
				else {
					prepara_buffer_locale_trasmissione ();
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------- */

void* main_sta_thread (void* nsp) {
	int ns = *(int*)nsp-1;	/* togliamo 1 così ns è allineato con l'indice dell'array */
	stato_sta_t s;
	timev_t t;
	sta_registry_t reg;

	/* Attendiamo un paio di secondi in modo da dare il tempo al mezzo condiviso 
		di mettersi in ascolto. Dopodichè possiamo effettuare la connessione anche noi */
	sleep (2);
	
	packet_test (ns);			/* fase di collegamento al mezzo condiviso */
	s.nready = -1;
	FD_ZERO (&s.Rset);
	FD_ZERO (&s.Wset);
	s.stafd = stafd_g [ns];
	s.mezzofd = mezzofd_g;
	s.fdtop = mezzofd_g;
	
	/* Inizializziamo il registro della stazione */
	reg.pack [0] = 0; reg.pack [1] = 2; reg.pack [2] = 1;	/* Sequenza che indica pacchetto vuoto */
	reg.t_mc_busy = 0;
	reg.in_trasmissione = FALSE;
	reg.in_ricezione = FALSE;
	reg.RTS = FALSE;


	/* Impostiamo il timeout per la select di 100msec */
	t.tv_sec = 0; t.tv_usec = 100000;

	
	while (1) {
		vita_della_sta (&s,t,ns,&reg);
	}

	return (0);
}

/* ------------------------------------------------------------------------- */

/* DA CANCELLARE #############################################3 */


void* main_sta_thread0 (void* nsp) {
	int len,n,ntotal=0,nwrite,numero_eventi,ns = *(int*)nsp-1;	/* togliamo 1 così ns è allineato con l'indice dell'array */
	timev_t t;
	fd_set Rset,Wset;	
	pframe_t f;
	pframe_t* pf;
	char* fb;
	char buf [_max_frame_buffer_size];
	char realbuf [_max_frame_buffer_size];
	int one_time = 1;
	
	char* messaggio_test = "Messaggio di test da 1 verso 4";
	
	/* Attendiamo un paio di secondi in modo da dare il tempo al mezzo condiviso 
		di mettersi in ascolto. Dopodichè possiamo effettuare la connessione anche noi */
	sleep (2);

	
	packet_test (ns);			/* fase di collegamento al mezzo condiviso */
	
	/* Impostiamo il timeout di 100msec */
	t.tv_sec = 0; t.tv_usec = 100000;
	
	while (1) {
	
	
	
		/* La stazione 1 scrive sulla 4 */	
		if ((ns == 0) && (one_time == 1)) {
			one_time = 0;
			
			bzero (&f,sizeof (f));
			f.data = 1;		/* pacchetto dati */
			f.tods = 0;		/* destinato ad una stazione */
			f.scan = 0;		/* siamo già collegati, quindi non richiediamo nessuna scansione */
			f.duration = 5;		/* Per il test la durata la lasciamo a 5. Dovrà poi essere calcolata in base
									al tipo e alla lunghezza del frame */
			f.packetl = _pframe_other_len + strlen (messaggio_test);	/* Lunghezza totale del pacchetto (base + dati) */
			cpmac (_mac_sta4,f.addr1);					/* mac address del destinatario */
			strncpy (f.addr2,stazione_g [ns].mac,6);	/* mac address della stazione che sta trasmettendo */
			f.buf = messaggio_test;
			f.crc = _crc_ok;

			/* Convertiamo la struttura in array di byte */
			fb = set_frame_buffer ((pframe_t*)&f);

			/* Spedizione messaggio */
			len = f.packetl;
			nwrite=0;
	
			while( (n = write(stafd_g [ns], &(fb[nwrite]), len-nwrite)) >0 )
				nwrite+=n;
				
			printf (_Csta "Stazione %d -- spedito messaggio : %s\n" _CColor_Off,ns+1,f.buf);
		}
	
	
	
	
	
	
	
	
		do {
			/* La select ha bisogno di essere riconfigurata completamente ad ogni ciclo */
			FD_ZERO (&Rset);
			FD_ZERO (&Wset);
			FD_SET (stafd_g [ns], &Rset);	/* Controlliamo in lettura il descrittore del mezzo condiviso */
	
			/* Ci interessa monitorare solo il descrittore del mezzo condiviso*/
			numero_eventi = select (100,&Rset,&Wset,NULL,&t);
		} while ((numero_eventi<0) && (errno==EINTR));
	
		if (numero_eventi < 0) {
			printf (_Cerror "Stazione : Errore nella select di attesa connessioni\n" _CColor_Off);
			fflush (stderr);
			exit (-1);
		}
		
		/* Ci interessano solo gli eventi in lettura sul nostro descrittore */
		if (FD_ISSET (stafd_g [ns],&Rset)) {
			printf ("Stazione %d -- Arrivato pacchetto \n",ns+1);
		
			/* Leggiamo il pacchetto arrivato */	
			do {
				n = recv (stafd_g [ns],buf,_max_frame_buffer_size,0);
			} while ((n<0) && (errno==EINTR));
			
			
			ntotal = appendi_pacchetto (realbuf,buf,ntotal,n);
			if (complete_frame (ntotal,realbuf) == TRUE) {
				/* Spacchettiamo e vediamo se è destinato a noi */
				pf = get_frame_buffer (realbuf);
	
				if (mac2nsta ((*pf).addr1) == (ns + 1)) 
					printf (_Csta "Stazione %d -- ricevuto messaggio : %s\n" _CColor_Off,ns+1,(*pf).buf);
			}
			else {
				printf ("PACCHETTO NON COMPLETO\n");
			}
		}
	}
	
	
	/* Questa parte non viene mai eseguita. Viene messa solo per correttezza formale */
	free (nsp);
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



