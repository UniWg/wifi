#ifndef _STAZIONE_SUB_H
#define _STAZIONE_SUB_H

/* --- Costanti ---------------- */
#define _t_busy_normal 0		/* costanti per il tempo di occupazione del mezzo */
#define _t_busy_after 1
#define _t_busy_error 2

#define _t_timer_delay_min 4000		/* range per i timeout */
#define _t_timer_delay_max 6000
#define _timeout_consecutivi 2		/* Massimo numero di timout consecutivi permessi */


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
	int ns;										/* Numero della stazione 0..3 */
	long t_mc_busy;								/* epoch occupazione mezzo */
	char in_trasmissione;						/* TRUE : stiamo trasmettendo */
	char in_ricezione;							/* TRUE : stiamo ricevendo */
	char RTS;									/* TRUE : sessione si trasmissione : RTS spedito */
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

/* ----------------------------------------------------------------------------
* Descrizione	: Se ci sono problemi nella configurazione blocca  l'applicazione
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
void checkUp (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Restituisce un numero casuale compreso tra min e max. Utilizzato per calcolare intervalli di tempo
* Par. Formali  :
			- min : tempo minimo in millisecondi
			- max : tempo massimo in millisecondi
---------------------------------------------------------------------------- */
long randomTime (long min,long max);

/* ----------------------------------------------------------------------------
* Descrizione	: Imposta il tempo (epoch) in cui il mezzo si considera occupato
* Par. Formali  :
			- tipo : tipo di definizione (vedi costanti)
			- tempo : tempo in millisecondi (se 0 non viene considerato)
---------------------------------------------------------------------------- */
void imposta_tempo_occupazione_MC (int tipo,long tempo,sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: macro - Blocco azzurro del flowchart
---------------------------------------------------------------------------- */
void continua_trasmissione (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se ci sono ancora pacchetti della stessa sequenza di trasmissione
					da spedire al mezzo
* Par. Ritorno  : Restituisce TRUE se ci sono ancora pacchetti
			- reg : registro di stato
---------------------------------------------------------------------------- */
char ci_sono_ancora_dati_nella_sequenza (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Resetta BTL e LTT
---------------------------------------------------------------------------- */
void resetta_buffer_trasmissione (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Prende il pacchetto che arriva dal mezzo e lo mette nel buffer temporaneo
* Par. Ritorno  : Restituisce il numero di byte letti
* Par. Formali  :
			- s : stato della select
			- tmp_pack : pacchetto di ricezione temporaneo
			- reg : registro di stato
---------------------------------------------------------------------------- */
int sta_prendi_pacchetto (stato_sta_t* s,char* tmp_pack,sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Accoda il pacchetto temporaneo al BLR (reg.pack) e verifica 
					se il pacchetto ottenuto é completo
* Par. Ritorno  : Restituisce TRUE se il pacchetto é completo
* Par. Formali  :
			- tmp_pack : pacchetto di ricezione temporaneo
			- len : lunghezza del pacchetto di ricezione temporaneo
			- reg : registro di stato
---------------------------------------------------------------------------- */
char pacchetto_completo (char* tmp_pack,int len,sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se il pacchetto ricevuto contiene un errore (CRC = 0)
* Par. Ritorno  : Restituisce TRUE se il pacchetto contiene un errore
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char CRC_uguale_0 (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Resetta l'indice dei pacchetti che arrivano
* Par. Formali  :
			- reg : registro di stato
			- force : se TRUE forza il reset dei parametri. 
					  se FALSE resetta i parametri solo se è scaduto il tempo di occupazione del mezzo
---------------------------------------------------------------------------- */
void resetta_parametri_del_registro (sta_registry_t* reg,char force); 

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se siamo in fase di ricezione di un pacchetto
* Par. Ritorno  : Restituisce TRUE se siamo in fase di ricezione
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char stiamo_ricevendo (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se siamo in fase di trasmissione di un pacchetto
* Par. Ritorno  : Restituisce TRUE se siamo in fase di trasmissione
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char stiamo_trasmettendo (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se il pacchetto ricevuto dal mezzo é per noi
* Par. Ritorno  : Restituisce TRUE se il pacchetto é per noi
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char pacchetto_nostro (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se il pacchetto ricevuto dal mezzo é un RTS
* Par. Ritorno  : Restituisce TRUE se il pacchetto é un RTS
* Par. Formali  :
			- reg : registro di stato
			- azzera_pacchetto : se TRUE azzera il pacchetto (nBLR) in tutti i casi
---------------------------------------------------------------------------- */
char is_RTS (sta_registry_t* reg,char azzera_pacchetto);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se il pacchetto ricevuto dal mezzo é un CTS
* Par. Ritorno  : Restituisce TRUE se il pacchetto é un CTS
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char is_CTS (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se il pacchetto ricevuto dal mezzo é un ACK
* Par. Ritorno  : Restituisce TRUE se il pacchetto é un ACK
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char is_ACK (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se il siamo in trasmissione e abbiamo spedito un RTS
* Par. Ritorno  : Restituisce TRUE se abbiamo spedito un RTS
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char spedito_RTS (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Spedisce il pacchetto al mezzo condiviso
* Par. Formali  :
			- reg : registro di stato
			- fb : puntatore al pacchetto da spedire
			- len : lunghezza del pacchetto
---------------------------------------------------------------------------- */
void spedisci_pacchetto_a_mezzo (sta_registry_t* reg,char* fb,int len);

/* ----------------------------------------------------------------------------
* Descrizione	: Spedisce BLT al mezzo
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
void spedisci_BLT (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Spedisce un RTS al mezzo
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
void spedisci_RTS (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Spedisce un RTS al mezzo
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
void spedisci_CTS (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Spedisce un RTS al mezzo
* Par. Formali  :
			- reg : registro di stato
			- ultimo_pacchetto : se TRUE, allora si tratta dell'ultimo pacchetto della sequenza
---------------------------------------------------------------------------- */
void spedisci_ACK (sta_registry_t* reg,char ultimo_pacchetto);

/* ----------------------------------------------------------------------------
* Descrizione	: Aggiorna il tempo di occupazione del mezzo in base a quanto 
					scritto nel campo Duration del pacchetto RTS ricevuto
---------------------------------------------------------------------------- */
void aggiorna_mezzo_busy (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se il numero di sequenza attuale è già stato ricevuto
* Par. Ritorno  : Restituisce TRUE se il numero di sequenza attuale è uguale al precedente.
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char dato_da_scartare (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Aggiunge il pacchetto ricevuto alla LRT
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
void costruisci_lista_ricezione_temporanea (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se abbiamo ricevuto l'ultimo pacchetto della sequenza
* Par. Ritorno  : Restituisce TRUE se abbiamo ricevuto l'ultimo pacchetto
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char ricevuto_ultimo_pacchetto (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Accoda LRT alla lista di ricezione della app
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
void accoda_LTR_a_LRL (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se é scaduto il timeout per la ricezione dell'ACK
* Par. Ritorno  : Restituisce TRUE se il timeout risulta scaduto
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char scaduto_timout_ACK (sta_registry_t* reg); 

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se é scaduto il timeout per la ricezione dell'CTS
* Par. Ritorno  : Restituisce TRUE se il timeout risulta scaduto
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char scaduto_timeout_CTS (sta_registry_t* reg); 

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se é scaduto il timeout per la ricezione del pacchetto completo
* Par. Ritorno  : Restituisce TRUE se il timeout risulta scaduto
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char scaduto_timeout_PACK (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se c'é qualche elemento nella lista di trasmissione
					dell'app o nella lista locale
* Par. Ritorno  : Restituisce TRUE se c'é qualche elemento da spedire
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char ho_dei_dati_da_spedire (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se la LTT ha giá qualche elemento
* Par. Ritorno  : Restituisce TRUE se la LTT non é vuota
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char lista_temporanea_trasmissione_allocata (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Verifica se il mezzo é disponibile
* Par. Ritorno  : Restituisce TRUE se il mezzo é disponibile
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
char mezzo_disponibile (sta_registry_t* reg);

/* ----------------------------------------------------------------------------
* Descrizione	: Prepara la LTT senza cancellare il contenuto di LTL
* Par. Formali  :
			- reg : registro di stato
---------------------------------------------------------------------------- */
void trasferisci_sequenza_da_LTL_in_LTT (sta_registry_t* reg);

void prn_reg (sta_registry_t* reg,int ns);

#endif













