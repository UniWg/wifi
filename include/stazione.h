#ifndef _STAZIONE_H
#define _STAZIONE_H

/* ------------------------------------------------------------------------- */
/* Costanti */

/* Numero delle stazioni */
#define _nsta 4
#define _Csta _CGreen		/* Colore dei messaggi delle stazioni */

/* valori per la mappa di bit dfa combinare con _sta_di_sta (campo "campo" di station_t) */
#define _campo_sta1 1
#define _campo_sta2 2
#define _campo_sta3 4
#define _campo_sta4 8
extern const int _campo_stax [];

/* definizione delle stazioni raggiungibili */
/* ogni stazione viene esclusa dalla sua lista */
#define _sta_di_sta1 0x0A	/* 1010  ( 4321 )*/
#define _sta_di_sta2 0x09	/* 1001 */
#define _sta_di_sta3 0x08	/* 1000 */ 
#define _sta_di_sta4 0x07	/* 0111 */
extern const int _sta_di_stax [];

/* definizione dei mac address (valori casuali) */
#define _mac_sta1 "10:11:12:13:14:15"
#define _mac_sta2 "20:21:22:23:24:25"
#define _mac_sta3 "30:31:32:33:34:35"
#define _mac_sta4 "F4:41:42:43:44:45"
extern const char _mac_stax [][17];

/* ------------------------------------------------------------------------- */
/* Struttura dati d'appoggio */

/* Definizione delle stazioni */
 typedef struct station {
 	char indice;	/* numero della stazione 1..4 */
 	char mac [6];	/* mac address */
 	char campo;		/* mappa di bit che identifica le stazioni raggiungibili */
 } station_t;
 
/* ------------------------------------------------------------------------- */
/* Variabili globali */ 
station_t stazione_g [_nsta];	/* identità delle stazioni */
int stafd_g [_nsta];			/* fd delle stazioni */

/* ------------------------------------------------------------------------- */
/* Prototipi delle funzioni */

void cpmac (const char* src,char* dst);
/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: copia il mac address dal formato esteso al formato a 6 byte.
				  Utilizzato in genere per copiare il mac address dentro
				  i campi addr della struttura pframe_t
* Par. Formali  :
			src : mac address sorgente in formato esteso
			dst : mac address destinazione in formato compatto 6 byte		    
---------------------------------------------------------------------------- */

void mac2str (const char* mac,char* asc);
/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: converte il mac address dal formato formato esteso al 
				  formato ascii a 6 byte
* Par. Formali  :
			- mac : indirizzo mac nel formato esteso xx:xx:xx:xx:xx:xx
					non ci sono controlli sulla correttezza formale della stringa
			- asc : contiene il risultato della conversione
---------------------------------------------------------------------------- */

void str2mac (char* asc,char* mac);
/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: converte il mac address dal formato formato a 6 byte al  
				  formato esteso
* Par. Formali  :
			- asc : indirizzo mac nel formato a 6 byte
					non ci sono controlli sulla correttezza formale della stringa
			- mac : contiene il risultato della conversione
---------------------------------------------------------------------------- */

void inizializza_stazioni (void);
/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: inizializza la struttura dati delle stazioni definendo:
					- numero della stazione
					- mac address
					- campo di visibilità elettromagnetica
---------------------------------------------------------------------------- */

void start_sta_thread (void);
/* ----------------------------------------------------------------------------
* Nome			: 
* Descrizione	: lancia i thread delle stazioni
---------------------------------------------------------------------------- */

#endif







