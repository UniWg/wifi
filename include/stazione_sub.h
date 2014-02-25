#ifndef _STAZIONE_SUB_H
#define _STAZIONE_SUB_H

/* --- Costanti ---------------- */
#define _t_busy_normal 0		/* costanti per il tempo di occupazione del mezzo */
#define _t_busy_after 1
#define _t_busy_error 2

/* ######################################################################### */
/* ######################################################################### */

typedef struct {
	char BLT [_max_frame_buffer_size];			/* Buffer temporaneo di trasmissione */
	char BLR [_max_frame_buffer_size];			/* Buffer temporaneo di ricezione  */
	int x;										/* numeri di caratteri ricevuti nel BLR */
	list2 *LTT [_nsta];							/* Lista temporanea di trasmissione */
	list2 *LTR [_nsta];							/* Lista temporanea di ricezione */
	long t_mc_busy;								/* epoch occupazione mezzo */
	char in_trasmissione;						/* TRUE : stiamo trasmettendo */
	char in_ricezione;							/* TRUE : stiamo ricevendo */
	char RTS;									/* TRUE : sessione di trasmissione : RTS spedito */
	int ns;										/* numero della stazione */
} sta_registry_t;

/* ------------------------------------------------------------------------- */
/* ######################################################################### */
/* ######################################################################### */
/* ###################### PROTOTIPI DELLE FUNZIONI ######################### */
/* ######################################################################### */
/* ######################################################################### */

int appendi_pacchetto (char* dst, char* src, int nnow, int nadd);
/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: Aggiunge src in coda a dst
* Par. Ritorno  : restituisce la lunghezza totale del pacchetto
* Par. Formali  :
			- dst  : buffer di destinazione
			- src  : buffer sorgente
			- nnow : numero di caratteri presenti nel buffer di destinazione
			- nadd : numero di caratteri presenti nel buffer sorgente
---------------------------------------------------------------------------- */

char frame_completo (int n, sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Controlla se il pacchetto è stato ricevuto completo
* Par. Ritorno	: restituisce TRUE se il pacchetto è completo
* Par. Formali	: 
			- n		: numero byte ricevuti
			- reg	: registro della stazione
---------------------------------------------------------------------------- */


int sta_prendi_pacchetto (stato_sta_t *s, sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Riceve il pacchetto dati
* Par. Ritorno	: Restituisce la lunghezza del pacchetto ricevuto
* Par. Formali	:
			- s 		: stato della stazione
			- reg		: registro della stazione
---------------------------------------------------------------------------- */

char CRC_zero (sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è corrotto
* Par. Ritorno	: FALSE se il pacchetto non è corrotto, TRUE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

int is_CTS(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è un CTS
* Par. Ritorno	: TRUE se il pacchetto è un CTS, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

int is_RTS(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è un RTS
* Par. Ritorno	: TRUE se il pacchetto è un RTS, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char ricezione (sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se siamo in ricezione
* Par. Ritorno	: TRUE se siamo in ricezione, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char trasmissione (sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se siamo in trasmissione
* Par. Ritorno	: TRUE se siamo in TRASMISSIONE, FALSE altrimenti
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

char buffer_trasmissione_vuoto(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il buffer in trasmissione è vuoto
* Par. Ritorno	: TRUE se il buffer è vuoto, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char pacchetto_nostro(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è destinato a noi
* Par. Ritorno	: TRUE se il pacchetto è destinato a noi, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char buffer_allocato(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il buffer temporaneo è stato già allocato o no
* Par. Ritorno	: TRUE se il buffer temporaneo è stato allocato, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char mezzo_disponibile(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il mezzo è disponibile o meno
* Par. Ritorno	: TRUE se il mezzo è disponibile, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void spedisci_RTS(sta_registry_t* reg, stato_sta_t *s);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Spedisce un RTS
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
			- s		: stato della stazione
---------------------------------------------------------------------------- */

char spedito_RTS(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se ho giá spedito un RTS
* Par. Ritorno	: TRUE se l'ho giá spedito, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void spedisci_pacchetto(sta_registry_t* reg, stato_sta_t *s);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Spredisce un pacchetto 
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
			- s		: stato della stazione
---------------------------------------------------------------------------- */

void resetta_buffer_ricezione (sta_registry_t* reg);

/* ------------------------------------------------------------------------- */






/*###########################################################################*/
/*####################DA COMPLETARE##########################################*/
/*###########################################################################*/

void reset_indice(void);

void reset_buffer(void);


int is_ACK(sta_registry_t* reg);






void spedisci_CTS(sta_registry_t* reg, stato_sta_t *s);





void continua_trasmissione(void);

void aggiungi_pacchetto(sta_registry_t* reg);

void aggiorna_MC(sta_registry_t* reg);

int scaduto_timeout_ACK(void);






void prepara_buffer(sta_registry_t* reg);



/* ------------------------------------------------------------------------- */

#endif
