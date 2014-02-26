#ifndef _MEZZO_SUB_H
#define _MEZZO_SUB_H

/* ------------------------------------------------------------------------- */
/* Prototipi delle funzioni */

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Verifica se nel buffer di ricezione del mezzo (.clibuf) ci sono
					più frame aventi lo stesso destinatario
* Par. Ritorno  : 0 se non ci sono mac corrispondenti
					altrimenti restituisce un numero che rappresenta la mappa di bit
					dei mac corrispondenti 
* Par. Formali  :
				s : struttura dati del mezzo
---------------------------------------------------------------------------- */
int stesso_dest (stato_t* s);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Verifica se nel buffer di ricezione del mezzo (.clibuf) c'è
					almeno un frame con il timestamp scaduto
* Par. Ritorno  : 0 se non ci sono frame con timestamp scaduto
					altrimenti restituisce il numero corrispondente al frame (stazione)
					avente timestamp scaduto
* Par. Formali  :
				s : struttura dati del mezzo
---------------------------------------------------------------------------- */
int out_of_time (stato_t* s);

/* ########################################################################### */

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Prende il primo pacchetto che è stato ricevuto dal mezzo condiviso
* Par. Formali  :
				s : registro di stato della select
				pack : pacchetto in formato pseudo 802.11 (da restituire)
---------------------------------------------------------------------------- */
void prendi_pacchetto (stato_t *s,char* pack);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Verifica se l'area relativa al pacchetto f è libera
* Par. Ritorno  : Restituisce TRUE se l'area è libera
* Par. Formali  :
				f : pacchetto formattato
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
char area_libera (pframe_t* f,area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Imposta il tempo di arrivo nell'area
* Par. Formali  :
				f : pacchetto formattato
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
void marca_arrivo (pframe_t* f,area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Occupa l'area del pacchetto f per un tempo pari a Duration
* Par. Formali  :
				f : pacchetto formattato
				aree : stato delle aree gestite dal mezzo
				pack : pacchetto in formato pseudo 802.11
---------------------------------------------------------------------------- */
void occupa_area (pframe_t* f,area_t* aree,char* pack);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Marca il frame corrotto e lo spedisce a tutte le stazioni 
				  dell'area di visibilità elettromagnetica (compreso il mittente)
* Par. Formali  :
				pack : pacchetto in formato pseudo 802.11
---------------------------------------------------------------------------- */
void marca_errore_per_collisione (pframe_t* f,area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Marca errore sull'area indicata da f e mette a zero il CRC del pacchetto 
* Par. Formali  :
				f : pacchetto formattato
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
void genera_errore_casuale (area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Spedisce primi n-1 byte del pacchetto a tutte le sta dell'area fuorchè al mittente
* Par. Formali  :
				s : registro di stato della select
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
void spedisci_prima_parte_pacchetto (stato_t *s,area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Spedisce ultimo byte del pacchetto a tutte le sta dell'area fuorchè al mittente
* Par. Formali  :
				s : registro di stato della select
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
void spedisci_ultimo_byte (stato_t *s,area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Verifica se nel buffer delle aree ci sono più pacchetti per 
					stesso mittente
* Par. Formali  :
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
void controlla_conflitto_di_destinazioni (area_t* aree) ;

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Verifica se è scaduto il TDout di qualche pacchetto
* Par. Ritorno  : Restituisce TRUE se è scaduto almeno un TDout
* Par. Formali  :
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
char scaduto_timer (area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Verifica se c'è almeno un pacchetto nelle aree
* Par. Ritorno  : Restituisce TRUE se c'è almeno un pacchetto
* Par. Formali  :
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
char pacchetto_in_area (area_t*  aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Resetta i parametri dell'area
* Par. Formali  :
				index : indice dell'area (0..n)
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
void reset_area (int index,area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Mette a zero il CRC del pacchetto contenuto nell'area indicata dall'indice
* Par. Formali  :
				index : indice dell'area (0..n)
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
void set_CRC0 (int index,area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Controlla se c'è qualche area con un errore in corso
* Par. Ritorno  : TRUE se trova almeno un'area con un errore in corso
* Par. Formali  :
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
char qualche_area_in_errore (area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Controlla se il pacchetto che è arrivato si riferisce ad un area in stato di errore
* Par. Ritorno  : TRUE se il pacchetto si riferisce ad un'area in stato di errore
* Par. Formali  :
				f : pacchetto formattato
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
char area_in_errore (pframe_t* f,area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Spedisce n-1 byte del pacchetto al mittente. Si utilizza dopo avere rilevato CRC = 0
* Par. Formali  :
				s : registro di stato della select
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
void spedisci_prima_parte_crc0_mittente (stato_t *s,area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Spedisce ultimo byte del pacchetto al mittente. Si utilizza dopo avere rilevato CRC = 0
* Par. Formali  :
				s : registro di stato della select
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
void spedisci_ultimo_byte_crc0_mittente (stato_t *s,area_t* aree);

#endif







