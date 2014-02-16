#ifndef _MEZZO_SUB_H
#define _MEZZO_SUB_H

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
* Descrizione	: Occupa l'area del pacchetto f per un tempo pari a Duration
* Par. Formali  :
				f : pacchetto formattato
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
void occupa_area (pframe_t* f,area_t* aree,char* pack);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Libera l'area indicata da f
* Par. Formali  :
				f : pacchetto formattato
---------------------------------------------------------------------------- */
void libera_area (pframe_t* f);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Marca il frame corrotto e lo spedisce a tutte le stazioni 
				  dell'area di visibilità elettromagnetica (compreso il mittente)
* Par. Formali  :
				pack : pacchetto in formato pseudo 802.11
				f : pacchetto formattato
---------------------------------------------------------------------------- */
void marca_frame_corrotto_e_spedisci (char* pack,pframe_t* f);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Svuota il buffer relativo all'area indicata da f
* Par. Formali  :
				f : pacchetto formattato
---------------------------------------------------------------------------- */
void svuota_buffer_area (pframe_t* f);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Prende il primo pacchetto che trova nel buffer delle aree
* Par. Ritorno  : Restituisce il numero dell'area dalla quale viene prelevato il pacchetto 
* Par. Formali  :
				pack : pacchetto in formato pseudo 802.11 (da restituire)
---------------------------------------------------------------------------- */
int prendi_pacchetto_dal_buffer (char* pack);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Verifica se è il momento di generare un pacchetto difettoso
* Par. Ritorno  : Restituisce TRUE se bisogna generare un pacchetto difettoso
---------------------------------------------------------------------------- */
char genera_errore_casuale (void);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Spedisce il pacchetto a tutte le sta dell'area fuorchè al mittente
* Par. Formali  :
				pack : pacchetto in formato pseudo 802.11
				f : pacchetto formattato
---------------------------------------------------------------------------- */
void spedisci_pacchetto (char* pack,pframe_t* f);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Toglie dal buffer delle aree il pacchetto indicato da f
* Par. Formali  :
				f : pacchetto formattato
---------------------------------------------------------------------------- */
void elimina_pacchetto_dal_buffer (pframe_t* f);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Verifica se nel buffer delle aree ci sono più pacchetti per 
					stesso mittente
* Par. Ritorno  : Restituisce TRUE se ci sono più pacchetti per lo stesso mittente
---------------------------------------------------------------------------- */
char conflitto_di_destinazioni (void);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Prende il primo pacchetto che trova nel buffer delle aree
* Par. Ritorno  : Restituisce il numero dell'area dalla quale viene prelevato il pacchetto 
* Par. Formali  :
				pack : pacchetto in formato pseudo 802.11 (da restituire)
---------------------------------------------------------------------------- */
char scaduto_timer (int area_attiva, area_t* aree);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Verifica se c'è almeno un pacchetto nelle aree
* Par. Ritorno  : Restituisce TRUE se c'è almeno un pacchetto
* Par. Formali  :
				aree : stato delle aree gestite dal mezzo
---------------------------------------------------------------------------- */
char pacchetto_in_area (area_t*  aree);

#endif







