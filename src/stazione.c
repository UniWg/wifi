#include "general_include.h"

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
	}

	free (mac);
}

/* ------------------------------------------------------------------------- */
void* main_sta_thread (void* nsp) {
	int len,n,nwrite,ns = *(int*)nsp-1;	/* togliamo 1 così ns è allineato con l'indice dell'array */
	char mac [18];
	pframe_t f;
	pframe_t* pf;
	char* fb;
	char* messaggio_test = "Messaggio di test";
	char buf [_max_frame_buffer_size];
	
	/* Attendiamo un paio di secondi in modo da dare il tempo al mezzo condiviso 
		di mettersi in ascolto. Dopodichè possiamo effettuare la connessione anche noi */
	sleep (2);
	
	
	
	
	
	/* TEST spedizione -- da CANCELLARE ################################### */
	packet_test (ns);
	
	if (ns == 1) {
		sleep (1);
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

		/* Covertiamo la struttura in array di byte */
		fb = set_frame_buffer ((pframe_t*)&f);

		/* Spedizione messaggio */
		len = f.packetl;
		nwrite=0;
	
		while( (n = write(stafd_g [ns], &(fb[nwrite]), len-nwrite)) >0 )
			nwrite+=n;
		
	} else sleep (3);
	
	/* Leggiamo il pacchetto arrivato 
		lettura e scrittura dovreanno essere gestite con una select */
	
	do {
		n = recv (stafd_g [ns],buf,_max_frame_buffer_size,0);
	} while ((n<0) && (errno==EINTR));
	
	/* Spacchettiamo e vediamo se è destinato a noi */
	pf = get_frame_buffer (buf);
	
	if (mac2nsta ((*pf).addr1) == (ns + 1)) 
		printf (_Csta "Stazione %d -- ricevuto messaggio : %s\n" _CColor_Off,ns+1,(*pf).buf);
	
	
	/* FINE TEST ########################################################## */
	
	
	
	
	
	while (1) {
		str2mac (stazione_g [ns].mac,mac);
		printf (_Csta "Stazione %d -- MAC: %s\n" _CColor_Off,ns+1,mac);
		sleep (ns+1);
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



