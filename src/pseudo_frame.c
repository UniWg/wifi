#include "general_include.h"

/* ------------------------------------------------------------------------- */

char* get_new_frame_buffer (void) {
	/* Allochiamo spazio per un buffer di dimensione massima ... */
	char *p = (char*) malloc (sizeof (char) * _max_frame_buffer_size);
	/* ... e per sicurezza ne azzeriamo il contenuto */
	clear_frame_buffer (p);
	return (p);
}

/* ------------------------------------------------------------------------- */

void free_frame_buffer (char* buffer) {
	/* Liberiamo lo spazio occupato dal buffer */
	free (buffer);
}

/* ------------------------------------------------------------------------- */

void clear_frame_buffer (char* buffer) {
	/* Azzera il contenuto del buffer (reset) */
	memset ((char*)buffer,0,_max_frame_buffer_size);
}

/* ------------------------------------------------------------------------- */

void set_frame_buffer (char* buf,pframe* pf) {
	/* Settiamo tutti i campi del frame buffer. La struttura deve essere
		compilata correttamente in tutte le sue parti */
	
	int* pi;
	
	*buf = (*pf).data;
	*(buf+1) = (*pf).dtype;
	*(buf+2) = (*pf).tods;
	*(buf+3) = (*pf).fromds;
	*(buf+4) = (*pf).rts;
	*(buf+5) = (*pf).cts;
	*(buf+6) = (*pf).scan;
	*(buf+7) = (*pf).duration;
	
	pi = (int*)(*pf).packetl;
	*(buf+8) = (*pi+2); 
	*(buf+9) = (*pi+3);
	
	
	
	
	/*
	struct pframe {
	char data, dtype, tods, fromds, rts, cts, scan, duration, crc;
	int packetl, seqcrtl;
	char addr1[6], addr2[6], addr3[6], addr4[6];
	char *buf;
} pframe;*/
}

/* ------------------------------------------------------------------------- */








