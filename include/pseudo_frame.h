#ifndef _PSEUDO_FRAME_H
#define _PSEUDO_FRAME_H

/* 
	Questo modulo contiene le funzioni per la gestione
	del frame pseudo 802.11 (che chiameremo anche frame buffer).
	Il frame buffer non sarà altro che un array di _max_frame_buffer_size byte
	
	"campi" del frame pseudo 802.11
	1b	data			0 - caratteri di controllo // 1 - pacchetto dati
	1b	data type		definisce il contenuto del pacchetto dati (solo se data == 1)
	1b	tods			1 - frame in uscita (diretto al distribution system) // 0 - altrimenti
	1b	fromds			1 - frame in ingresso (arrivato dal DS) // 0 - altrimenti
	1b	rts				1 - ready to send // 0 - non attivato
	1b	cts				1 - clear to send // 0 - non attivato
	1b	scan			0 - no scan // 1 - scan richiesta // 2 - scan risposta
	1b	duration		tempo di occupazione del canale (unità 100ms)
	2b	packet lenght	lunghezza in byte del pacchetto
	6b	addr1			indirizzo mac destinatario
	6b	addr2			indirizzo mac mittente
	6b	addr3
	6b	addr4
	4b	seqcrtl			numero di sequenza del pacchetto
 2000b	payload			se data == 1 -- pachetto di dati
 						se data == 0 -- pacchetto di controllo. Valido solo per le 
 										risposte da precedente richiesta di scansione,
 										cioè valido solo se scan == 2
 										(da decidere quali informazioni mettere)
 	1b	crc				0 se errore
 	
 	
 	- la richiesta di scan viene fatta per conoscere le stazioni presenti nel proprio
 	  raggio d'azione	
 	- il tempo di occupazione del canale (duration) ha solo due possibili valori:
 		- 5 (cioè 500ms) per pacchetti RTS, CTS e pacchetti dati con dimensione < 100 bytes
 		- 20 (cioè 2000ms) per tutti gli altri pacchetti
 	 
*/

/* ------------------------------------------------------------------------- */
/* Struttura dati d'appoggio */

/*  la dimensione del buffer (buf) viene calcolata automaticamente come differenza
	tra la lunghezza del pacchetto e tutto ciò che non è campo dati (_pframe_other_len) */
typedef struct pframe {
	char data, dtype, tods, fromds, rts, cts, scan, duration, crc;
	int packetl, seqctrl;
	char addr1[6], addr2[6], addr3[6], addr4[6];
	char *buf;
} pframe;

/* ------------------------------------------------------------------------- */

char* set_frame_buffer (pframe* pf);
/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: setta il frame buffer in base al contenuto della struttura 
				  pframe. Non permette di fare un settaggio selettivo.
				  Vengono settati TUTTI i "campi" del frame buffer.
* Par. Ritorno  : indirizzo del frame buffer
* Par. Formali  :
			- pf  : puntatore alla struttura pframe
---------------------------------------------------------------------------- */

pframe* get_frame_buffer (char* buf);
/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: prende il contenuto del buffer e lo trasferisce in una
				  struttura di tipo pframe
* Par. Ritorno  : struttura che rapprensenta il contenuto del buffer
* Par. Formali  :
			- buf : puntatore al frame buffer da leggere
---------------------------------------------------------------------------- */

void remove_frame_buffer (char* buffer);
/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: dealloca un frame buffer
* Par. Formali  :
			- buffer : indirizzo del buffer da deallocare
---------------------------------------------------------------------------- */

void remove_pframe (pframe* pf);
/* ----------------------------------------------------------------------------
* Nome			: carlo
* Descrizione	: dealloca una struttura di tipo pframe
* Par. Formali  :
			- pf : indirizzo della struttura da deallocare
---------------------------------------------------------------------------- */

#endif






