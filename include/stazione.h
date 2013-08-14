#ifndef _STAZIONE_H
#define _STAZIONE_H

/* ------------------------------------------------------------------------- */
/* Costanti */

/* Numero delle stazioni */
#define _nsta 4

/* valori per la mappa di bit (campo campo di station) */
#define _campo_sta1 1
#define _campo_sta2 2
#define _campo_sta3 4
#define _campo_sta4 8
extern const int _campo_stax [];

/* definizione delle stazioni raggiungibili */
/* ogni stazione viene esclusa dalla sua lista */
#define _sta_di_sta1 0x0A
#define _sta_di_sta2 0x09
#define _sta_di_sta3 0x08
#define _sta_di_sta4 0x07
extern const int _sta_di_stax [];

/* definizione dei mac address (valori casuali) */
#define _mac_sta1 "10:11:12:13:14:15"
#define _mac_sta2 "20:21:22:23:24:25"
#define _mac_sta3 "30:31:32:33:34:35"
#define _mac_sta4 "40:41:42:43:44:45"
extern const char _mac_stax [][17];

/* ------------------------------------------------------------------------- */
/* Struttura dati d'appoggio */

/* Definizione delle stazioni */
 typedef struct station {
 	char indice;	/* numero della stazione 1..4 */
 	char mac [6];	/* mac address */
 	char campo;		/* mappa di bit che identifica le stazioni raggiungibili */
 } station_t;
station_t stazione [4];

/* ------------------------------------------------------------------------- */
/* Prototipi delle funzioni */

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
* Descrizione	: inizializza le stazioni definendo:
					- numero della stazione
					- mac address
					- campo di visibilità elettromagnetica
---------------------------------------------------------------------------- */

#endif







