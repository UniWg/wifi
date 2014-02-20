#include "general_include.h"

/* ------------------------------------------------------------------------- */

void cpmac (const char* src,char* dst) {
	char macapp [6];
	mac2str (src,macapp);
	strncpy (dst,macapp,6);
}

/* ------------------------------------------------------------------------- */
/* converte un carattere rappresentato in hex nel relativo decimale */
int chex2int (char c) {
	/* il carattere DEVE essere 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F */
	if ((c>47) && (c<58))	/* è un numero */
		return (c-48);
	return (c-55);			/* è una lettera */
}

/* ------------------------------------------------------------------------- */
void mac2str (const char* mac,char* asc) {
	/* Da 18 a 6 */
	int i,j;
	char u [2];
	
	j = 0;
	for (i=0;i<16;i+=3) {
		/* Prendiamo due caratteri (un numero hex) ... */
		u [0] = mac [i];
		u [1] = mac [i+1];
		/* ... e li convertiamo in numero ... */
		asc [j] = (chex2int (u [0])*16) + chex2int (u [1]);
		j++;
	}
}

/* ------------------------------------------------------------------------- */
/* Converte un numero hex della relativa coppia di caratteri */
void ihex2str (unsigned char numero,char* s) {
	int i,n;
	
	s[2] = 0;	/* terminatore */
	for (i=0;i<2;i++) {
		if (i==0)	n = (unsigned char)(numero / 16);
		else n = numero - (n * 16);
		if (n<10) s[i] = 48+n;		/* asc(48) = '0' */
		else s[i] = 65+n-10;		/* asc(65) = 'A' */
	}
}

/* ------------------------------------------------------------------------- */
void str2mac (char* asc,char* mac) {
	/* Da 6 a 18 */
	/* restituisce una stringa ben formata, quindi anche con il terminatore */
	char d [6][3];
	int i;
	
	for  (i=0;i<6;i++)	ihex2str ((unsigned char)asc[i],d[i]);
	snprintf (mac,18,"%s:%s:%s:%s:%s:%s",d[0],d[1],d[2],d[3],d[4],d[5]);
}
/* ------------------------------------------------------------------------- */
int mac2nsta (char* mac) {
	int n = -1;
	int i;
	
	for (i=0;i<_nsta;i++) {
		if (strncmp (mac,stazione_g [i].mac,6) == 0)
			return (stazione_g [i].indice);
	}
	return (n);
}

/* ------------------------------------------------------------------------- */

long getNOWmsec (void) {
	long epoca;
	timev_t t;
	
	gettimeofday (&t,NULL);		/* prendiamo il tempo attuale */
	epoca = t.tv_sec * 1000;	/* secondi : moltiplichiamo per 1000 per ottenere millisecondi */
	epoca += t.tv_usec/1000;	/* microsecondi : dividiamo per 1000 per ottenere millisecondi */
	return (epoca);
}


/* ------------------------------------------------------------------------- */

/* --------------------------------- */
list2* fifo_create (void) {
	list2* s=(list2*) malloc (sizeof (list2));
	
	/* La fifo ha l'elemento sentinella sempre presente */
	s->pack [0] = 0;
	s->next=s;
	s->prec=s;
	
	return (s);
}

/* --------------------------------- */
void fifo_push (list2* s,char* pack) {
	/* Inseriamo un elemento in testa */
	list2* p=(list2*) malloc (sizeof (list2));
	
	strncpy (p->pack,pack,_max_frame_buffer_size);	/* Si potrebbe ottimizzare andando a leggere la lunghezza reale del pacchetto */
	p->next = s->next;
	p->prec = s;
	s->next->prec = p;
	s->next = p;
}

/* --------------------------------- */
char fifo_read (list2* s,char* pack) {
	if (s->next == s)	return (FALSE);
	else {
		strncpy (pack,s->prec->pack,_max_frame_buffer_size);
		return (TRUE);
	}
}

/* --------------------------------- */
char fifo_pop (list2* s,char* pack) {
	list2* app;
	
	if (s->next == s)	return (FALSE);
	else {
		app = s->prec;
		
		/* Scolleghiamo app */
		app->prec->next = s;
		s->prec = app->prec;
		
		strncpy (pack,app->pack,_max_frame_buffer_size);
		free (app);
		return (TRUE);
	}
}

/* --------------------------------- */
char fifo_is_empty (list2* s) {
	if (s->next == s) return (TRUE);	/* Lista vuota */
	else return (FALSE);
}

/* --------------------------------- */







