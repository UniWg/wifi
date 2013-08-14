#include "general_include.h"

const int _campo_stax [] = {_campo_sta1,_campo_sta2,_campo_sta3,_campo_sta4};
const int _sta_di_stax [] = {_sta_di_sta1,_sta_di_sta2,_sta_di_sta3,_sta_di_sta4};
const char _mac_stax [][17] = {_mac_sta1,_mac_sta2,_mac_sta3,_mac_sta4};

/* ------------------------------------------------------------------------- */
/* Converte il mac address in ASCII */
void mac2str (int sta,char* mac) {
	int i,n,j;
	char u [3];
	
	u [2] = 0;	/* terminatore */
	j = 5;
	for (i=16;i>1;i--) {
		if ((i==16) || (i==13) || (i==10) || (i==7) || (i==4) || (i==1)) {
			/* Prendiamo due caratteri (un numero) ... */
			u [1] = _mac_stax [sta][i];
			u [0] = _mac_stax [sta][i-1];
			/* ... li convertiamo prima in numero ... */
			n = atoi (u);
			/* ... e poi in ascii */
			mac [j] = (char) n; j--;
		}
	}
}

/* ------------------------------------------------------------------------- */
void inizializza_stazioni (void) {
	int i,j;
	char* mac = (char*) malloc (sizeof (char) * 6);
	
	for (i=0;i<_nsta;i++) {
		stazione [i].indice = i+1;
		mac2str (i,mac);
		for (j=5;j>0;j--)
			printf ("%c",mac [j]);
		strncpy (stazione [i].mac,mac,6);
		stazione [i].campo = _sta_di_stax [i];
	}
	
	free (mac);
}





