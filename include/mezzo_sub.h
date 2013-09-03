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

#endif
