#include "general_include.h"

/* ------------------------------------------------------------------------- */
char tid2napp (void) {
	int i = 0;
	pthread_t tid = pthread_self ();

	while (tidentity [i] != tid) {
		i++;
		if (i == _nsta) {
			printf (_Cerror "Il thread %u non è una applicazione\n" _CColor_Off,(unsigned int)tid);
			exit (0);
		}
	}
	
	return (i+1);
}

/* ------------------------------------------------------------------------- */
void Send (char* mac_dest,char* buf,int msg_len) {
	char* fb;
	char mitt [6];
	char dest [6];
	int id = tid2napp ();	/* Numero dell'applicazione (1..4) */
		
	printf ("Stazione %d spedito un messaggio con Send\n",id);
		
	/* Ricaviamo il mac6 della stazione che spedisce ... */
	mac2str (_mac_stax [id-1],mitt);
	/* ... e quello della stazione che deve ricevere */
	mac2str (mac_dest,dest);
	
	/* Progressivo del pacchetto */
	prog_crc [id-1]++;
	if (prog_crc [id-1]>120)	prog_crc [id-1] = 5;
	
	/* Prepariamo il pacchetto ... */
	fb = make_pseduo_frame (buf,msg_len,1,mitt,dest,prog_crc [id-1]);
	
	/* ... e lo mettiamo in lista di trasmissione */
	pthread_mutex_lock (&mutexLTL [id-1]);
	fifo_push (LTL [id-1],fb);
	pthread_mutex_unlock (&mutexLTL [id-1]);
	
	{
		pframe_t *p;
		
		p = get_frame_buffer (fb);
		printf ("destinatario=%d # mittente=%d\n",mac2nsta ((*p).addr1),mac2nsta ((*p).addr2));
		free (p);
	}
}

/* ------------------------------------------------------------------------- */
void Recv (char* mittmac,char* buf,int* msg_len) {
	char pack [_max_frame_buffer_size];
	pframe_t *p;
	char ricevuto = FALSE;
	int i;
	int id = tid2napp ();	/* Numero dell'applicazione (1..4) */

	/* la Recv è bloccante */
	do {
		sleep (1);
		
		pthread_mutex_lock (&mutexLRL [id-1]);
		
		if (fifo_pop (LRL [id-1],pack) == TRUE) {
			p = get_frame_buffer (pack);
			ricevuto = TRUE;
		}
		
		pthread_mutex_unlock (&mutexLRL [id-1]);
	} while (ricevuto == FALSE);
	
	*msg_len = (*p).packetl - _pframe_other_len;
	/* Copiamo il contenuto del messaggio ... */
	for (i=0;i<(*p).packetl;i++)
		buf [i] = (*p).buf [i];
	/* ... e convertiamo il mac sorgente */
	str2mac ((*p).addr2,mittmac);
	
	/* Mettiamo il terminatore per consentire la stampa corretta */
	buf [*msg_len] = 0; 	/* messaggio */
	
	free (p);
}











