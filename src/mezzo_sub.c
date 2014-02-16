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
	int i,s = mac2nsta ((*f).addr2);

	/* Copiamo il pacchetto nell'area in modo da simulare il pacchetto in transito.
	Da ora in poi si considererà questa copia del pacchetto */	
	for (i=0;i<(*f).packetl;i++)
		aree [s-1].pack [i]= pack [i];
		
	/* Impostiamo la durata del pacchetto a seconda del tipo */
	aree [s-1].durata = siamo qui
}

/* ------------------------------------------------------------------------- */
void libera_area (pframe_t* f) {

}

/* ------------------------------------------------------------------------- */
void metti_pacchetto_nel_buffer (char* pack,pframe_t* f) {

}

/* ------------------------------------------------------------------------- */
void marca_frame_corrotto_e_spedisci (char* pack,pframe_t* f) {

}

/* ------------------------------------------------------------------------- */
void svuota_buffer_area (pframe_t* f) {

}

/* ------------------------------------------------------------------------- */
char buffer_pieno (void) {

	return (TRUE);
}

/* ------------------------------------------------------------------------- */
void prendi_pacchetto_dal_buffer (char* pack) {

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

/* ######################################################################### */
/* ######################################################################### */
/* ######################################################################### */









