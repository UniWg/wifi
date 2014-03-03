#ifndef _UTILS_H
#define _UTILS_H

/* Elemento della lista circolare */
typedef struct circle_list {
	char pack [_max_frame_buffer_size];
	struct circle_list *prec,*next;
} list2;

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: copia il mac address dal formato esteso al formato a 6 byte.
				  Utilizzato in genere per copiare il mac address dentro
				  i campi addr della struttura pframe_t
* Par. Formali  :
			src : mac address sorgente in formato esteso
			dst : mac address destinazione in formato compatto 6 byte		    
---------------------------------------------------------------------------- */
void cpmac (const char* src,char* dst);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: converte il mac address dal formato formato esteso al 
				  formato ascii a 6 byte
* Par. Formali  :
			- mac : indirizzo mac nel formato esteso xx:xx:xx:xx:xx:xx
					non ci sono controlli sulla correttezza formale della stringa
			- asc : contiene il risultato della conversione
---------------------------------------------------------------------------- */
void mac2str (const char* mac,char* asc);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: converte il mac address dal formato formato a 6 byte al  
				  formato esteso
* Par. Formali  :
			- asc : indirizzo mac nel formato a 6 byte
					non ci sono controlli sulla correttezza formale della stringa
			- mac : contiene il risultato della conversione
---------------------------------------------------------------------------- */
void str2mac (char* asc,char* mac);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: dato il mac restituisce il relativo numero di stazione
* Par. Ritorno  : numero della stazione
* Par. Formali  :
			- mac : mac address della stazione nel formato 6 byte
---------------------------------------------------------------------------- */
int mac2nsta (char* mac);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: dato il numero di stazione restituisce il relativo mac6
* Par. Formali  :
			- n : numero della stazione (1..4)
			- mac : mac address della stazione nel formato 6 byte (ritorno)
---------------------------------------------------------------------------- */
void nsta2mac (int n,char* mac);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: Restituisce epoch in millisecondi
* Par. Ritorno  : tempo attuale in millisecondi
---------------------------------------------------------------------------- */
long getNOWmsec (void);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: Crea una lista circolare con la sentinella inizializzata
* Par. Ritorno  : Restituisce il puntatore alle sentinella
---------------------------------------------------------------------------- */
list2* fifo_create (void); 

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: Inserisce un elemento in testa alla lista circolare
* Par. Formali  :
			- s : sentinella
			- pack : pacchetto dati 
---------------------------------------------------------------------------- */
void fifo_push (list2* s,char* pack);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: Legge l'elemento in testa alla coda senza cancellarlo
* Par. Ritorno  : Restituisce FALSE se non ci sono elementi nella coda 
* Par. Formali  :
			- s : sentinella
			- pack : pacchetto dati (viene restituito)
---------------------------------------------------------------------------- */
char fifo_read (list2* s,char* pack);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: Legge l'ennesimo elemento rispettando l'ordine fifo
* Par. Ritorno  : Restituisce FALSE se l'ennesimo elemento non esiste
* Par. Formali  :
			- s : sentinella
			- pack : pacchetto dati (viene restituito)
			- n : numero dell'elemento
---------------------------------------------------------------------------- */
char fifo_read_deep (list2* s,char* pack,int n);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: Rimuove l'elemento dalla testa della coda e lo restituisce
* Par. Ritorno  : Restituisce FALSE se non ci sono elementi nella coda 
* Par. Formali  :
			- s : sentinella
			- pack : pacchetto dati (viene restituito)
---------------------------------------------------------------------------- */
char fifo_pop (list2* s,char* pack);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: Elimina gli elementi della fifo senza cancellare la sentinella
* Par. Formali  :
			- s : sentinella
---------------------------------------------------------------------------- */
void fifo_reset (list2* s);

/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: Verifica se la fifo é vuota
* Par. Ritorno  : Restituisce TRUE se non ci sono elementi nella fifo
* Par. Formali  :
			- s : sentinella
---------------------------------------------------------------------------- */
char fifo_is_empty (list2* s);



#endif



