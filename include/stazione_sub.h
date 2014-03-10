#ifndef _STAZIONE_SUB_H
#define _STAZIONE_SUB_H

/* --- Costanti ---------------- */
#define _t_busy_normal 0		/* costanti per il tempo di occupazione del mezzo */
#define _t_busy_after 1
#define _t_busy_error 2

#define _t_timer_delay_min 4000		/* range per i timeout */
#define _t_timer_delay_max 6000
#define _timeout_consecutivi 2		/* Massimo numero di timout consecutivi permessi */

/* ######################################################################### */
/* ######################################################################### */

typedef struct {
	int ns;										/* Numero della stazione 0..3 */
	long t_mc_busy;								/* epoch occupazione mezzo */
	char in_trasmissione;						/* TRUE : stiamo trasmettendo */
	char in_ricezione;							/* TRUE : stiamo ricevendo */
	char RTS;									/* TRUE : sessione di trasmissione : RTS spedito */
	list2* LTT;									/* Lista temporanea di trasmissione */
	list2* LTR;									/* Lista temporanea di ricezione */
	char BLR [_max_frame_buffer_size];			/* Buffer per il pacchetto di input (si forma man mano che arrivano dati dal mezzo) */
	int nBLR;									/* Dimensione attuale di BLR */
	int nBLT;									/* Numero di pacchetti della sequenza realmente trasmessi */
	long timeCTS;								/* timeout per ricezione CTS */
	long timeACK;								/* timeout per ricezione ACK */
	long timePACK;								/* timeout per ricezione pacchetto ben formato */
	int nPACK;									/* Numero di timeout PACK consecutivi */
	int lastProg [4];							/* Ultimo numero progressivo di ogni stazioni (0..4) */
} sta_registry_t;


/*
	BRL -> Buffer Ricezione Locale	(dal mezzo verso la sta)
	LRT -> Lista Ricezione Temporanea (lista della sta che una volta completata verrá trasferita alla app)
	LRL -> Lista Ricezione Locale (lista della app)
	
	BTL -> Buffer Trasmissione Locale (dalla sta verso il mezzo)
	LTT -> Lista Trasmissione Temporanea (lista della sta con i dati presi dalla app)
	LTL -> Lista Trasmissione Locale (lista della app)
	
*/

/* ------------------------------------------------------------------------- */
/* ######################################################################### */
/* ######################################################################### */
/* ###################### PROTOTIPI DELLE FUNZIONI ######################### */
/* ######################################################################### */
/* ######################################################################### */

int sta_prendi_pacchetto (stato_sta_t *s, char* tmp_pack, sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Riceve il pacchetto dati
* Par. Ritorno	: Restituisce la lunghezza del pacchetto ricevuto
* Par. Formali	:
			- s 		: stato della stazione
			- tmp_pack	: pacchetto di ricezione temporaneo
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

char is_CTS(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è un CTS
* Par. Ritorno	: TRUE se il pacchetto è un CTS, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char is_RTS(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è un RTS
* Par. Ritorno	: TRUE se il pacchetto è un RTS, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char is_ACK(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il pacchetto è un ACK
* Par. Ritorno	: TRUE se il pacchetto è un ACK, FALSE altrimenti
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

void spedisci_CTS(sta_registry_t* reg, stato_sta_t *s);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Spedisce un CTS
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
			- s		: stato della stazione
---------------------------------------------------------------------------- */

void spedisci_ACK(sta_registry_t* reg, char ultimo);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Spedisce un ACK
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
			- ultimo	: controllo per vedere se è arrivato l'ultimo pacchetto
---------------------------------------------------------------------------- */

char spedito_RTS(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se ho giá spedito un RTS
* Par. Ritorno	: TRUE se l'ho giá spedito, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */



void from_LTT_to_BLT(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Spredisce un pacchetto trasferendo i dati da LTT a BLT
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char  scaduto_timeout_PACK (sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se é scaduto il timeout per la ricezione del pacchetto completo
* Par. Ritorno	: Restituisce TRUE se il timeout è scaduto
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char scaduto_timeout_ACK(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se é scaduto il timeout per l'ACK
* Par. Ritorno	: Restituisce TRUE se il timeout è scaduto
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char scaduto_timeout_CTS(sta_registry_t* reg); 
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se é scaduto il timeout per il CTS
* Par. Ritorno	: Restituisce TRUE se il timeout è scaduto
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void reset_parametri (sta_registry_t* reg, char force);
/* ----------------------------------------------------------------------------
* Descrizione	: Resetta l'indice dei pacchetti che arrivano
* Par. Formali  :
			- reg : registro di stato
			- force : se TRUE forza il reset dei parametri. 
					  se FALSE resetta i parametri solo se è scaduto il tempo di occupazione del mezzo
---------------------------------------------------------------------------- */

char pacchetto_completo (int n, char* tmp_pack, sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Descrizione	: Accoda il pacchetto temporaneo al BLR (reg.pack) e verifica 
					se il pacchetto ottenuto é completo
* Par. Ritorno  : Restituisce TRUE se il pacchetto é completo
* Par. Formali  :
			- tmp_pack : pacchetto di ricezione temporaneo
			- len : lunghezza del pacchetto di ricezione temporaneo
			- reg : registro di stato
---------------------------------------------------------------------------- */

long randomTime (long min,long max);
/* ----------------------------------------------------------------------------
* Descrizione	: Restituisce un numero casuale compreso tra min e max. Utilizzato per calcolare intervalli di tempo
* Par. Formali  :
			- min : tempo minimo in millisecondi
			- max : tempo massimo in millisecondi
---------------------------------------------------------------------------- */

void from_LTL_to_LTT(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Sposta i dati da LTL a LTT
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char dato_da_scartare (sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il numero di sequenza attuale è già stato ricevuto
* Par. Ritorno	: Restituisce TRUE se il numero di sequenza attuale è uguale al precedente
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void from_BLR_to_LTR(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Sposta i dati da BLR a LTR
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char seqctrl_uguale_1(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se il numero regressivo è uguale a 1
* Par. Ritorno	: TRUE se il numero regressivo è = 1, FALSE altrimenti
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void from_LTR_to_LRL(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Sposta i dati da LTR a LRL
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void continua_trasmissione(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: macro - Blocco azzurro del flowchart
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

char ci_sono_dati_da_spedire_al_mezzo (sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Verifica se ci sono ancora dati nella stessa sequenza di trasmissione
* Par. Ritorno	: Restituisce TRUE se ci sono ancora dati
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void reset_buffer_trasmissione(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Resetta BTL e LTT
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void aggiungi_pacchetto(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Agglomerato di funzioni che servono a spostare il pacchetto da BLR a LTR e da LTR s LRL
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */

void aggiorna_MC(sta_registry_t* reg);
/* ----------------------------------------------------------------------------
* Nome			: luca
* Descrizione	: Imposta il mezzo occupato
* Par. Ritorno	: NULL
* Par. Formali	: 
			- reg	: registro della stazione
---------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */

#endif
