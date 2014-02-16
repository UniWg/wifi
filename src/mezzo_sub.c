#include "general_include.h"

/* ------------------------------------------------------------------------- */
int stesso_dest (stato_t *s) {
	int i,j,k,conf;
	
	conf = 0;
	for (i=0;i<_nsta-1;i++) {			/* elemento da confrontare */
		conf = 0;
		for (j=i+1;j<_nsta;j++) {		/* elementi */
			for (k=0;k<6;k++) {			/* lunghezza mac */
				if ((*s).clibuf [i].stamacdest [k] == (*s).clibuf [j].stamacdest [k]) {
					if (k == 5)	{		/* Abbiamo trovato due elementi uguali */
						conf = conf & _campo_stax [i] & _campo_stax [j];
					}
				}
				else	k=6;	/* appena troviamo un elemento non corrispondente interrompiamo il ciclo */
			}
		}
	}
	return (conf);
}

/* ------------------------------------------------------------------------- */
int out_of_time (stato_t *s) {
	int i,conf,micros;
	timev_t t;
	
	/* Prendiamo il tempo attuale */
	gettimeofday (&t,NULL);
	
	conf = 0;
	for (i=0;i<_nsta;i++) {
		micros = (t.tv_sec - (*s).clibuf [i].timestamp.tv_sec)*1000;
		micros = micros + (int)((t.tv_usec - (*s).clibuf [i].timestamp.tv_usec) / 1000);
	}
	
	return (conf);
}

/* ------------------------------------------------------------------------- */

/* ######################################################################### */
/* ############### VITA DEL MEZZO ########################################## */
/* ######################################################################### */

/* ------------------------------------------------------------------------- */
void prendi_pacchetto (stato_t *s,char* pack) {
	int j,n;

	/* Controlliamo quali descrittori sono settati */
	for (j=0;j<_nsta;j++) {
		if (FD_ISSET ((*s).clientfd [j],&(*s).Rset)) {
			/* Leggo il frame e lo metto nel buffer che restituisco (pack) */
			do {
				n = recv ((*s).clientfd [j], pack, _max_frame_buffer_size, 0);
			} while ((n<0) && (errno==EINTR));
		
			break;	
		}
	}
}

/* ------------------------------------------------------------------------- */
char area_libera (pframe_t* f,area_t* aree) {
	/* L'area è occupata se il valore di "durata" è maggiore di zero.
		L'area deve essere dedotta da f. 
		In particolare la stazione 4 dovrà avere entrambe le aree libere per poter trasmettere */
	
	int i,power2=1,s = mac2nsta ((*f).addr2);
	char ris = TRUE;
	
	for (i=0;i<_n_area;i++) {
		if (_area_stax [s-1] & power2) {
			if (aree [i].durata > 0) {	/* Se in un area è impostata una durata allora l'area è occupata */
				ris = FALSE;
				break;
			}
		}
		power2 *= 2;		/* potenze di 2 da confrontare con la bitmap della stazione */
	}
	
	return (ris);
}

/* ------------------------------------------------------------------------- */
void occupa_area (pframe_t* f,area_t* aree,char* pack) {
	int i,j,power2=1,s = mac2nsta ((*f).addr2);
	timev_t t;
	long epoca;

	/* 
		Nel caso la stazione sia la 4 allora deve mettere lo stesso pacchetto in entrambe
		le aree perchè sarà da spedire a tutti
	*/ 

	for (i=0;i<_n_area;i++) {
		if (_area_stax [s-1] & power2) {
			/* Copiamo il pacchetto nell'area in modo da simulare il pacchetto in transito.
			Da ora in poi si considererà questa copia del pacchetto */	
			for (j=0;j<(*f).packetl;j++)
				aree [i].pack [j]= pack [j];
		
			/* Impostiamo la durata del pacchetto a seconda del tipo 
				500ms per pacchetti RTS, CTS o < 100 byte
				2000ms per tutti gli altri
			*/
			
			for (j=0;j<10;j++) {		mettere a posto la duration
			gettimeofday (&t,NULL);
			epoca = t.tv_sec * 1000;
			epoca = t.tv_usec;
			printf ("EPOCH : %ld\n",epoca);
			
			if ((*f).rts == 1 || (*f).cts || (*f).packetl < 100)	aree [i].durata = _packet_duration_low;
			else 	aree [i].durata = _packet_duration_hi;
	
			/* Da questo momento l'area risulta occupata */
		}
		power2 *= 2;
	}
}

/* ------------------------------------------------------------------------- */
void libera_area (pframe_t* f) {

}

/* ------------------------------------------------------------------------- */
void marca_frame_corrotto_e_spedisci (char* pack,pframe_t* f) {

}

/* ------------------------------------------------------------------------- */
void svuota_buffer_area (pframe_t* f) {

}

/* ------------------------------------------------------------------------- */
int prendi_pacchetto_dal_buffer (char* pack) {

	return (0);
}

/* ------------------------------------------------------------------------- */
char genera_errore_casuale (void) {

	return (TRUE);
}

/* ------------------------------------------------------------------------- */
void spedisci_pacchetto (char* pack,pframe_t* f) {

}

/* ------------------------------------------------------------------------- */
void elimina_pacchetto_dal_buffer (pframe_t* f)  {

}

/* ------------------------------------------------------------------------- */
char conflitto_di_destinazioni (void) {

	return (TRUE);
}

/* ------------------------------------------------------------------------- */

char scaduto_timer (int area_attiva, area_t* aree) {

	return (TRUE);
}

/* ------------------------------------------------------------------------- */

char pacchetto_in_area (area_t*  aree) {

	return (TRUE);
}

/* ------------------------------------------------------------------------- */

/* ######################################################################### */
/* ######################################################################### */
/* ######################################################################### */









