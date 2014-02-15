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
	if (n<_pframe_other_len)
		return (FALSE); 	/* La dimensione è inferiore alla minima */

	if (get_packet_len (buf) == n)
		return (TRUE);
	else
		return (FALSE);
	
}

/* ------------------------------------------------------------------------- */



/* !!!!!!!!!!!!!!!!!!!! DA CANCELLARE !!!!!!!!!!!!!!!!!!!!!!!!!!!!1 */

/* ------------------ */
void pseudo_test (void) {
	pframe_t *p = (pframe_t*) malloc (sizeof (pframe_t));
	char m [10] = "messaggioK";
	char* fb;
	int i;
	
	/* SCRITTURA */
	
	printf ("Test sulla struttura dati del frame buffer\n");
	printf ("\nOra scriviamo i dati ...\n");
	
	(*p).data = 1;
	(*p).dtype = 2;
	(*p).tods = 3;
	(*p).fromds = 4;
	(*p).rts = 5;
	(*p).cts = 6;
	(*p).scan = 7;
	(*p).duration = 127;
	(*p).packetl = _pframe_other_len + 10;
	strncpy ((*p).addr1,"AB44EF",6);
	strncpy ((*p).addr2,"LM76PQ",6);
	strncpy ((*p).addr3,"abcdef",6);
	strncpy ((*p).addr4,"lmnopq",6);
	(*p).seqctrl = 65532;
	(*p).buf = (char*)&m;
	(*p).crc = 98;	
	
	fb = set_frame_buffer (p);
	
	/* LETTURA */
	
	printf ("\nE poi li leggiamo ...\n");
	(*p).buf = NULL;
	remove_pframe (p);		/* Deallochiamo per essere certi che venga cancellato */
	
	p = get_frame_buffer (fb);
	
	printf ("Data	: %d\n",(int)(*p).data);
	printf ("Dtype 	: %d\n",(int)(*p).dtype);
	printf ("ToDS 	: %d\n",(int)(*p).tods);
	printf ("FromDS	: %d\n",(int)(*p).fromds);
	printf ("rts	: %d\n",(int)(*p).rts);
	printf ("cts 	: %d\n",(int)(*p).cts);
	printf ("scan 	: %d\n",(int)(*p).scan);
	printf ("durata	: %d\n",(int)(*p).duration);
	printf ("PLen	: %d\n",(int)(*p).packetl);
	printf ("addr1	: ");
	for (i=0;i<6;i++) {
		printf ("%c",(*p).addr1 [i]);
	}
	printf ("\naddr2	: ");
	for (i=0;i<6;i++) {
		printf ("%c",(*p).addr2 [i]);
	}
	printf ("\naddr3	: ");
	for (i=0;i<6;i++) {
		printf ("%c",(*p).addr3 [i]);
	}
	printf ("\naddr4	: ");
	for (i=0;i<6;i++) {
		printf ("%c",(*p).addr4 [i]);
	}
	printf ("\nSeqctr	: %d\n",(int)(*p).seqctrl);
	printf ("dati	: ");
	for (i=0;i<10;i++) 
		printf ("%c",(*p).buf[i]);
	printf ("\nCRC	: %d\n",(int)(*p).crc);
	
}








