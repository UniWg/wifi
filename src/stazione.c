#include "general_include.h"

const int _campo_stax [] = {_campo_sta1,_campo_sta2,_campo_sta3,_campo_sta4};
const int _sta_di_stax [] = {_sta_di_sta1,_sta_di_sta2,_sta_di_sta3,_sta_di_sta4};
const char _mac_stax [][17] = {_mac_sta1,_mac_sta2,_mac_sta3,_mac_sta4};

/* ------------------------------------------------------------------------- */
void mac2str (const char* mac,char* asc) {
	int i,j;
	unsigned int n;
	char u [3];
	
	u [2] = 0;	/* terminatore */
	j = 0;
	for (i=0;i<16;i++) {
		if ((i==0) || (i==3) || (i==6) || (i==9) || (i==12) || (i==15)) {
			/* Prendiamo due caratteri (un numero hex) ... */
			u [0] = mac [i];
			u [1] = mac [i+1];
			/* ... li convertiamo prima in numero ... */
			sscanf (u,"%x",&n);
			/* ... e poi in ascii */
			asc [j] = (char) n; j++;
		}
	}
}

/* ------------------------------------------------------------------------- */
void str2mac (char* asc,char* mac) {
	char s [18];

	sprintf (s,"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
		(int)asc[0],(int)asc[1],(int)asc[2],(int)asc[3],(int)asc[4],(int)asc[5]);	
	strncpy (mac,s,17);	
}

/* ------------------------------------------------------------------------- */
void inizializza_stazioni (void) {
	int i;
	char* mac = (char*) malloc (sizeof (char) * 6);
	
	for (i=0;i<_nsta;i++) {
		stazione [i].indice = i+1;
		mac2str (_mac_stax [i],stazione [i].mac);	
		stazione [i].campo = _sta_di_stax [i];
	}

	free (mac);
}





