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
				+500ms per pacchetti RTS, CTS o < 100 byte
				+2000ms per tutti gli altri
			*/
			if (aree [i].durata != 0) {
				printf ("<occupa_area> errore critico. \"Durata\" area %d non è a zero",i+1);  
				exit (1);
			}
			aree [i].durata = getNOWmsec ();	/* epoch msec */
			if ((*f).rts == 1 || (*f).cts || (*f).packetl < 100)	aree [i].durata += _packet_duration_low;
			else 	aree [i].durata += _packet_duration_hi;
	
			/* Da questo momento l'area risulta occupata */
		}
		power2 *= 2;
	}
}

/* ------------------------------------------------------------------------- */
void marca_errore_per_collisione (pframe_t* f,area_t* aree) {
	int i,power2=1,s = mac2nsta ((*f).addr2);
	
	/* Potenzialmente sta4 potrebbe generare collisioni su entrambe le aree */
	
	for (i=0;i<_n_area;i++) {
		if (_area_stax [s-1] & power2) {
			/* Siamo in una delle aree di appartenenza del pacchetto (potrebbe essere solo questa) */	
			/* Marchiamo errore sull´area */
			set_CRC0 (i,aree);
		}
		power2 *= 2;
	}
}

/* ------------------------------------------------------------------------- */
void marca_arrivo (pframe_t* f,area_t* aree) {
	int i,power2=1,s = mac2nsta ((*f).addr2);

	for (i=0;i<_n_area;i++) {
		if (_area_stax [s-1] & power2) {
			/* Siamo in una delle aree di appartenenza del pacchetto (potrebbe essere solo questa) */	
			/* Marchiamo tempo sull´area */
			aree [i].arrivo = getNOWmsec ();
		}
		power2 *= 2;
	}
}

/* ------------------------------------------------------------------------- */
/* Il contatore di pacchetti non viene inizializzato, tanto il valore ha solo importanza relativa */
void genera_errore_casuale (area_t* aree) {
	int i;
	char errore;
	
	if (_abilita_errori == TRUE) {
	
		for (i=0;i<_n_area;i++) {
			/* L'errore viene generato solo se c'è un pacchetto in corso di tramissione */
			if ((aree [i].durata > 0) && (aree [i].errore_in_corso == FALSE)) {
				errore = FALSE;
				if (aree [i].contatore_di_pacchetti > 1000) {
					aree [i].contatore_di_pacchetti = 0;
					errore = TRUE;
				} 
				else {
					if ((aree [i].contatore_di_pacchetti % (100 / _percento_errori)) == 0) {
						aree [i].contatore_di_pacchetti++;
						errore = TRUE;
					}
				}
	
				if (errore == TRUE) {
					/* Abbiamo generato un errore. Dobbiamo indicarlo e mettere a zero il CRC del pacchetto */
					set_CRC0 (i,aree);
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------- */
void get_dest (int index,area_t* aree,char* dest) {
	pframe_t* f;

	f = get_frame_buffer (aree [index].pack);	
	strncpy (dest,(*f).addr1,6);
	free (f);
}

/* ------------------------------------------------------------------------- */
void controlla_conflitto_di_destinazioni (area_t* aree) {
	int i;
	char d1 [6],d2 [6];
	
	/* Per semplicitá non cicliamo sulle aree, ma consideriamo solo le due aree attive */
	/* Diamo errore se entrambe le aree vogliono trasmettere alla stazione 4 (quella in comune) */
	
	/* Ha senso fare la verifica solo se entrambe le aree hanno un pacchetto */
	if ((aree[0].durata > 0) && (aree[1].durata > 0)) {	
		get_dest (0,aree,d1);	
		get_dest (1,aree,d2);
		/* if (strncmp (d1,d2,6) == 0) { */
		if ((mac2nsta (d1) == 4) && (mac2nsta (d2) == 4)) {
			/* I destinatari delle due aree sono uguali. Marchiamo errore su entrambe le aree */
			for (i=0;i<=1;i++) {
				set_CRC0 (i,aree);
			}
				
			/* A questo punto il pacchetto che viene scartato non viene mandato a nessuna stazione dell'area
				quindi la stazione che aveva mandato (per esempio) un RTS non si vedrà arrivare risposta e
				rileverà l'errore causa timeout */	
			
			
			if ((aree [0].arrivo > 0) && (aree [1].arrivo > 0)) {
				if (aree [0].arrivo < aree [1].arrivo)	i=0;
				else	i=1;	
			
				aree [i].durata = 0;
				aree [i].spedita_prima_parte = FALSE;
				aree [i].errore_in_corso = FALSE;
				aree [i].spedita_prima_parte_errore = FALSE;
			}
		}
	}
}

/* ------------------------------------------------------------------------- */

char scaduto_timer (area_t* aree) {
	long epoca;
	int i;	
	char ris = FALSE;
	
	epoca = getNOWmsec ();	/* epoch msec */
	for (i=0;i<_n_area;i++)	{
		if ((aree [i].durata>0) && (aree [i].durata<=epoca)) {
			ris = TRUE;
			break;
		}
	}

	return (ris);
}

/* ------------------------------------------------------------------------- */

char pacchetto_in_area (area_t* aree) {
	int i;
	char ris = FALSE;
	
	for (i=0;i<_n_area;i++)	{
		if (aree [i].durata != 0) {	
			ris = TRUE;					/* Abbiamo almeno un'area occupata */
			break;
		}
	}

	return (ris);
}

/* ------------------------------------------------------------------------- */

void spedisci_prima_parte_pacchetto (stato_t *s,area_t* aree) {
	int i,k,mitt,len,nwrite,n;
	pframe_t* f;
	
	for (i=0;i<_n_area;i++)	{
		if ((aree [i].durata > 0) && (aree [i].spedita_prima_parte == FALSE)) {
			/* Spediamo gli n-1 byte di questo pacchetto a tutte le stazioni dell'area tranne che al mittente */
			
			f = get_frame_buffer (aree [i].pack);		/* Prendiamo il pacchetto da spedire */
			mitt = mac2nsta ((*f).addr2);				/* numero stazione mittente */
			len = (*f).packetl-1;						/* Non spediamo l'ultimo byte */	

			for (k=0;k<_nsta;k++) {
				/* and logico tra il numero della stazione e la configurazione del campo di visibilità del mittente */
				/* in pratica serve per dedurre le stazioni raggiungibili */
				if ((_campo_stax [k] & _sta_di_stax [mitt-1]) && (_area_stax [k] & (i+1)) && (k!=(mitt-1))) {
					nwrite=0;
					while ((n = write((*s).clientfd [k], aree [i].pack, len-nwrite)) >0)
						nwrite+=n;
					#if MC_DEBUG == 1
						printf (_Cmezzo "%d Spedito primi n-1 byte a stazione %d\n" _CColor_Off,mitt,k+1);
					#endif
				}
			}

			aree [i].spedita_prima_parte = TRUE;	/* I primi n-1 byte sono stati spediti */
			
			free (f);
		}
	}
}

/* ------------------------------------------------------------------------- */

void spedisci_ultimo_byte (stato_t *s,area_t* aree) {
	long epoca;
	int i,k,mitt,len,nwrite,n;
	pframe_t* f;
	char buf [1];
	
	epoca = getNOWmsec ();	/* epoch msec */
	for (i=0;i<_n_area;i++)	{
		if ((aree [i].durata > 0) && (aree [i].spedita_prima_parte == TRUE) && (aree [i].durata<=epoca)) {
			/* Spediamo l'ultimo  byte di questo pacchetto a tutte le stazioni dell'area tranne che al mittente */
			
			f = get_frame_buffer (aree [i].pack);		/* Prendiamo il pacchetto da spedire */
			mitt = mac2nsta ((*f).addr2);				/* numero stazione mittente */
			len = 1;										
			buf [0] = aree [i].pack [(*f).packetl-1];	/* Spediamo solo l'ultimo byte */
			
			for (k=0;k<_nsta;k++) {
				/* and logico tra il numero della stazione e la configurazione del campo di visibilità del mittente */
				/* in pratica serve per dedurre le stazioni raggiungibili */
				if ((_campo_stax [k] & _sta_di_stax [mitt-1]) && (_area_stax [k] & (i+1)) && (k!=(mitt-1))) {
					nwrite=0;
					while ((n = write((*s).clientfd [k], buf, len-nwrite)) >0)
						nwrite+=n;
					#if MC_DEBUG == 1
						printf (_Cmezzo "%d Spedito ultimo byte a stazione %d\n" _CColor_Off,mitt,k+1);
					#endif
				}
			}
			
			/* Togliamo il pacchetto dal buffer */
			/* Se ci sono errori allora non devono essere resettati, ma lasciati in evidenza */
			reset_area (i,aree);
			
			/* Incrementiamo il numero di pacchetti spediti (serve per il calcolo degli errori) */
			aree [i].contatore_di_pacchetti++;
			
			if ((*f).crc == 0) {
				aree [i].errore_in_corso = TRUE;
				aree [i].spedita_prima_parte_errore = FALSE;
				aree [i].durata = epoca + _packet_duration_low;
			}
			
			free (f);
		}
	}
}

/* ------------------------------------------------------------------------- */
void reset_area (int index,area_t* aree) {
	aree [index].durata = 0;
	aree [index].spedita_prima_parte = FALSE;
}

/* ------------------------------------------------------------------------- */
void set_CRC0 (int index,area_t* aree) {
	pframe_t* f;
	
	/* Individuiamo la lunghezza del pacchetto ... */
	f = get_frame_buffer (aree [index].pack);
	
	/* ... e mettiamo a zero l'ultimo byte, quello corrispondente al CRC */
	aree [index].pack [(*f).packetl-1] = 0;
	
	free (f);
}

/* ------------------------------------------------------------------------- */
char qualche_area_in_errore (area_t* aree) {
	int i;
	
	for (i=0;i<_n_area;i++) {
		if (aree [i].errore_in_corso == TRUE) {
			return (TRUE);
		}	
	}
	
	return (FALSE);
}

/* ------------------------------------------------------------------------- */
char area_in_errore (pframe_t* f,area_t* aree) {
	/* L'area deve essere dedotta da f. 
		In particolare la stazione 4 dovrà avere entrambe le aree prive di errori per poter trasmettere */
	
	int i,power2=1,s = mac2nsta ((*f).addr2);
	char ris = FALSE;
	
	for (i=0;i<_n_area;i++) {
		if (_area_stax [s-1] & power2) {
			if (aree [i].errore_in_corso == TRUE) {	/* E' sufficiente avere un'area in errore */
				ris = TRUE;
				break;
			}
		}
		power2 *= 2;		/* potenze di 2 da confrontare con la bitmap della stazione */
	}
	
	return (ris);
}

/* ------------------------------------------------------------------------- */
void spedisci_prima_parte_crc0_mittente (stato_t *s,area_t* aree) {
	int i,mitt,len,nwrite,n;
	pframe_t* f;
	
	for (i=0;i<_n_area;i++)	{
		if ((aree [i].durata > 0) && (aree [i].spedita_prima_parte_errore == FALSE) && (aree [i].errore_in_corso == TRUE)) {
			/* Spediamo gli n-1 byte di questo pacchetto a tutte le stazioni dell'area tranne che al mittente */
			
			f = get_frame_buffer (aree [i].pack);		/* Prendiamo il pacchetto da spedire */
			mitt = mac2nsta ((*f).addr2);				/* numero stazione mittente (1..4) */
			len = (*f).packetl-1;						/* Non spediamo l'ultimo byte */	


			nwrite=0;
			while ((n = write((*s).clientfd [mitt-1], aree [i].pack, len-nwrite)) >0)
				nwrite+=n;
			#if MC_DEBUG == 1
				printf (_Cmezzo "%d Spedito primi n-1 byte a stazione mittente %d\n" _CColor_Off,mitt,mitt);
			#endif 
			
			
			aree [i].spedita_prima_parte_errore = TRUE;	/* I primi n-1 byte sono stati spediti */
			
			free (f);
		}
	}
}

/* ------------------------------------------------------------------------- */
void spedisci_ultimo_byte_crc0_mittente (stato_t *s,area_t* aree) {
	long epoca;
	int i,mitt,len,nwrite,n;
	pframe_t* f;
	char buf [1];
	
	epoca = getNOWmsec ();	/* epoch msec */
	for (i=0;i<_n_area;i++)	{
		if ((aree [i].durata > 0) && (aree [i].durata<=epoca) && (aree [i].spedita_prima_parte_errore == TRUE) && (aree [i].errore_in_corso == TRUE)) {
			/* Spediamo l'ultimo  byte di questo pacchetto a tutte le stazioni dell'area tranne che al mittente */
			
			f = get_frame_buffer (aree [i].pack);		/* Prendiamo il pacchetto da spedire */
			mitt = mac2nsta ((*f).addr2);				/* numero stazione mittente */
			len = 1;										
			buf [0] = aree [i].pack [(*f).packetl-1];	/* Spediamo solo l'ultimo byte */

			
			nwrite=0;
			while ((n = write((*s).clientfd [mitt-1], buf, len-nwrite)) >0)
				nwrite+=n;
			#if MC_DEBUG == 1
				printf (_Cmezzo "%d Spedito ultimo byte a stazione %d\n" _CColor_Off,mitt,mitt);
			#endif

			
			/* Togliamo il pacchetto dal buffer */
			/* Se ci sono errori allora non devono essere resettati, ma lasciati in evidenza */
			reset_area (i,aree);
			
			aree [i].errore_in_corso = FALSE;
			aree [i].spedita_prima_parte_errore = FALSE;
			
			free (f);
		}
	}
}

/* ------------------------------------------------------------------------- */

/* ######################################################################### */
/* ######################################################################### */
/* ######################################################################### */









