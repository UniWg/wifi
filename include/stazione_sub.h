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
	char RTS;									/* TRUE : sessione di trasmissione : RTS spedito */
} sta_registry_t;

/* ------------------------------------------------------------------------- */
/* PROTOTIPI DELLE FUNZIONI */

int sta_prendi_pacchetto (stato_sta_t *s, int ns, char* pack);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Riceve il pacchetto dati
* Par. Ritorno	: Restituisce la lunghezza del pacchetto ricevuto
* Par. Formali	: 
			- s		: stato della stazione
			- ns	: numero della stazione
			- pack	: pacchetto in ricezione
---------------------------------------------------------------------------- */

int pacchetto_completo (char* pack, int len);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è completo o no
* Par. Ritorno	: Restituisce 1 se il pacchetto è completo, zero altrimenti
* Par. Formali	: 
			- pack	: pacchetto in ricezione
			- len	: lunghezza del pacchetto
---------------------------------------------------------------------------- */

int CRC_zero (char* pack);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è corrotto
* Par. Ritorno	: 0 se il pacchetto non è corrotto, 1 altrimenti
* Par. Formali	: 
			- pack	: pacchetto in ricezione
---------------------------------------------------------------------------- */

int ricezione (sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se siamo in ricezione
* Par. Ritorno	: 1 se siamo in ricezione, 0 altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

int trasmissione (sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se siamo in trasmissione
* Par. Ritorno	: 1 se siamo in ricezione, 0 altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void imposta_tempo_occupazione_MC (int tipo,long tempo, sta_registry_t* reg );
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Imposta un tempo di occupazione del mezzo diverso in relazione alle costanti _t_busy
* Par. Ritorno	: NULL
* Par. Formali	: 
			- tipo	: intero che decide quale tempo aggiungere
			- tempo	: tempo da attendere
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void resetta_buffer_ricezione (sta_registry_t* reg);

/* ------------------------------------------------------------------------- */

/*###########################################################################*/
/*####################DA COMPLETARE##########################################*/
/*###########################################################################*/

void reset_indice();

void reset_buffer();

int pacchetto_nostro();

int is_CTS(sta_registry_t* reg);

int spedito_RTS();

void spedisci_pacchetto();
/* ------------------------------------------------------------------------- */

#endif
