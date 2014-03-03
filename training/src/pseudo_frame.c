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

char* set_frame_buffer (pframe_t* pf) {
	/* Settiamo tutti i campi del frame buffer. La struttura deve essere
		compilata correttamente in tutte le sue parti */
	char* buf = (char*) malloc ((*pf).packetl);
	char* pi;
	int i,vi;
	
	bzero (buf,(*pf).packetl);
	
	*(buf+0) = (*pf).data;
	*(buf+1) = (*pf).dtype;
	*(buf+2) = (*pf).tods;
	*(buf+3) = (*pf).fromds;
	*(buf+4) = (*pf).rts;
	*(buf+5) = (*pf).cts;
	*(buf+6) = (*pf).scan;
	*(buf+7) = (*pf).duration;
	
	i = (*pf).packetl;
	pi = (char *) &i;
	*(buf+8) = *(pi+1); 	/* Prendiamo solo i due byte più a dx 3210 */
	*(buf+9) = *(pi+0);
	
	set_addr (1,buf,(*pf).addr1);
	set_addr (2,buf,(*pf).addr2);
	set_addr (3,buf,(*pf).addr3);
	set_addr (4,buf,(*pf).addr4);
	
	vi = (*pf).seqctrl;
	pi = (char *) &vi;
	for (i=0;i<4;i++)
		*(buf+_seqctrl_pos+i) = *(pi+3-i);	/* dobbiamo rovesciare le cifre */
	
	/* La dimensione della parte dati la ricaviamo dalla differenza tra
		la lunghezza totale del pacchetto e la lunghezza di tutto ciò
		che non è parte dati */
	for (i=0;i<((*pf).packetl - _pframe_other_len);i++) 
		*(buf+_payload_pos+i) = (*pf).buf[i];
		
	*(buf+_payload_pos+i) = (*pf).crc;
	
	return (buf);
}

/* ------------------------------------------------------------------------- */
/* Restituisce il valore del campo Packet Lenght */
int get_packet_len (char* buf) {
	int vi;
	char* pi;
	
	vi = 0;
	pi = (char *) &vi;
	*(pi+1) = *(buf+8); 
	*(pi+0) = *(buf+9);
	return (vi);
}

/* ------------------------------------------------------------------------- */

pframe_t* get_frame_buffer (char* buf) {
	pframe_t *pf = (pframe_t*) malloc (sizeof (pframe_t));	
	int i,plen,vi;
	char* pi;
	
	bzero (pf,sizeof (pframe_t));
	
	(*pf).data = *(buf+0);
	(*pf).dtype = *(buf+1);
	(*pf).tods = *(buf+2);
	(*pf).fromds = *(buf+3);
	(*pf).rts = *(buf+4);
	(*pf).cts = *(buf+5);
	(*pf).scan = *(buf+6);
	(*pf).duration = *(buf+7);
	(*pf).packetl = get_packet_len (buf);
	
	get_addr (1,buf,(*pf).addr1);
	get_addr (2,buf,(*pf).addr2);
	get_addr (3,buf,(*pf).addr3);
	get_addr (4,buf,(*pf).addr4);
	
	vi = 0;
	pi = (char *) &vi;
	for (i=0;i<4;i++)
		*(pi+3-i) = *(buf+_seqctrl_pos+i);
	(*pf).seqctrl = vi;
	
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
char* pacchetto_di_controllo (char tipo,char* mitt,char* dest) {
	pframe_t *p = (pframe_t*) malloc (sizeof (pframe_t));
	int rts=0,cts=0,ack=1;
	char* fb;
	
	if (tipo == _rts_packet_type) rts = 1;
	if (tipo == _cts_packet_type) cts = 1;
	if (tipo == _ack_packet_type) ack = _ack_value;
	
	(*p).data = 0;					/* Carattere di controllo */
	(*p).dtype = 0;					
	(*p).tods = 1;					/* Pacchetto in uscita */
	(*p).fromds = 0;				
	(*p).rts = rts;
	(*p).cts = cts;
	(*p).scan = 0;
	(*p).duration = _packet_value_low;
	(*p).packetl = _pframe_other_len;
	strncpy ((*p).addr1,dest,6);
	strncpy ((*p).addr2,mitt,6);
	strncpy ((*p).addr3,"000000",6);
	strncpy ((*p).addr4,"000000",6);
	(*p).seqctrl = ack;
	(*p).buf = NULL;
	(*p).crc = _crc_ok;
	
	fb = set_frame_buffer (p);

	return (fb);
}

/* ------------------------------------------------------------------------- */

char* make_pseduo_frame (char* messaggio,int msglen,int seqctrl,char* mitt_mac6,char* dest_mac6,char prog_crc) {
	pframe_t *p = (pframe_t*) malloc (sizeof (pframe_t));
	char* fb;
	
	/* Impostiamo i dati del pacchetto */
	(*p).data = 1;					/* pacchetto dati */
	(*p).dtype = 1;					/* tipo di dati 1 */
	(*p).tods = 0;					/* Il pacchetto non e' per il mz ma per un'altra stazione */
	(*p).fromds = 0;
	(*p).rts = 0;						
	(*p).cts = 0;
	(*p).scan = 0;
	if (msglen < 100) (*p).duration = 5; else (*p).duration = 20;
	(*p).packetl = _pframe_other_len + msglen;
	strncpy ((*p).addr1,dest_mac6,6);
	strncpy ((*p).addr2,mitt_mac6,6);
	strncpy ((*p).addr3,"000000",6);
	strncpy ((*p).addr4,"000000",6);
	(*p).seqctrl = seqctrl;
	(*p).buf = messaggio;
	(*p).crc = prog_crc; /* (rand() % 128);	Utilizziamo il campo crc per mettere un numero casuale di pacchetto. Serve per evitare messaggi doppi */
	
	/* Impacchettiamo */
	fb = set_frame_buffer (p);

	return (fb);
}

/* ------------------------------------------------------------------------- */

void remove_frame_buffer (char* buffer) {
	/* Liberiamo lo spazio occupato dal buffer */
	free (buffer);
}

/* ------------------------------------------------------------------------- */

void remove_pframe (pframe_t* pf) {
	free (pf);
}

/* ------------------------------------------------------------------------- */
/* 
	Per verificare se un pacchetto è completo bisogna fare le seguenti valutazioni:
	Se il numero di byte ricevuti è inferiore alla lunghezza minima (_pframe_other_len)
	allora il pacchetto è sicuramente incompleto.
	Se è uguale o superiore alla lunghezza minima, allora bisogna andare a vedere 
	se il valore di _pack_len corrisponde con la lunghezza totale
*/

char complete_frame (int n,char* buf) {
	if (n<_pframe_other_len)				/* Dobbiamo avere un numero di caratteri sufficienti per valutare la lunghezza */
		return (FALSE); 	

	if (get_packet_len (buf) == n)
		return (TRUE);
	else
		return (FALSE);
	
}

/* ------------------------------------------------------------------------- */


