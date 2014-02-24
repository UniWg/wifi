#include "general_include.h"

/* ------------------------------------------------------------------------- */

int appendi_pacchetto (char* dst, char* src, int nnow, int nadd) {
	int i, x, y;
	pframe_t *f;
	
	f = get_frame_buffer(src);
	y = mac2nsta((*f).addr2);
	x = mac2nsta((*f).addr1);
	printf ("da STA %d a STA %d: Appendi pacchetto %d %d\n",y,x,nnow,nadd);
	for (i=0;i<nadd;i++)	{
		dst [nnow+i] = src [i];
	}
	return (nnow+nadd);
}

/* ------------------------------------------------------------------------- */

char frame_completo (int n, sta_registry_t* reg) {
	int x;
	pframe_t *f;

	if (n<15)				/* Dobbiamo avere sufficienti caratteri per leggere la len del pacchetto */
		return (FALSE); 	
		

	if (get_packet_len ((*reg).BLR) == n){
		f = get_frame_buffer((*reg).BLR);
		x = mac2nsta((*f).addr1);
		printf (_Csta "STA %d: Pacchetto completo ricevuto \n" _CColor_Off, x);
		return (TRUE);
	}
	else
		return (FALSE);
	
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
	/* metto a false il flag sia di in_trasmissione che di in_ricezione */
	(*reg).in_trasmissione = FALSE;
	(*reg).in_ricezione = FALSE;
	(*reg).RTS = FALSE;	
	
}

/* ------------------------------------------------------------------------- */

void continua_trasmissione (void) {
	/* MACRO -- blocco azzurro */
	
	/*
	if (ci_sono_dati_da_spedire_al_mezzo ()) {
		spedisci_pacchetto ();
	}
	else { */
		/* Trasmissione verso il mezzo terminata con successo */
/*		resetta_buffer_trasmissione ();
		imposta_tempo_occupazione_MC (_t_busy_after,0)
	}*/
}

/* ------------------------------------------------------------------------- */

int sta_prendi_pacchetto (stato_sta_t *s, sta_registry_t* reg) {
	int n, ns;
	char* fb;				/* struttura d'appoggio */
	
	ns = (*reg).ns;
	fb = (char *) malloc (_max_frame_buffer_size * sizeof(char));
		
	/*printf("qui %d \t", ns);*/
	if (FD_ISSET (stafd_g [ns], &(*s).Rset)) {
		/* Leggiamo il pacchetto arrivato */	
		do {
			n = recv (stafd_g [ns], fb, _max_frame_buffer_size, 0);
		} while ((n<0) && (errno==EINTR));
		
		(*reg).x = appendi_pacchetto((*reg).BLR, fb, (*reg).x, n);
	}
	return ((*reg).x);
}
	
/* ------------------------------------------------------------------------- */

char CRC_zero (sta_registry_t* reg) {
	pframe_t *f;
	char* pack;

	pack = (*reg).BLR;			
	f = get_frame_buffer(pack);	/* Spacchetto il pacchetto nel frame */
	if ((*f).crc == 0) {
		return TRUE;
	}
	else return (FALSE);

}

/* ------------------------------------------------------------------------- */

int is_CTS(sta_registry_t* reg) {
	int ns;
	pframe_t *f;
	char* pack;

	ns = (*reg).ns;
	pack = (*reg).BLR;
	
	f = get_frame_buffer(pack);	/* Spacchetto il pacchetto nel frame */

	if((*f).cts == 1) {
		printf (_Csta "STA %d: Pacchetto CTS ricevuto \n" _CColor_Off, ns+1);
		return TRUE;
	}
	else return FALSE;

}

/* ------------------------------------------------------------------------- */

int is_RTS(sta_registry_t* reg) {
	int ns;
	pframe_t *f;
	char* pack;

	ns = (*reg).ns;
	pack = (*reg).BLR;
	
	f = get_frame_buffer(pack);	/* Spacchetto il pacchetto nel frame */

	if((*f).rts == 1) {
		printf (_Csta "STA %d: Pacchetto RTS ricevuto \n" _CColor_Off, ns+1);
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

char pacchetto_nostro(sta_registry_t* reg) {
	pframe_t *f;
	int x, ns;

	ns = (*reg).ns;

	f = get_frame_buffer((*reg).BLR);								/* Spacchetto il pacchetto nel frame */
	x = strncmp((*f).addr1, stazione_g [ns].mac, 6);		/* metto a confronto i due indirizzi mac  */
	if (x == 0) {
		printf (_Csta "Stazione %d -- Arrivato pacchetto \n" _CColor_Off,ns+1);
		return (TRUE);
	}
	else return (FALSE);
		
}

/* ------------------------------------------------------------------------- */

char buffer_trasmissione_vuoto(sta_registry_t* reg) { 	/* bisogna controllare se la Fifo è vuota*/
	int n;
	char* buf_loc;

	buf_loc = (*reg).BLT;	


	n = strlen(buf_loc);		/* Verifico la lunghezza del buffer */
	/*printf("%d \n", n);*/
	if (n == 0) {
		return TRUE;
	}
	else return FALSE;
}

/* ------------------------------------------------------------------------- */

char buffer_allocato(sta_registry_t* reg) {
	list2 *p;
	
	p = (*reg).LTT;

	if (fifo_is_empty(p)) {			/* Se la lista temporanea di trasmissione è vuota */
		return FALSE;
	}
	else return TRUE;

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

void spedisci_RTS(sta_registry_t* reg, stato_sta_t *s) {
	int len, nwrite, n, ns;
	pframe_t *f = (pframe_t*) malloc (sizeof (pframe_t));	
	pframe_t *g = (pframe_t*) malloc (sizeof (pframe_t));			
	char* fb;				 /* Frame buffer (con campo dati a zero) */
	list2 *p;	

	ns = (*reg).ns;
	FD_SET (stafd_g[ns], &(*s).Wset);
	
	p = (*reg).LTT;
	g = get_frame_buffer((*p).pack);

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
	strncpy ((*f).addr1, (*g).addr1,6);				/* mac6 del destinatario */
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
		sprintf(msgerror, _Cerror"Stazione %d : write() failed [err %d] "_CColor_Off, ns+1, errno);
		perror(msgerror);
		fflush(stdout);
	}
	printf (_Csta "STA %d : Pacchetto RTS inviato \n" _CColor_Off, ns+1);
	(*reg).in_trasmissione = TRUE;
	(*reg).in_ricezione = FALSE;
	(*reg).RTS = TRUE;
}

/* ------------------------------------------------------------------------- */

void spedisci_CTS(sta_registry_t* reg, stato_sta_t *s) {
	int len, nwrite, n, ns;
	pframe_t *f = (pframe_t*) malloc (sizeof (pframe_t));	
	pframe_t *g;			
	char* fb;				 /* Frame buffer (con campo dati a zero) */
	list2 *p;	

	ns = (*reg).ns;
	FD_SET (stafd_g[ns], &(*s).Wset);
	
	p = (*reg).LTT;
	g = get_frame_buffer((*p).pack);

	/* Impostiamo i campi del frame da spedire */
	bzero (f,sizeof (pframe_t));
	(*f).data = 0;									/* frame di controllo */
	(*f).tods = 1;									/* destinato al mezzo condiviso */
	(*f).dtype = 0;
	(*f).fromds = 0;
	(*f).rts = 0;									
	(*f).cts = 1;									/* clear to send */ 
	(*f).scan = 1;									/* scansione - utilizzata per richiedere il collegamento al mezzo */
	(*f).duration = 5;	
	(*f).packetl = _pframe_other_len;				/* Lunghezza base del pacchetto (non ci sono dati) */
	strncpy ((*f).addr1, (*g).addr1,6);				/* mac6 del destinatario */
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
		sprintf(msgerror, _Cerror"Stazione %d : write() failed [err %d] "_CColor_Off, ns+1, errno);
		perror(msgerror);
		fflush(stdout);
	}
	printf (_Csta "STA %d : Pacchetto CTS inviato \n" _CColor_Off, ns+1);
	(*reg).in_trasmissione = TRUE;
	(*reg).in_ricezione = FALSE;
	(*reg).RTS = FALSE;


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

void spedisci_pacchetto(sta_registry_t* reg, stato_sta_t *s) {
	int nwrite, len, n, ns;
	pframe_t *f = (pframe_t*) malloc (sizeof (pframe_t));
	char* buf;				/*  */ 								
	list2 *p;						

	ns = (*reg).ns;	
	p = (*reg).LTT;
	buf = (*reg).BLT;

	if (fifo_read(p, buf)) {		/* Se c'è un elemento nella lista lo metto nel buffer di trasmissione e spedisco */

		/*FD_SET (stafd_g[ns], &(*s).Wset);*/

		f = get_frame_buffer(buf);

		len = (*f).packetl;
		nwrite=0;

		while( (n = write(stafd_g [ns], &(buf[nwrite]), len-nwrite)) >0 ) {
			printf( "BYTE %d \t", n);
			nwrite+=n;	
		}
	}

}

/* ------------------------------------------------------------------------- */

void aggiorna_MC(sta_registry_t* reg) {
	long i, x;
	pframe_t *f;

	f = get_frame_buffer((*reg).BLT);
	x = (*f).duration;
	i = getNOWmsec() + x;
	

	imposta_tempo_occupazione_MC(_t_busy_normal, i, reg);
	
}

/* ------------------------------------------------------------------------- */
void reset_indice(void) {}
void reset_buffer(void) {}



int is_ACK(sta_registry_t* reg) {return 0;}



void aggiungi_pacchetto(sta_registry_t* reg) {}



int scaduto_timeout_ACK(void) {return 0;}

void prepara_buffer(void){}


/* ######################################################################### */
/* ######################################################################### */




