#ifndef _UTILS_H
#define _UTILS_H

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

#endif



