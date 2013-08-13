#include "general_include.h"

/* ------------------------------------------------------------------------- */

void set_addr (char indy,char* buf, char* ad) {
	int i,start;
	
	/* Calcola l'indice di inizio di Addr(indy) */
	start = _addr1_pos + ((indy-1) * 6);
	for (i=0;i<6;i++)
		*(buf+start+i) = ad[i];
}

/* ------------------------------------------------------------------------- */

void get_addr (char indy,char* buf, char* ad) {
	int i,start;
	
	/* Calcola l'indice di inizio di Addr(indy) */
	start = _addr1_pos + ((indy-1) * 6);
	for (i=0;i<6;i++)
		ad[i] = *(buf+start+i);
}

/* ------------------------------------------------------------------------- */

char* set_frame_buffer (pframe* pf) {
	/* Settiamo tutti i campi del frame buffer. La struttura deve essere
		compilata correttamente in tutte le sue parti */
	char* buf = (char*) malloc ((*pf).packetl);
	int* pi;
	int i;
	
	*(buf+0) = (*pf).data;
	*(buf+1) = (*pf).dtype;
	*(buf+2) = (*pf).tods;
	*(buf+3) = (*pf).fromds;
	*(buf+4) = (*pf).rts;
	*(buf+5) = (*pf).cts;
	*(buf+6) = (*pf).scan;
	*(buf+7) = (*pf).duration;
	
	pi = (int*)(*pf).packetl;
	*(buf+8) = *(pi+2); 
	*(buf+9) = *(pi+3);
	
	set_addr (1,buf,(*pf).addr1);
	set_addr (2,buf,(*pf).addr2);
	set_addr (3,buf,(*pf).addr3);
	set_addr (4,buf,(*pf).addr4);
	
	pi = (int*)(*pf).seqctrl;
	for (i=0;i<4;i++)
		*(buf+_seqctrl_pos+i) = *(pi+i);
	
	/* La dimensione della parte dati la ricaviamo dalla differenza tra
		la lunghezza totale del pacchetto e la lunghezza di tutto ciò
		che non è parte dati */
	for (i=0;i<((*pf).packetl - _pframe_other_len);i++)
		*(buf+_payload_pos+i) = (*pf).buf[i];
		
	*(buf+_payload_pos+i) = (*pf).crc;
	
	return (buf);
}

/* ------------------------------------------------------------------------- */

pframe* get_frame_buffer (char* buf) {
	pframe *pf = (pframe*) malloc (sizeof (pframe));
	int* pi;
	int i,plen;

	(*pf).data = *(buf+0);
	(*pf).dtype = *(buf+1);
	(*pf).tods = *(buf+2);
	(*pf).fromds = *(buf+3);
	(*pf).rts = *(buf+4);
	(*pf).cts = *(buf+5);
	(*pf).scan = *(buf+6);
	(*pf).duration = *(buf+7);
	
	pi = (int*)(*pf).packetl;
	*(pi+2) = *(buf+8); 
	*(pi+3) = *(buf+9);
	
	get_addr (1,buf,(*pf).addr1);
	get_addr (2,buf,(*pf).addr2);
	get_addr (3,buf,(*pf).addr3);
	get_addr (4,buf,(*pf).addr4);
	
	pi = (int*)(*pf).seqctrl;
	for (i=0;i<4;i++)
		*(pi+i) = *(buf+_seqctrl_pos+i);
	
	/* Calcoliamo la lunghezza della parte dati ... */
	plen = (*pf).packetl - _pframe_other_len;
	/* ... creiamo lo spazio necessario ... */
	(*pf).buf = (char*) malloc (sizeof (char) * plen);
	/* ... e poi trasferiamo i dati */
	for (i=0;i<plen;i++)
		(*pf).buf[i] = *(buf+_payload_pos+i);
	
	(*pf).crc = *(buf+_payload_pos+i);
	
	return (pf);
}

/* ------------------------------------------------------------------------- */

void remove_frame_buffer (char* buffer) {
	/* Liberiamo lo spazio occupato dal buffer */
	free (buffer);
}

/* ------------------------------------------------------------------------- */

void remove_pframe (pframe* pf) {
	/* rimuoviamo prima lo spazio relativo al buffer dati ... */
	free ((*pf).buf);
	/* ... e poi la struttura vera e propria */
	free (pf);
}

/* ------------------------------------------------------------------------- */








