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
* Par. Ritorno	: Restituisce TRUE se il pacchetto è completo, FALSE altrimenti
* Par. Formali	: 
			- pack	: pacchetto in ricezione
			- len	: lunghezza del pacchetto
---------------------------------------------------------------------------- */

char CRC_zero (char* pack);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è corrotto
* Par. Ritorno	: FALSE se il pacchetto non è corrotto, TRUE altrimenti
* Par. Formali	: 
			- pack	: pacchetto in ricezione
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

char buffer_trasmissione_vuoto(char* buf_t);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il buffer in trasmissione è vuoto
* Par. Ritorno	: TRUE se il buffer è vuoto, FALSE altrimenti
* Par. Formali	: 
			- buf_t	: buffer in trasmissione
---------------------------------------------------------------------------- */

char pacchetto_nostro(char* pack, int ns);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è destinato a noi
* Par. Ritorno	: TRUE se il pacchetto è destinato a noi, FALSE altrimenti
* Par. Formali	: 
			- pack	: pacchetto in ricezione
			- ns	: numero della stazione
---------------------------------------------------------------------------- */

char buffer_allocato(char* tmp_buf);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il buffer temporaneo è stato già allocato o no
* Par. Ritorno	: TRUE se il buffer temporaneo è stato allocato, FALSE altrimenti
* Par. Formali	: 
			- tmp_buf	: buffer temporaneo
---------------------------------------------------------------------------- */

char mezzo_disponibile(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il mezzo è disponibile o meno
* Par. Ritorno	: TRUE se il mezzo è disponibile, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void spedisci_RTS(sta_registry_t* reg, int ns);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Spedisce un RTS
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: fregistro della stazione
			- ns	: numero della stazione
---------------------------------------------------------------------------- */

void resetta_buffer_ricezione (sta_registry_t* reg);

/* ------------------------------------------------------------------------- */






/*###########################################################################*/
/*####################DA COMPLETARE##########################################*/
/*###########################################################################*/

void reset_indice(void);

void reset_buffer(void);


int is_CTS(sta_registry_t* reg);

int is_ACK(sta_registry_t* reg);

int is_RTS(sta_registry_t* reg);


int spedito_RTS(void);

void spedisci_CTS(void);

void spedisci_pacchetto(void);



void continua_trasmissione(void);

void aggiungi_pacchetto(char* pack);

void aggiorna_MC(void);

int scaduto_timeout_ACK(void);






void prepara_buffer(void);



/* ------------------------------------------------------------------------- */

#endif
