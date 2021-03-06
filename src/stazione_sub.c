#include "general_include.h"

/* ------------------------------------------------------------------------- */


long randomTime (long min,long max) {
	return ((rand() % min)+(max-min));
}

/* ------------------------------------------------------------------------- */

char pacchetto_completo (int len, char* tmp_pack, sta_registry_t* reg) {
	int i;
	
	/* Appendiamo i dati arrivati a quelli giá esistenti in BLR */
	for (i=0;i<len;i++) {
		(*reg).BLR [(*reg).nBLR+i] = tmp_pack [i];
	}

	/* Aggiorniamo la dimensione del pacchetto */
	(*reg).nBLR += len;
	
	
	/* Restituisce TRUE se il pacchetto é completo */
	return (complete_frame ((*reg).nBLR,(*reg).BLR));
	
}

/* ------------------------------------------------------------------------- */

void imposta_tempo_occupazione_MC (int tipo,long tempo, sta_registry_t* reg ) {
	/* se tipo é _t_busy_error allora somma ad epoch attuale un tempo casuale tra 4 e 6 sec
	se tipo é _t_busy_after allora somma ad epoch attuale un tempo casuale tra 500 e 1000ms
		se tipo é _t_busy_normal allora somma il tempo indicato 
	*/
	long epoch = getNOWmsec ();
	long newtmp;
    
	switch (tipo) {
		case (_t_busy_error) : {
			long r = (rand() % 4000)+2000;		
			newtmp = epoch + r;
			break;
		}
		case (_t_busy_after) : {
			long r = (rand() % 500)+500;		
			newtmp = epoch + r;
			break;
		}
		case (_t_busy_normal) : {
			newtmp = epoch + tempo;
			break;
		}
	}

	/* Aggiorniamo il tempo di occupazione del mezzo solo se il valore attuale é inferiore a quello calcolato ora */
	if ((*reg).t_mc_busy < newtmp) {
		(*reg).t_mc_busy = newtmp;
	}	
	
}

/* ------------------------------------------------------------------------- */

void continua_trasmissione (sta_registry_t* reg) {
	/* MACRO -- blocco azzurro */
	
	
	if (ci_sono_dati_da_spedire_al_mezzo(reg)) {
		from_LTT_to_BLT(reg);
	}
	else { 
		/* Trasmissione verso il mezzo terminata con successo */
		reset_buffer_trasmissione(reg);
		imposta_tempo_occupazione_MC (_t_busy_after, 0, reg);
	}
}

/* ------------------------------------------------------------------------- */

char ci_sono_dati_da_spedire_al_mezzo (sta_registry_t* reg){
	char tmp_buf [_max_frame_buffer_size];

	/* Controlliamo se esiste l'elemento successivo */
	if (fifo_read_deep ((*reg).LTT,tmp_buf,(*reg).nBLT+1)) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/* ------------------------------------------------------------------------- */

void reset_buffer_trasmissione(sta_registry_t* reg) {
	pframe_t *p;
	char pack [_max_frame_buffer_size];
	int seqctrl;

	/* Cancelliamo gli elementi spediti da LTL */
	pthread_mutex_lock (&mutexLTL [(*reg).ns]);

	do {
		fifo_pop (LTL [(*reg).ns],pack);	/* Estraiamo un elemento ... */
		
		p = get_frame_buffer (pack);			/* ... e ci fermiamo quando il numero di sequenza è = 1 */
		seqctrl=(*p).seqctrl;
		free (p);
	} while (seqctrl!=1);
	
	pthread_mutex_unlock (&mutexLTL [(*reg).ns]);

	/* Cancelliamo tutti gli elementi della LTT ... */
	fifo_reset ((*reg).LTT);
	
	/* ... e ci togliamo dallo stato di trasmissione */
	(*reg).in_trasmissione = FALSE;
	(*reg).RTS = FALSE;
}

/* ------------------------------------------------------------------------- */

int sta_prendi_pacchetto (stato_sta_t *s, char* tmp_pack, sta_registry_t* reg) {
	int n, ns;

	ns = (*reg).ns;
		
	/*printf("STA %d : IN LETTURA  \n", ns+1);*/

	/* Leggiamo il pacchetto arrivato */	
	do {
		n = recv (stafd_g [ns], tmp_pack, _max_frame_buffer_size, 0);
	} while ((n<0) && (errno==EINTR));
	
	/* Appena iniziamo a formare il pacchetto settiamo il timeout  */
	if ((*reg).nBLR == 0) {
		(*reg).timePACK = getNOWmsec () + randomTime (_t_timer_delay_min,_t_timer_delay_max);
	}

	return (n);
}
	
/* ------------------------------------------------------------------------- */

char CRC_zero (sta_registry_t* reg) {
	pframe_t *f;
		
	f = get_frame_buffer((*reg).BLR);	/* Spacchetto il pacchetto nel frame */
	if ((*f).crc == 0) {
		#if STA_DEBUG == 1
			printf (_Csta "STA %d : Ricevuto pacchetto con CRC = 0\n"_CColor_Off ,(*reg).ns+1); 
		#endif
		free (f);
		return TRUE;
	}
	else {
	free (f);
	return (FALSE);
	}

}

/* ------------------------------------------------------------------------- */

char is_CTS(sta_registry_t* reg) {
	int ns;
	pframe_t *f;

	ns = (*reg).ns;
	
	f = get_frame_buffer((*reg).BLR);	/* Spacchetto il pacchetto nel frame */

	if((*f).cts == 1) {
	#if STA_DEBUG == 1
		printf (_Csta "STA %d : Pacchetto CTS ricevuto \n" _CColor_Off, ns+1);
	#endif
		(*reg).nBLR = 0; 
		(*reg).timeCTS = 0;		/* CTS ricevuto. Resettiamo il timeout */
		free (f);
		return TRUE;
	}
	else {
	free (f);
	return FALSE;
	}
}

/* ------------------------------------------------------------------------- */

char is_RTS(sta_registry_t* reg) {
	int ns;
	pframe_t *f;
	char* pack;

	ns = (*reg).ns;
	pack = (*reg).BLR;
	
	f = get_frame_buffer(pack);	/* Spacchetto il pacchetto nel frame */

	if((*f).rts == 1) {
	#if STA_DEBUG == 1
		printf (_Csta "STA %d : Pacchetto RTS ricevuto \n" _CColor_Off, ns+1);
	#endif
		free (f);
		return TRUE;
	}
	else {
	free (f);	
	return FALSE;
	}
}

/* ------------------------------------------------------------------------- */
char is_ACK (sta_registry_t* reg) {
	pframe_t *p;
	int ns;

	/* Spacchettiamo il buffer locale di ricezione ... */
	p = get_frame_buffer ((*reg).BLR);
	ns = (*reg).ns;

	if ((*p).seqctrl == _ack_value) {
		#if STA_DEBUG == 1
		 	printf (_Csta"STA %d : ricevuto ACK\n"_CColor_Off ,ns+1); 
		#endif
		(*reg).nBLR = 0;
		(*reg).timeACK = 0;			/* ACK ricevuto. Resettiamo il timeout */
		free (p);
		return (TRUE);
	}
	else {
		free (p);
		return (FALSE);
	}
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
	x = strncmp((*f).addr1, stazione_g [ns].mac, 6);				/* metto a confronto i due indirizzi mac  */
	if (x == 0) {
	#if STA_DEBUG == 1
		printf (_Csta "STA %d : Arrivato pacchetto \n" _CColor_Off,ns+1);
	#endif
		free (f);
		return (TRUE);
	}
	else {
	free (f);		
	return (FALSE);
	}
}

/* ------------------------------------------------------------------------- */

char buffer_trasmissione_vuoto(sta_registry_t* reg) { 	/* bisogna controllare se la Fifo è vuota*/
	int ns;
	char ris = FALSE;

	ns = (*reg).ns;

		/* Controlliamo se ci sono elementi nella lista di trasmissione locale */
	pthread_mutex_lock (&mutexLTL [ns]);
	ris = fifo_is_empty (LTL [ns]);			/* TRUE se la lista é vuota */
	pthread_mutex_unlock (&mutexLTL [ns]);

	if (ris) {
		return TRUE;
	}
	else {
#if STA_DEBUG == 1
	printf("Fifo di %d non vuota \n", ns+1);		
#endif
	return FALSE;
	}
}

/* ------------------------------------------------------------------------- */

char buffer_allocato(sta_registry_t* reg) {
	int x;

	x = (*reg).ns;


	if (fifo_is_empty((*reg).LTT)) {			/* Se la lista temporanea di trasmissione è vuota */
		return FALSE;
	}
	else {
#if STA_DEBUG == 1
	printf("Lista tmp di %d non vuota \n", x+1);
#endif
	return TRUE;
	}
}

/* ------------------------------------------------------------------------- */

void from_LTL_to_LTT(sta_registry_t* reg){
	int ns, x;
	pframe_t *p;
	char pack [_max_frame_buffer_size];
	int seqctrl;

	/* Dobbiamo trasferire una sequenza dalla LTL alla LTT senza cancellare gli elementi da LTL */
	/* Un blocco finisce quando il numero regressivo del pacchetto raggiunge 1 */

	ns = (*reg).ns;

	pthread_mutex_lock (&mutexLTL [ns]);

	x = 1;
	do{ 
		fifo_read_deep(LTL[ns], pack, x);	/*  Dato che c'è qualcosa da leggere lo trasferisco nella lista temporanea di trasmissione*/

		fifo_push((*reg).LTT, pack);

		p = get_frame_buffer (pack);
		seqctrl=(*p).seqctrl;
		free (p);
		x++;
	} while (seqctrl!=1);
	
	pthread_mutex_unlock (&mutexLTL [(*reg).ns]);		

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
	pframe_t *g;			
	char* fb;				 /* Frame buffer (con campo dati a zero) */
	char pack[_max_frame_buffer_size];	
	int x;

	ns = (*reg).ns;
	
	fifo_read ((*reg).LTT, pack);
	g = get_frame_buffer(pack);

	x = mac2nsta((*g).addr1);

	fb = pacchetto_di_controllo(_rts_packet_type,(*g).addr2,(*g).addr1);

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

#if STA_DEBUG == 1
	printf (_Csta "STA %d a STA %d : Pacchetto RTS inviato \n" _CColor_Off, ns+1, x);
#endif

	free (g);
	free (f);

	/* Ci mettiamo in trasmissione */
	(*reg).in_trasmissione = TRUE;
	(*reg).in_ricezione = FALSE;
	(*reg).RTS = TRUE;
	(*reg).nBLT = 0;					/* Azzeriamo il numero (relativamente alla sequenza) di pacchetti trasmessi */
	(*reg).timeCTS = getNOWmsec () + randomTime (_t_timer_delay_min,_t_timer_delay_max);	/* Settiamo il timeout per la ricezione del CTS */
}

/* ------------------------------------------------------------------------- */

void spedisci_CTS(sta_registry_t* reg, stato_sta_t *s) {
	int len, nwrite, n, ns;
	pframe_t *f = (pframe_t*) malloc (sizeof (pframe_t));	
	pframe_t *g;			
	char* fb;				 /* Frame buffer (con campo dati a zero) */	
	int x;

	ns = (*reg).ns;
	FD_SET (stafd_g[ns], &(*s).Wset);
	
	/*fifo_read ((*reg).LTT, pack);*/
	g = get_frame_buffer((*reg).BLR);

	x = mac2nsta((*g).addr2);

	fb = pacchetto_di_controllo(_cts_packet_type,(*g).addr1,(*g).addr2);

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

#if STA_DEBUG == 1
	printf (_Csta "STA %d a STA %d: Pacchetto CTS inviato \n" _CColor_Off, ns+1, x);
#endif
	free (g);
	free (f);

	/* Ci mettiamo in ricezione */
	(*reg).in_trasmissione = FALSE;
	(*reg).in_ricezione = TRUE;
}

/* ------------------------------------------------------------------------- */

void spedisci_ACK(sta_registry_t* reg, char ultimo){
	pframe_t* f;
	char *fb;
	int len, nwrite, x, ns, n;

	/* Ricaviamo mittente e destinatario dal BLR (sono invertiti rispetto il pacchetto che si trova nel BLR) */
	f = get_frame_buffer((*reg).BLR);

	ns = (*reg).ns;
	x = mac2nsta((*f).addr2);
		
	fb = pacchetto_di_controllo(_ack_packet_type,(*f).addr1,(*f).addr2);

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

#if STA_DEBUG == 1
	printf (_Csta "STA %d a STA %d: Pacchetto ACK inviato \n" _CColor_Off, ns+1, x);
#endif

	/* Se abbiamo ricevuto tutti i pacchetti, allora non siamo più in ricezione */
	if (ultimo == TRUE) {
		(*reg).in_ricezione = FALSE;
	}
	free (f);


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

void from_LTT_to_BLT(sta_registry_t* reg) {
	int nwrite, len, n, ns;		
	char BLT [_max_frame_buffer_size];	
	pframe_t *p;
	int x;											

	ns = (*reg).ns;
	(*reg).nBLT++;	


	if (fifo_read_deep((*reg).LTT, BLT, (*reg).nBLT)) {		/* Prendiamo il pacchetto da LTT */

		p = get_frame_buffer(BLT);
		x = mac2nsta((*p).addr1);
		len = get_packet_len(BLT);
		nwrite=0;

		while( (n = write(stafd_g [ns], &(BLT[nwrite]), len-nwrite)) >0 ) {
			nwrite+=n;	
		}
#if STA_DEBUG == 1
	printf (_Csta "STA %d a STA %d: Pacchetto dati inviato \n" _CColor_Off, ns+1, x);
#endif

	}

}

/* ------------------------------------------------------------------------- */

void aggiorna_MC(sta_registry_t* reg) {
	long i;
	pframe_t *f;

	f = get_frame_buffer((*reg).BLR);
	/* Ricavo la durata del pacchetto e la confronto con i due valori possibili */
	if ((*f).duration == _packet_value_low)  {
		i = _packet_duration_low; 			/* 500 msec */
	}
	else {
		i = _packet_duration_hi;			/* 2000 msec */
	}

	imposta_tempo_occupazione_MC(_t_busy_normal, i, reg);
	free(f);
	
}

/* ------------------------------------------------------------------------- */

char scaduto_timeout_PACK (sta_registry_t* reg) {
	long now = getNOWmsec ();

	/* Recupera le situazioni critiche (dove il successivo controllo non ha avuto effetto) */ 
	if ((*reg).nPACK > _timeout_consecutivi) {	
		(*reg).nPACK = 0;
		(*reg).timePACK = 0;
		(*reg).t_mc_busy =0;
		(*reg).nBLR = 0;
		return (TRUE);
	}
	
	if (((*reg).timePACK > 0) && ((*reg).timePACK < now))	{ 
		#if STA_DEBUG == 1
			printf (_Csta"STA %d: scaduto PACK\n"_CColor_Off ,(*reg).ns+1);			/* Scaduto tempo di composizione del pacchetto */
		#endif
		
		(*reg).nPACK++;
		return (TRUE);
	}		
	
	return (FALSE);


}

/* ------------------------------------------------------------------------- */

char scaduto_timeout_CTS(sta_registry_t* reg) {
	long now = getNOWmsec ();
	
	if (((*reg).timeCTS > 0) && ((*reg).timeCTS < now))	{ 
		#if STA_DEBUG == 1
			printf (_Csta"STA %d: scaduto CTS\n"_CColor_Off ,(*reg).ns+1);			/* Scaduto tempo del CTS */
		#endif
		
		(*reg).timeCTS = 0;			/* Reset timeout del CTS */
		return (TRUE);
	}		
	
	return (FALSE);

}

/* ------------------------------------------------------------------------- */

char scaduto_timeout_ACK(sta_registry_t* reg) {
	long now = getNOWmsec ();
	
	if (((*reg).timeACK > 0) && ((*reg).timeACK < now))	{ 
		#if STA_DEBUG == 1
			printf (_Csta"STA %d: scaduto ACK\n"_CColor_Off ,(*reg).ns+1);			/* Scaduto tempo dell'ACK */
		#endif
		
		(*reg).timeACK = 0;			/* Reset timeout dell'ACK */
		(*reg).nBLR = 0;
		return (TRUE);
	}		
	
	return (FALSE);

}

/* ------------------------------------------------------------------------- */

void reset_parametri(sta_registry_t* reg, char force) {

	if ((force == TRUE) || ((*reg).t_mc_busy < getNOWmsec ())) {
		(*reg).in_trasmissione = FALSE;
		(*reg).in_ricezione = FALSE;
		(*reg).RTS = FALSE;
		fifo_reset ((*reg).LTT);
		fifo_reset ((*reg).LTR);
		(*reg).nBLR = 0;
		(*reg).nBLT = 0;
	}
}

/* ------------------------------------------------------------------------- */

void aggiungi_pacchetto(sta_registry_t* reg) {
	char ultimo_pacchetto = FALSE;

	if (dato_da_scartare(reg)) {	
		ultimo_pacchetto = TRUE;
	}
	else {
	
		from_BLR_to_LTR(reg);
		
		if (seqctrl_uguale_1(reg)) {		/* Se la funzione è vera vuol dire che abbiamo ricevuto l'ultimo pacchetto della sequenza */

			from_LTR_to_LRL(reg);
			ultimo_pacchetto = TRUE;
		}
	
	}

	spedisci_ACK(reg, ultimo_pacchetto);
}

/* ------------------------------------------------------------------------- */

char dato_da_scartare (sta_registry_t* reg) {
	pframe_t *f;
	int x, crc;
	char ris;

	/* Siccome il pacchetto viene considerato spedito anche se non si riceve l'ack,
		allora verifichiamo il numero progressivo del messaggio per evitare di stampare dei doppioni.
		Il numero è stato messo impropriamente nel crc per comodità */
	
	f = get_frame_buffer ((*reg).BLR);
	x = mac2nsta((*f).addr2);
	crc = (*f).crc;


	/* printf("CRC = %d \n", crc); */

	
	if ((*reg).lastProg[x-1] == crc) {			/* Il pacchetto ha lo stesso numero del precedente. Lo scartiamo */
		ris = TRUE;
	}
	else {
		(*reg).lastProg[x-1] = crc;				/* Aggiorniamo l'ultimo numero di sequenza */
		ris = FALSE;
	}

	free(f);

	return (ris);
}

/* ------------------------------------------------------------------------- */

void from_BLR_to_LTR(sta_registry_t* reg){

	/* Inseriamo il pacchetto ricevuto nella lista temporanea di ricezione ... */
	fifo_push ((*reg).LTR,(*reg).BLR);
	/* ... ed eliminiamo il pacchetto */
	(*reg).nBLR = 0;

}

/* ------------------------------------------------------------------------- */

char seqctrl_uguale_1(sta_registry_t* reg){
	pframe_t *f;

	f = get_frame_buffer((*reg).BLR);	/* La dimensione é stata azzerata (from_BLR_to_LTR), ma i dati del pacchetto esistono ancora */

	/* Controlliamo il numero regressivo */

	if ((*f).seqctrl == 1) {
		free(f);
		return TRUE;
	}
	else {
		free(f);
		return FALSE;
	}
		
}

/* ------------------------------------------------------------------------- */

void from_LTR_to_LRL(sta_registry_t* reg){
	char pack [_max_frame_buffer_size];


	pthread_mutex_lock (&mutexLRL [(*reg).ns]);
	
	while (fifo_pop ((*reg).LTR,pack) == TRUE) {		/* Al termine la LTR risulta vuota */
		fifo_push (LRL [(*reg).ns],pack);
	}
	
	pthread_mutex_unlock (&mutexLRL [(*reg).ns]);

	
	/* Abbiamo terminato la ricezione dell'intera sequenza */
}

/* ------------------------------------------------------------------------- */

/* ######################################################################### */
/* ######################################################################### */

