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








