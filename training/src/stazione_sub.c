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

void checkUp (sta_registry_t* reg) {
	/* Non possiamo essere contemporaneamente in trasmissione e ricezione */ 
	if (((*reg).in_trasmissione == TRUE) && ((*reg).in_ricezione == TRUE)) {
		printf (_Cerror "STA%d - Siamo contemporaneamente in trasmissione e ricezione\n" _CColor_Off,(*reg).ns+1);
		exit (1);
	}
	
	/* Non possiamo non essere in trasmissione e avere dei dati nella LTT */
/*	if (((*reg).in_trasmissione == FALSE) && (fifo_is_empty ((*reg).LTT) == FALSE)) {
		printf (_Cerror "STA%d - Non siamo in trasmissione ma abbiamo dei dati in LTT\n" _CColor_Off,(*reg).ns+1);
		exit (1);
	}*/
}

/* ------------------------------------------------------------------------- */
long randomTime (long min,long max) {
	return ((rand() % min)+(max-min));
}

/* ------------------------------------------------------------------------- */
void imposta_tempo_occupazione_MC (int tipo,long tempo,sta_registry_t* reg) {
	long epoch = getNOWmsec ();
	long newtmp;

	/* se tipo é _t_busy_error allora somma ad epoch attuale un tempo casuale tra 4 e 6 sec
	se tipo é _t_busy_after allora somma ad epoch attuale un tempo casuale tra 500 e 1000ms
		se tipo é _t_busy_normal allora somma il tempo indicato 
	*/
	
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
		
		/*if (tipo == _t_busy_normal)
		printf ("STA%d mezzo occupato fino a %ld -- now %ld ++ %ld\n",(*reg).ns+1,newtmp,epoch,tempo);		*/
	}
}

/* ------------------------------------------------------------------------- */
void continua_trasmissione (sta_registry_t* reg) {
	/* MACRO -- blocco azzurro */
	
	if (ci_sono_ancora_dati_nella_sequenza (reg) == TRUE) {
		spedisci_BLT (reg);			/* Spediamo prossimo pacchetto della sequenza */
	}
	else { 
		/* Trasmissione verso il mezzo terminata con successo */
		resetta_buffer_trasmissione (reg);
		imposta_tempo_occupazione_MC (_t_busy_after,0,reg);
	}
}

/* ------------------------------------------------------------------------- */
char ci_sono_ancora_dati_nella_sequenza (sta_registry_t* reg) {
	char buftmp [_max_frame_buffer_size];
	
	/* Controlliamo se esiste l'elemento successivo */
	return (fifo_read_deep ((*reg).LTT,buftmp,(*reg).nBLT+1));
}

/* ------------------------------------------------------------------------- */
void resetta_buffer_trasmissione (sta_registry_t* reg) {
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
int sta_prendi_pacchetto (stato_sta_t* s,char* tmp_pack,sta_registry_t* reg) {
	int n;

	/* Prendiamo il pacchetto dal mezzo */
	do {
		n = recv (stafd_g [(*reg).ns],tmp_pack,_max_frame_buffer_size,0);
	} while ((n<0) && (errno==EINTR));

	/* Appena iniziamo a formare il pacchetto settiamo il timeout  */
	if ((*reg).nBLR == 0) {
		(*reg).timePACK = getNOWmsec () + randomTime (_t_timer_delay_min,_t_timer_delay_max);
	}

	return (n);
}

/* ------------------------------------------------------------------------- */
char pacchetto_completo (char* tmp_pack,int len,sta_registry_t* reg) {
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
char CRC_uguale_0 (sta_registry_t* reg) {
	pframe_t *p;
	
	p = get_frame_buffer ((*reg).BLR);
	if ((*p).crc == 0) {
		#if STA_DEBUG == 1
			printf (_CIRed "STA%d ricevuto pacchetto con CRC = 0\n"_CColor_Off ,(*reg).ns+1); 
		#endif
		free (p);
		return (TRUE);
	} 
	else {
		free (p);
		return (FALSE);
	}
}

/* ------------------------------------------------------------------------- */
void resetta_parametri_del_registro (sta_registry_t* reg,char force) {

	/* Il tempo di occupazione del mezzo viene impostato nella successiva procedura */
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
char stiamo_ricevendo (sta_registry_t* reg) {

	return ((*reg).in_ricezione);
}

/* ------------------------------------------------------------------------- */
char stiamo_trasmettendo (sta_registry_t* reg) {

	return ((*reg).in_trasmissione);
}

/* ------------------------------------------------------------------------- */
char pacchetto_nostro (sta_registry_t* reg) {
	pframe_t *p;
	
	/* Spacchettiamo il buffer locale di ricezione ... */
	p = get_frame_buffer ((*reg).BLR);
	/* ... e verifichiamo se il pacchetto é per noi */
	if (mac2nsta ((*p).addr1) == (*reg).ns+1) {
		free (p);
		return (TRUE);
	}
	else {
		free (p);
		return (FALSE);
	}
}

/* ------------------------------------------------------------------------- */
char is_RTS (sta_registry_t* reg,char azzera_pacchetto) {
	pframe_t *p;
	
	/* Spacchettiamo il buffer locale di ricezione ... */
	p = get_frame_buffer ((*reg).BLR);

	if ((*p).rts == 1) {
		 #if STA_DEBUG == 1
		 	printf (_CBIYellow "STA%d ricevuto RTS\n"_CColor_Off ,(*reg).ns+1); 
		 #endif
		 (*reg).nBLR = 0; 
		 free (p);
		 return (TRUE);
	}
	else {
		if (azzera_pacchetto == TRUE)	/* Nel caso il pacchetto non sia nostro, lo dobbiamo comunque azzerare */
			(*reg).nBLR = 0;
		free (p);
		return (FALSE);
	}
}

/* ------------------------------------------------------------------------- */
char is_CTS (sta_registry_t* reg) {
	pframe_t *p;
	
	/* Spacchettiamo il buffer locale di ricezione ... */
	p = get_frame_buffer ((*reg).BLR);

	if ((*p).cts == 1) {
		 #if STA_DEBUG == 1
		 	printf (_CBIYellow "STA%d ricevuto CTS\n"_CColor_Off ,(*reg).ns+1); 
		 #endif
		 (*reg).nBLR = 0; 
		 (*reg).timeCTS = 0;		/* CTS ricevuto. Resettiamo il timeout */
		 free (p);
		 return (TRUE);
	}
	else {
		free (p);
		return (FALSE);
	}
}

/* ------------------------------------------------------------------------- */
char is_ACK (sta_registry_t* reg) {
	pframe_t *p;
	
	/* Spacchettiamo il buffer locale di ricezione ... */
	p = get_frame_buffer ((*reg).BLR);

	if ((*p).seqctrl == _ack_value) {
		#if STA_DEBUG == 1
		 	printf (_CBIYellow "STA%d ricevuto ACK\n"_CColor_Off ,(*reg).ns+1); 
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
void spedisci_pacchetto_a_mezzo (sta_registry_t* reg,char* fb,int len) {
	int n,nwrite=0;

	/* Il mezzo ha un descrittore per ogni stazione. stafd_g [] é il relativo descrittore lato stazioni */
	while( (n = write(stafd_g [(*reg).ns], &(fb[nwrite]), len-nwrite)) >0 )
		nwrite+=n;
}

/* ------------------------------------------------------------------------- */
char spedito_RTS (sta_registry_t* reg) {

	return ((*reg).RTS);
}

/* ------------------------------------------------------------------------- */
void spedisci_BLT (sta_registry_t* reg)	{
	char BLT [_max_frame_buffer_size];

	/* Spediamo il prossimo pacchetto */
	(*reg).nBLT++;
	
	/* Ricaviamo il pacchetto dalla LTT */
	if (fifo_read_deep ((*reg).LTT,BLT,(*reg).nBLT) == FALSE) {
		printf (_Cerror "Stazione %d : spedisci_BLT\n" _CColor_Off,(*reg).ns+1);
		printf (_Cerror "Il numero di pacchetto richiesto (%d) non é presente nella BLT\n" _CColor_Off,(*reg).nBLT);
		exit (0);
	}
	
	spedisci_pacchetto_a_mezzo (reg,BLT,get_packet_len (BLT));
	
	(*reg).timeACK = getNOWmsec () + randomTime (_t_timer_delay_min,_t_timer_delay_max+4000);	/* Settiamo il timeout per la ricezione dell'ACK */
	
	#if STA_DEBUG == 1
	 	printf (_CBIYellow "STA%d Spedito BLT\n"_CColor_Off ,(*reg).ns+1); 
	#endif
}

/* ------------------------------------------------------------------------- */
void spedisci_RTS (sta_registry_t* reg) {
	pframe_t *p;
	char pack [_max_frame_buffer_size];
	char* fb;
	
	/* Ricaviamo mittente e destinatario dal primo pacchetto che si trova sulla LTT */
	fifo_read ((*reg).LTT,pack);
	/* Spacchettiamo ... */
	p = get_frame_buffer (pack);
	/* ... e creiamo il frame buffer */
	fb=pacchetto_di_controllo (_rts_packet_type,(*p).addr2,(*p).addr1);
	
	spedisci_pacchetto_a_mezzo (reg,fb,_pframe_other_len);
	
	(*reg).RTS = TRUE;
	
	#if STA_DEBUG == 1
	 	printf (_CBIYellow "STA%d Spedito RTS a STA%d\n"_CColor_Off ,(*reg).ns+1,mac2nsta ((*p).addr1)); 
	#endif
	
	/* Ci portiamo in stato di trasmissione */
	(*reg).in_trasmissione = TRUE;
	(*reg).in_ricezione = FALSE;
	(*reg).nBLT = 0;					/* Azzeriamo il numero (relativamente alla sequenza) di pacchetti trasmessi */
	(*reg).timeCTS = getNOWmsec () + randomTime (_t_timer_delay_min,_t_timer_delay_max);	/* Settiamo il timeout per la ricezione del CTS */
	
	free (p);
}

/* ------------------------------------------------------------------------- */
void spedisci_CTS (sta_registry_t* reg) {
	pframe_t *p;
	char* fb;
	
	/* Ricaviamo mittente e destinatario dal BLR (sono invertiti rispetto il pacchetto che si trova nel BLR) */
	p = get_frame_buffer ((*reg).BLR);
	
	/* Creiamo il frame buffer */
	fb=pacchetto_di_controllo (_cts_packet_type,(*p).addr1,(*p).addr2);
	
	spedisci_pacchetto_a_mezzo (reg,fb,_pframe_other_len);
	
	/* ... e ci mettiamo in ricezione */
	(*reg).in_ricezione = TRUE;
	(*reg).in_trasmissione = FALSE;
	
	#if STA_DEBUG == 1
	 	printf (_CBIYellow "STA%d Spedito CTS\n"_CColor_Off ,(*reg).ns+1); 
	#endif
	
	free (p);
}

/* ------------------------------------------------------------------------- */
void spedisci_ACK (sta_registry_t* reg,char ultimo_pacchetto) {
	pframe_t *p;
	char* fb;
	
	/* Ricaviamo mittente e destinatario dal BLR (sono invertiti rispetto il pacchetto che si trova nel BLR) */
	p = get_frame_buffer ((*reg).BLR);
	
	/* Creiamo il frame buffer */
	fb=pacchetto_di_controllo (_ack_packet_type,(*p).addr1,(*p).addr2);
	
	spedisci_pacchetto_a_mezzo (reg,fb,_pframe_other_len);
	
	/* Se abbiamo ricevuto tutti i pacchetti, allora non siamo più in ricezione */
	if (ultimo_pacchetto == TRUE)
		(*reg).in_ricezione = FALSE;
		
	free (p);
}

/* ------------------------------------------------------------------------- */
void aggiorna_mezzo_busy (sta_registry_t* reg) {
	long tempo;
	pframe_t *p;
	
	/* Spacchettiamo il buffer locale di ricezione ... */
	p = get_frame_buffer ((*reg).BLR);
	/* ... e ricaviamo la duration */
	if ((*p).duration == _packet_value_low)
		tempo = _packet_duration_low; 			 /* 500 msec */
	else
		tempo = _packet_duration_hi;			/* 2000 msec */
	
	imposta_tempo_occupazione_MC (_t_busy_normal,tempo,reg);
	
	free (p);
}

/* ------------------------------------------------------------------------- */

char dato_da_scartare (sta_registry_t* reg) {
	pframe_t *p;
	int staz;
	char ris;
	
	/* Siccome il pacchetto viene considerato spedito anche se non si riceve l'ack,
		allora verifichiamo il numero progressivo del messaggio per evitare di stampare dei doppioni.
		Il numero è stato messo impropriamente nel crc per comodità */
	
	/* Spacchettiamo il buffer locale di ricezione ... */
	p = get_frame_buffer ((*reg).BLR);
	staz = mac2nsta ((*p).addr2); 	/* (1..4) */
	printf ("CRC = %d\n",(*p).crc);
	if ((*reg).lastProg [staz-1]== (*p).crc) {	
		ris = TRUE;								/* Il pacchetto ha lo stesso numero del precedente. Lo scartiamo */
	}
	else {
		(*reg).lastProg [staz-1] = (*p).crc;				/* Aggiorniamo l'ultimo numero di sequenza */
		ris = FALSE;
	}
	
	free (p);
	
	return (ris);
}

/* ------------------------------------------------------------------------- */
void costruisci_lista_ricezione_temporanea (sta_registry_t* reg) {
	/* Inseriamo il pacchetto ricevuto nella lista temporanea di ricezione ... */
	fifo_push ((*reg).LTR,(*reg).BLR);
	/* ... ed eliminiamo il pacchetto */
	(*reg).nBLR = 0;
}

/* ------------------------------------------------------------------------- */
char ricevuto_ultimo_pacchetto (sta_registry_t* reg) {
	pframe_t *p;
	
	/* Spacchettiamo ... */
	p = get_frame_buffer ((*reg).BLR);	/* La dimensione é stata azzerata (costruisci_lista_ricezione_temporanea), ma i dati del pacchetto esistono ancora */
	
	/* ... e controlliamo il numero regressivo */
	if ((*p).seqctrl == 1) {
		free (p);
		return (TRUE);
	}
	else {
		free (p);
		return (FALSE);
	}
}

/* ------------------------------------------------------------------------- */
void accoda_LTR_a_LRL (sta_registry_t* reg) {
	char pack [_max_frame_buffer_size];


	pthread_mutex_lock (&mutexLRL [(*reg).ns]);
	
	while (fifo_pop ((*reg).LTR,pack) == TRUE) {		/* Al termine la LTR risulta vuota */
		fifo_push (LRL [(*reg).ns],pack);
	}
	
	pthread_mutex_unlock (&mutexLRL [(*reg).ns]);

	
	/* Abbiamo terminato la ricezione dell'intera sequenza */
}

/* ------------------------------------------------------------------------- */
char scaduto_timout_ACK (sta_registry_t* reg) {
	long now = getNOWmsec ();
	
	if (((*reg).timeACK > 0) && ((*reg).timeACK<now)) {
		#if STA_DEBUG == 1		
			printf (_CBIYellow "sta%d scaduto ACK\n"_CColor_Off ,(*reg).ns+1); 			/* Scaduto ACK */	 	
		#endif
		
		(*reg).nBLR = 0;			
		(*reg).timeACK = 0;	
		
		return (TRUE);
	}
	
	return (FALSE);
}

/* ------------------------------------------------------------------------- */
char scaduto_timeout_CTS (sta_registry_t* reg) {
	long now = getNOWmsec ();
	
	if (((*reg).timeCTS > 0) && ((*reg).timeCTS<now))	{ 
		#if STA_DEBUG == 1		
			printf (_CBIYellow "sta%d scaduto CTS\n"_CColor_Off ,(*reg).ns+1); 			/* Scaduto CTS */	 	
		#endif
		
		(*reg).timeCTS = 0;
	
		return (TRUE);
	}		
	
	return (FALSE);
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
	
	if (((*reg).timePACK > 0) && ((*reg).timePACK<now))	{ 
		#if STA_DEBUG == 1		
			printf (_CBIYellow"sta%d scaduto PACK\n"_CColor_Off ,(*reg).ns+1);			/* Scaduto tempo di composizione del pacchetto */
		#endif
		
		(*reg).nPACK++;
		return (TRUE);
	}		
	
	return (FALSE);
}

/* ------------------------------------------------------------------------- */
char ho_dei_dati_da_spedire (sta_registry_t* reg) {
	char ris = FALSE;

	/* Controlliamo se ci sono elementi nella lista di trasmissione locale */
	pthread_mutex_lock (&mutexLTL [(*reg).ns]);
	ris = fifo_is_empty (LTL [(*reg).ns]);			/* TRUE se la lista é vuota */
	pthread_mutex_unlock (&mutexLTL [(*reg).ns]);

	/* Controlliamo anche la lista locale LTT */
	if (ris == TRUE) {
		ris = fifo_is_empty ((*reg).LTT);
	}
	
	return (!ris); 
}

/* ------------------------------------------------------------------------- */
char lista_temporanea_trasmissione_allocata (sta_registry_t* reg) {
	/* Verifica se la LTT e' vuota */ 
	
	return (!fifo_is_empty ((*reg).LTT));
}

/* ------------------------------------------------------------------------- */
char mezzo_disponibile (sta_registry_t* reg) {
	/* Il mezzo é disponibile se il tempo attuale é superiore al tempo di occupazione ipotizzato */
	/* Attenzione : questa é solo una valutazione. Non é detto che il mezzo sia effettivamente libero */
	/* Ci penserá il mezzo a risolvere eventuali conflitti */
	
	if (getNOWmsec () >= ((*reg).t_mc_busy))
		return (TRUE);
	else
		return (FALSE);
}

/* ------------------------------------------------------------------------- */
void trasferisci_sequenza_da_LTL_in_LTT (sta_registry_t* reg) {
	pframe_t *p;
	char pack [_max_frame_buffer_size];
	int seqctrl;
	int prog;
	
	/* Dobbiamo trasferire una sequenza dalla LTL alla LTT senza cancellare gli elementi da LTL */
	/* Un blocco finisce quando il numero regressivo del pacchetto raggiunge 1 */
	
	if (!fifo_is_empty ((*reg).LTT)) {
		printf (_Cerror "STA%d - Dobbiamo inizializzare LTT, ma contiene giá dei dati\n" _CColor_Off,(*reg).ns+1);
		exit (1);
	}
	
	pthread_mutex_lock (&mutexLTL [(*reg).ns]);
	
	prog = 1;
	do {
		/* Prendiamo un elemento da LTL ... */
		fifo_read_deep (LTL [(*reg).ns],pack,prog);
		/* ... e lo inseriamo in LTT */
		fifo_push ((*reg).LTT,pack);
		
		p = get_frame_buffer (pack);
		seqctrl=(*p).seqctrl;
		free (p);
		prog++;
	} while (seqctrl!=1);
	
	pthread_mutex_unlock (&mutexLTL [(*reg).ns]);
}
 
/* ######################################################################### */
/* ######################################################################### */

void prn_reg (sta_registry_t* reg,int ns) {
	pframe_t *p;
	
	if ((ns == 0) || (ns == ((*reg).ns+1))) {
		printf ("STA%d # busy=%ld # tras=%d # ric=%d # ",(*reg).ns+1,(*reg).t_mc_busy,(*reg).in_trasmissione,(*reg).in_ricezione);
		printf ("RTS=%d # nBLR=%d # nBLT=%d # NOW=%ld # ",(*reg).RTS,(*reg).nBLR,(*reg).nBLT,getNOWmsec ());
		
		p = get_frame_buffer ((*reg).BLR);
		printf ("destBLR=%d # mittBLR=%d # ",mac2nsta ((*p).addr1),mac2nsta ((*p).addr2));
		
		printf ("TRUE=%d\n",TRUE);
		
		
		printf ("isCTS=%d # isRTS=%d # isACK=%d # timePACK=%ld\n",(*p).cts,(*p).rts,is_ACK (reg),(*reg).timePACK);
		
		free (p);

	}
}











