#include "general_include.h"

const int _campo_stax [] = {_campo_sta1,_campo_sta2,_campo_sta3,_campo_sta4};
const int _sta_di_stax [] = {_sta_di_sta1,_sta_di_sta2,_sta_di_sta3,_sta_di_sta4};
const char _mac_stax [][17] = {_mac_sta1,_mac_sta2,_mac_sta3,_mac_sta4};


/* ------------------------------------------------------------------------- */
void packet_test (int ns) {
	/* funzione di prova da CANCELLARE */
	/* Ci colleghiamo al mezzo condiviso e gli spediamo un pacchetto */
	sain_t mezzo;
	int ris,n, nwrite, len;
	char* msg = "messaggio di test";
	
	/* Apriamo il socket -- IPV4, TCP */
	if ((stafd_g [ns] = socket (AF_INET,SOCK_STREAM,0))<0) {
		printf (_Cerror "Stazione : Errore nell'apertura del socket\n" _CColor_Off);
		exit (-1);
	}
	DEBUG_STA "STA: Socket connesso\n" END_MC
	
	
	/* per completezza qui bisogna aggiungere la bind */
	
	/* assign our destination address */
	memset ( &mezzo, 0, sizeof(mezzo) );
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
	
	/* Spedizione messaggio */
	len = strlen(msg)+1;
	nwrite=0;
	
	/* ORA DOBBIAMO TENTARE DI MANDARE UN FRAME 802.11 */
	/* E POI BISOGNA SIMULARE UN TENTATIVO DI CONNESSIONE */
	
	while( (n = write(stafd_g [ns], &(msg[nwrite]), len-nwrite)) >0 )
		nwrite+=n;
	if(n<0) {
		char msgerror[1024];
		sprintf(msgerror,"Stazione :  write() failed [err %d] ",errno);
		perror(msgerror);
		fflush(stdout);
	}
}

/* ------------------------------------------------------------------------- */
void mac2str (const char* mac,char* asc) {
	int i,j;
	unsigned int n;
	char u [3];
	
	u [2] = 0;	/* terminatore */
	j = 0;
	for (i=0;i<16;i+=3) {
		/* Prendiamo due caratteri (un numero hex) ... */
		u [0] = mac [i];
		u [1] = mac [i+1];
		/* ... li convertiamo prima in numero ... */
		sscanf (u,"%x",&n);
		/* ... e poi in ascii */
		asc [j] = (char) n; j++;
	}
}

/* ------------------------------------------------------------------------- */
void str2mac (char* asc,char* mac) {
	/* restituisce una stringa ben formata, quindi anche con il terminatore */
	sprintf (mac,"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
		(int)asc[0],(int)asc[1],(int)asc[2],(int)asc[3],(int)asc[4],(int)asc[5]);	
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
	int ns = *(int*)nsp-1;	/* togliamo 1 così ns è allineato con l'indice dell'array */
	char mac [18];
	
	/* Attendiamo un paio di secondi in modo da dare il tempo al mezzo condiviso 
		di mettersi in ascolto. Dopodichè possiamo effettuare la connessione anche noi */
	sleep (2);
	
	packet_test (ns);
	
	while (1) {
		str2mac (stazione_g [ns].mac,mac);
		printf ("Stazione %d -- MAC: %s\n",ns+1,mac);
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



