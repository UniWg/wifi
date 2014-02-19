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

int sta_prendi_pacchetto (stato_sta_t *s, int ns, char* pack) {
	int n;

	do {
		n = recv (stafd_g [ns], &pack, _max_frame_buffer_size, 0);
	} while ((n<0) && (errno==EINTR));
	
	return n;
}
	

/* ------------------------------------------------------------------------- */

int pacchetto_completo (char* pack, int len) {
	
	if (len == _max_frame_buffer_size) {
		return (1);
	}
	else return (0);
}

/* ------------------------------------------------------------------------- */

int CRC_zero (char* pack) {
	pframe_t *f;

	f = get_frame_buffer(pack);	/* Spacchetto il pacchetto nel frame */
	if ((*f).crc == 0) {
		return 1;
	}
	else return 0;

}

/* ------------------------------------------------------------------------- */

int ricezione (sta_registry_t* reg) {

	if ((*reg).in_ricezione == TRUE) {
		return (1);
	}
	else return 0;
}
	
/* ------------------------------------------------------------------------- */

int trasmissione (sta_registry_t* reg) {

	if ((*reg).in_trasmissione == TRUE) {
		return (1);
	}
	else return 0;
}



/* ######################################################################### */
/* ######################################################################### */




