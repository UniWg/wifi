#include "general_include.h"

/* ------------------------------------------------------------------------- */

int appendi_pacchetto (char* dst, char* src, int nnow, int nadd) {
	int i;
	
	printf ("Appendi pacchetto %d %d\n",nnow,nadd);
	for (i=0;i<nadd;i++)	{
		dst [nnow+i] = src [i];
	}
	return (nnow+nadd);
}

/* ------------------------------------------------------------------------- */

/* ######################################################################### */
/* ######################################################################### */

void resetta_buffer_ricezione (sta_registry_t* reg) {

	/*
		deve resettare sia il buffer in cui si accumulano i dati provenienti dal mezzo
		(il buffer si considera resettato se i caratteri dei primi 3 indici sono 0 2 1)
		sia la lista che contiene i vari pacchetti di questo blocco
	*/

}

/* ------------------------------------------------------------------------- */

void imposta_tempo_occupazione_MC (int tipo,long tempo, sta_registry_t* reg ) {
	/* se tipo é _t_busy_error allora somma ad epoch attuale un tempo casuale tra 4 e 6 sec
	se tipo é _t_busy_after allora somma ad epoch attuale un tempo casuale tra 500 e 1000ms
		se tipo é _t_busy_normal allora somma il tempo indicato 
	*/
	long n, i;
    
		/* mettere a false il flag sia di in_trasmissione che di in_ricezione */
	n = rand() % 6000 + 4000;
	i = rand() % 1000 + 500;
	if (tipo == _t_busy_error) {
		(*reg).t_mc_busy += n;
	}
	else if (tipo == _t_busy_after) {
		(*reg).t_mc_busy += i;	
	}
	else if (tipo == _t_busy_normal) {
		(*reg).t_mc_busy += tempo;
	}
	
}

/* ------------------------------------------------------------------------- */

void continua_trasmissione (void) {
	/* MACRO -- blocco azzurro */
	
	/*
	if (ci_sono_dati_da_spedire_al_mezzo ()) {
		spedisci_pacchetto_a_mezzo ();
	}
	else { */
		/* Trasmissione verso il mezzo terminata con successo */
/*		resetta_buffer_trasmissione ();
		imposta_tempo_occupazione_MC (_t_busy_after,0)
	}*/
}

/* ------------------------------------------------------------------------- */

int sta_prendi_pacchetto (stato_sta_t *s, int ns, char* pack, char* buf_loc) {
	int n, tot=0;

	/* Ci interessano solo gli eventi in lettura sul nostro descrittore */
	if (FD_ISSET (stafd_g [ns],&(*s).Rset)) {

		/* Leggiamo il pacchetto arrivato */	
		do {
			n = recv (stafd_g [ns], &pack, _max_frame_buffer_size, 0);
		} while (n<0);
		
		tot = appendi_pacchetto(buf_loc, pack, tot, n);
}
	return (tot);
}
	
/* ------------------------------------------------------------------------- */

char CRC_zero (char* pack) {
	pframe_t *f;
	
	f = get_frame_buffer(pack);	/* Spacchetto il pacchetto nel frame */
	if ((*f).crc == 0) {
		return TRUE;
	}
	else return (FALSE);

}

/* ------------------------------------------------------------------------- */

int is_CTS(char* pack) {
	pframe_t *f;
	
	f = get_frame_buffer(pack);	/* Spacchetto il pacchetto nel frame */

	printf (_CGreen "STA: Pacchetto CTS ricevuto \n" _CColor_Off);
	return TRUE;
	if((*f).cts == 1) {
		printf (_CGreen "STA: Pacchetto CTS ricevuto \n" _CColor_Off);
		return TRUE;
	}
	else return FALSE;

}

/* ------------------------------------------------------------------------- */

char ricezione (sta_registry_t* reg) {

	if ((*reg).in_ricezione == TRUE) {
		return (TRUE);
	}
	else return (FALSE);
}
	
/* ------------------------------------------------------------------------- */

char trasmissione (sta_registry_t* reg) {

	if ((*reg).in_trasmissione == TRUE) {
		return (TRUE);
	}
	else return (FALSE);
}

/* ------------------------------------------------------------------------- */

char pacchetto_nostro(char* pack, int ns) {
	pframe_t *f;
	int x;
	
	f = get_frame_buffer(pack);								/* Spacchetto il pacchetto nel frame */
	x = strncmp((*f).addr1, stazione_g [ns].mac, 6);		/* metto a confronto i due indirizzi mac  */
	if (x == 0) {
		printf (_CGreen "Stazione %d -- Arrivato pacchetto \n" _CColor_Off,ns+1);
		return (TRUE);
	}
	else return (FALSE);
		
}

/* ------------------------------------------------------------------------- */

char buffer_trasmissione_vuoto(char* buf_t) { 	/* bisogna controllare se la Fifo è vuota*/
	int n;

	n = strlen(buf_t);		/* Verifico la lunghezza del buffer */

	if (n == 0) {
		return TRUE;
	}
	else return FALSE;
}

/* ------------------------------------------------------------------------- */

char buffer_allocato(char* tmp_buf) {
	int n;

	n = strlen(tmp_buf);

	if (n > 0) {			/* Se il buffer non è vuoto */
		return TRUE;
	}
	else return FALSE;

}

/* ------------------------------------------------------------------------- */

char mezzo_disponibile(sta_registry_t* reg) {
	long n;
	
	n = getNOWmsec();
	if ((*reg).t_mc_busy < n) {		/* Il mezzo è disponibile se il tempo di occupazione è minore del tempo attuale */
		return TRUE;
	}
	else return FALSE;

}

/* ------------------------------------------------------------------------- */

void spedisci_RTS(sta_registry_t* reg, stato_sta_t *s, int ns) {
	int len, nwrite, n;
	pframe_t *f = (pframe_t*) malloc (sizeof (pframe_t));
	char* fb;				 /* Frame buffer (con campo dati a zero) */
	
	
	FD_SET (stafd_g[ns], &(*s).Wset);

	/* Impostiamo i campi del frame da spedire */
	bzero (f,sizeof (pframe_t));
	(*f).data = 0;									/* frame di controllo */
	(*f).tods = 1;									/* destinato al mezzo condiviso */
	(*f).dtype = 0;
	(*f).fromds = 0;
	(*f).rts = 1;									/* ready to send */
	(*f).cts = 0;
	(*f).scan = 1;									/* scansione - utilizzata per richiedere il collegamento al mezzo */
	(*f).duration = 5;	
	(*f).packetl = _pframe_other_len;				/* Lunghezza base del pacchetto (non ci sono dati) */
	strncpy ((*f).addr1,stazione_g [1].mac,6);		/* mac6 del destinatario */
	strncpy ((*f).addr2,stazione_g [ns].mac,6);		/* mac address della stazione che sta trasmettendo */
	strncpy ((*f).addr3,"000000",6);
	strncpy ((*f).addr4,"000000",6);
	(*f).seqctrl = 1;
	(*f).buf = NULL;
	(*f).crc = _crc_ok;
	
	/* Covertiamo la struttura in array di byte */
	fb = set_frame_buffer (f);

	/* Spedizione messaggio */
	len = (*f).packetl;
	nwrite=0;

	while( (n = write(stafd_g [ns], &(fb[nwrite]), len-nwrite)) >0 ) {
		nwrite+=n;	
	}

	if(n<0) {
		char msgerror[1024];
		sprintf(msgerror, _Cerror"Stazione %d : write() failed [err %d] "_CColor_Off, ns, errno);
		perror(msgerror);
		fflush(stdout);
	}
	printf (_CGreen "STA %d : Pacchetto RTS inviato \n" _CColor_Off, (ns+1));
	(*reg).in_trasmissione = TRUE;
	(*reg).RTS = TRUE;
}

/* ------------------------------------------------------------------------- */

char spedito_RTS(sta_registry_t* reg) {
	
	if((*reg).RTS) {
		(*reg).RTS = FALSE;
		return TRUE;
	}
	else return FALSE;
}

/* ------------------------------------------------------------------------- */

void spedisci_pacchetto(char* pack, stato_sta_t *s, int ns) {
	int nwrite, len, n;
	pframe_t *f = (pframe_t*) malloc (sizeof (pframe_t));
	char* fb;				 /* Frame buffer (con campo dati a zero) */
		
	FD_SET (stafd_g[ns], &(*s).Wset);

	f = get_frame_buffer(pack);
	fb = set_frame_buffer(f);

	len = (*f).packetl;
	nwrite=0;

	while( (n = write(stafd_g [ns], &(fb[nwrite]), len-nwrite)) >0 ) {
		printf( "BYTE %d \t", n);
		nwrite+=n;	
	}


}

/* ------------------------------------------------------------------------- */
void reset_indice(void) {}
void reset_buffer(void) {}

void spedisci_CTS(void) {}


int is_ACK(sta_registry_t* reg) {return 0;}
int is_RTS(sta_registry_t* reg) {return 0;}


void aggiungi_pacchetto(char* pack) {}

void aggiorna_MC(void) {}

int scaduto_timeout_ACK(void) {return 0;}


void prepara_buffer(void){}

/* ######################################################################### */
/* ######################################################################### */




