#ifndef _APPLICA_SUB_H
#define _APPLICA_SUB_H

/* ------------------------------------------------------------------------- 
* Nome			: 
* Descrizione	: Dato il pid del thread ricava da tidentity [] il numero dell'applicazione
* Par. Ritorno  : Restituisce il numero dell'applicazione (1..4)
---------------------------------------------------------------------------- */
char tid2napp (void);

/* ------------------------------------------------------------------------- 
* Nome			: 
* Descrizione	: Spedisce un messaggio ad un'altra applicazione 
* Par. Formali  :
			- id : identificativo dell'applicazione che spedisce (1..4)
			- mac_dest : MAC del destinatario nel formato esteso XX:XX:XX:XX:XX:XX
			- buf : buffer contenente il messaggio da trasmettere
			- msg_len : lunghezza del messaggio (compreso eventuale zero terminatore)
---------------------------------------------------------------------------- */
void Send (char* mac_dest,char* buf,int msg_len);

void Recv (char* mittmac,char* buf,int* msg_len);



#endif
