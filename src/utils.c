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
void nsta2mac (int n,char* mac) {
	int i;
	
	if (n<=_nsta) {
		mac2str (_mac_stax [n-1],mac);
	} 
	else {						/* Errore. Mettiamo il mac a zero */
		for (i=0;i<6;i++)		
			mac [i] = '0';
	}
}
<<<<<<< HEAD


=======


>>>>>>> 346a338a7aa1cad0d74e95cd014eba2f56eb8b0a
/* ------------------------------------------------------------------------- */
long getNOWmsec (void) {
	long epoca;
	timev_t t;
	
<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> 346a338a7aa1cad0d74e95cd014eba2f56eb8b0a
	gettimeofday (&t,NULL);					/* prendiamo il tempo attuale */
	epoca = (t.tv_sec & 0xFFFFF) * 1000;	/* secondi : teniamo le prime 5 cifre e moltiplichiamo per 1000 per ottenere millisecondi */
	epoca += t.tv_usec / 1000 ;				/* microsecondi : dividiamo per 1000 per ottenere millisecondi */

<<<<<<< HEAD
	return (epoca);
}

=======
=======
	gettimeofday (&t,NULL);		/* prendiamo il tempo attuale */
	epoca = (t.tv_sec & 0xFFFFF) * 1000;	/* secondi : teniamo le prime 5 cifre e moltiplichiamo per 1000 per ottenere millisecondi */
	epoca += t.tv_usec/1000;	/* microsecondi : dividiamo per 1000 per ottenere millisecondi */
>>>>>>> 6b6edd2a24a65bb52b1e54931c083fb505f9fbdd
	return (epoca);
}

/* ------------------------------------------------------------------------- */
int tempo_progressivo (void) {
	timev_t t;
	
	gettimeofday (&t,NULL);							/* prendiamo il tempo attuale */
	return (((int)t.tv_sec & 0x63) +1 );			/* Restituiamo le prime 2 cifre dei secondi */
}


>>>>>>> 346a338a7aa1cad0d74e95cd014eba2f56eb8b0a
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
	int i;
	/* Inseriamo un elemento in testa */
	list2* p=(list2*) malloc (sizeof (list2));
	
	/*Si potrebbe ottimizzare andando a leggere la lunghezza reale del pacchetto */
	for (i=0;i<_max_frame_buffer_size;i++)
		p->pack [i]=pack [i];
	
	p->next = s->next;
	p->prec = s;
	s->next->prec = p;
	s->next = p;
}

/* --------------------------------- */
char fifo_read (list2* s,char* pack) {
	int i;
	if (s->prec == s)	return (FALSE);
	else {
		for (i=0;i<_max_frame_buffer_size;i++)
			pack [i]=s->prec->pack [i];
		return (TRUE);
	}
}

/* --------------------------------- */
char fifo_read_deep (list2* s,char* pack,int n) {
	int i;
	list2* p = s;
	
	/* Ci portiamo sull'elemento richiesto ... */
	for (i=0;i<n;i++) {
		p = p->prec;
		if (p == s) 	return (FALSE);
	}
	
	/* ... e lo leggiamo */	
	for (i=0;i<_max_frame_buffer_size;i++)
		pack [i]=p->pack [i];
	return (TRUE);	
}

/* --------------------------------- */
char fifo_pop (list2* s,char* pack) {
	list2* app;
	int i;
	
	if (s->prec == s)	return (FALSE);
	else {
		app = s->prec;
		
		/* Scolleghiamo app */
		app->prec->next = s;
		s->prec = app->prec;
		
		for (i=0;i<_max_frame_buffer_size;i++)
			pack [i]=app->pack [i];
		free (app);
		return (TRUE);
	}
}

/* --------------------------------- */
void fifo_reset (list2* s) {
	list2* p;
	
	/* Cancelliamo gli elementi ... */
	p = s->next;
	while (p != s) {
		p = p->next;
		free (p->prec);
	}
	
	/* ... e richiudiamo la sentinella su se stessa */
	s->next = s;
	s->prec = s;
}

/* --------------------------------- */
char fifo_is_empty (list2* s) {
	if (s->next == s) return (TRUE);	/* Lista vuota */
	else return (FALSE);
}

/* --------------------------------- */







