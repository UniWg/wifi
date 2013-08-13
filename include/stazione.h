#ifndef _STAZIONE_H
#define _STAZIONE_H

/* Definizione delle stazioni */
 typedef struct station {
 	char mac [6];	/* mac address */
 	char indice;	/* numero della stazione 1..4 */
 	char campo;		/* mappa di bit che identifica le stazioni raggiungibili */
 } station;

#endif
