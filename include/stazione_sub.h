#ifndef _STAZIONE_SUB_H
#define _STAZIONE_SUB_H

/* --- Costanti ---------------- */
#define _t_busy_normal 0		/* costanti per il tempo di occupazione del mezzo */
#define _t_busy_after 1
#define _t_busy_error 2


/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: Aggiunge src in coda a dst
* Par. Ritorno  : restituisce la lunghezza totale del pacchetto
* Par. Formali  :
			- dst : buffer di destinazione
			- src : buffer sorgente
			- nnow : numero di caratteri presenti nel buffer di destinazione
			- nadd : numero di caratteri presenti nel buffer sorgente
---------------------------------------------------------------------------- */
int appendi_pacchetto (char* dst, char* src, int nnow, int nadd);



/* ######################################################################### */
/* ######################################################################### */

typedef struct {
	char pack [_max_frame_buffer_size];			/* Pacchetto che si forma accodando i vari tmp_pack che man mano arrivano dal mezzo */
	long t_mc_busy;								/* epoch occupazione mezzo */
	char in_trasmissione;						/* TRUE : stiamo trasmettendo */
	char in_ricezione;							/* TRUE : stiamo ricevendo */
	char RTS;									/* TRUE : sessione si trasmissione : RTS spedito */
} sta_registry_t;

/* ------------------------------------------------------------------------- */

void resetta_buffer_ricezione (sta_registry_t* reg);

/* ------------------------------------------------------------------------- */

void imposta_tempo_occupazione_MC (int tipo,long tempo);


#endif
