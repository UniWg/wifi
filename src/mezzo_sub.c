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
void prendi_pacchetto (char* pack) {

}

/* ------------------------------------------------------------------------- */
char area_libera (pframe_t* f) {
	
	return (TRUE);
}

/* ------------------------------------------------------------------------- */
void occupa_area (pframe_t* f) {

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









