#ifndef _STAZIONE_H
#define _STAZIONE_H

/* ------------------------------------------------------------------------- */
/* Costanti */

/* Numero delle stazioni */
#define _nsta 4
#define _Csta _CGreen		/* Colore dei messaggi delle stazioni */

/* Aree di visibilità delle stazioni */
#define _area_sta1 0x01		/* 01  (21) */
#define _area_sta2 0x01		/* 01 */
#define _area_sta3 0x02		/* 10 */
#define _area_sta4 0x03		/* 11 */ 	
extern const int _area_stax [];

/* valori per la mappa di bit da combinare con _sta_di_sta (campo "campo" di station_t) */
#define _campo_sta1 1
#define _campo_sta2 2
#define _campo_sta3 4
#define _campo_sta4 8
extern const int _campo_stax [];

/* definizione delle stazioni raggiungibili (compresa la stazione stessa) */
#define _sta_di_sta1 0x0B	/* 1011  ( 4321 )*/
#define _sta_di_sta2 0x0B	/* 1011 */
#define _sta_di_sta3 0x0C	/* 1100 */ 
#define _sta_di_sta4 0x0F	/* 1111 */
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
 	char area;		/* area di visibilità elettromagnetica */
 } station_t;
 
 typedef struct {
	int nready;			/* risultato al termine della select */
	fd_set Rset;		/* fd da controllare il lettura */
	fd_set Wset;		/* fd da controllare in scrittura */
	int fdtop; 			/* fd da monitorare con valore più alto */
	int stafd;			/* fd della stazione */
	int mezzofd;		/* fd del mezzo condiviso */
} stato_sta_t;
 
/* ------------------------------------------------------------------------- */
/* Variabili globali */ 
station_t stazione_g [_nsta];	/* identità delle stazioni */
int stafd_g [_nsta];			/* fd delle stazioni */

/* ------------------------------------------------------------------------- */
/* Prototipi delle funzioni */

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







