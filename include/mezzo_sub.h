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
				pack : pacchetto in formato pseudo 802.11 (da restituire)
---------------------------------------------------------------------------- */
void prendi_pacchetto (char* pack);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Verifica se l'area relativa al pacchetto f è libera
* Par. Ritorno  : Restituisce TRUE se l'area è libera
* Par. Formali  :
				f : pacchetto formattato
---------------------------------------------------------------------------- */
char area_libera (pframe_t* f);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Occupa l'area del pacchetto f per un tempo pari a Duration
* Par. Formali  :
				f : pacchetto formattato
---------------------------------------------------------------------------- */
void occupa_area (pframe_t* f);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Libera l'area indicata da f
* Par. Formali  :
				f : pacchetto formattato
---------------------------------------------------------------------------- */
void libera_area (pframe_t* f);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Mette il pacchetto nel buffer (nell'area giusta)
* Par. Formali  :
				pack : pacchetto in formato pseudo 802.11
				f : pacchetto formattato
---------------------------------------------------------------------------- */
void metti_pacchetto_nel_buffer (char* pack,pframe_t* f);

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
* Descrizione	: Verifica se il buffer contiene qualche elemento
* Par. Ritorno  : Restituisce TRUE se c'è almeno un pacchetto nel buffer
---------------------------------------------------------------------------- */
char buffer_pieno (void);

/* ------------------------------------------------------------------------- 
* Nome			: carlo
* Descrizione	: Prende il primo pacchetto che trova nel buffer delle aree
* Par. Formali  :
				pack : pacchetto in formato pseudo 802.11 (da restituire)
---------------------------------------------------------------------------- */
void prendi_pacchetto_dal_buffer (char* pack);

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

#endif







