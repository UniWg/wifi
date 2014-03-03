#include "general_include.h"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void* app_thread1 (void* param) {
	applica_t s = *(applica_t*)param;	
	int na = s.napp-1; 					/* togliamo 1 così ns è allineato con l'indice dell'array */
	char buf [_pframe_data_len+1];
	char mittmac [18];					/* mac del mittente */
	int msg_len;
	
	tidentity [na] = pthread_self ();
	printf (_Capp "Applicazione %d attiva (ID %u)\n" _CColor_Off,na+1,(unsigned int)tidentity [na]);
	
	/* -------- */
	
	switch (s.ricetta) {
		case  _sim_ping_pong : {
			/* Inizia la stazione 1 */
			strcpy (buf,"Simulazione \"Ping Pong\" ");
			while (1) {
				/* Aggiungo il mio identificativo al messaggio ... */
				strcat (buf,"-STA1");
				/* ... mando il messaggio a sta2 ... */
				Send (_mac_sta2,buf,strlen (buf));
				
				/* ... e aspetto la risposta */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
			}
			break;
		}
		case _sim_underground : {
			/* Inizia la stazione 1 */
			strcpy (buf,"Simulazione \"Underground\" ");
			while (1) {
				/* Aggiungo il mio identificativo al messaggio ... */
				strcat (buf,"-STA1");
				/* ... mando il messaggio a sta4 ... */
				Send (_mac_sta4,buf,strlen (buf));
				
				/* ... e aspetto la risposta */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
			}
			break;
		}
		case _sim_pain_chain : {
			while (1) {
				/* Aspetto il messaggio ... */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
				
				/* ... aggiungo il mio identificativo ... */
				strcat (buf,"-sta1");
				/* ... e spedisco la risposta a sta2 */
				Send (_mac_sta2,buf,strlen (buf));
			}
			break;
		}
		case _sim_crazy_ball : {
			while (1) {
				/* Aspetto il messaggio ... */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
				
				/* ... aggiungo il mio identificativo ... */
				strcat (buf,"-sta1");
				/* ... e spedisco la risposta a sta2 o 4 */
				switch (rand() % 2) {
					case (0) : Send (_mac_sta2,buf,strlen (buf)); break;
					case (1) : Send (_mac_sta4,buf,strlen (buf)); break;
				}
			}
			break;
		}
		case _sim_fast_n_furious : {
			/* Inizia la stazione 1 */
			strcpy (buf,"Simulazione \"Fast & Furious\" - STA1");
			/* ... mando il messaggio a sta2 e sta4... */
			Send (_mac_sta2,buf,strlen (buf));
			Send (_mac_sta4,buf,strlen (buf));
			
			/* ... e aspetto le risposte */
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione FAST : %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione FURIOUS : %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
			printf ("\n Success !! \n\n");
			exit (0);
			break;
		}
		case _sim_fast_n_furious_2 : {
			/* Aspetto il messaggio ... */
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
			
			/* ... aggiungo il mio identificativo ... */
			strcat (buf,"-sta1");
			/* ... e spedisco la risposta */
			Send (_mac_sta4,buf,strlen (buf));
			break;
		}
		case _sim_simple_crash : {
			/* Non partecipo */
			break;
		}
		case _sim_mega_crash : {
			/* Le stazioni 1,2,3 spediscono contemporaneamente un messaggio a sta4 */
			strcpy (buf,"Simulazione \"Mega Crash\" ");

			/* Aggiungo il mio identificativo al messaggio ... */
			strcat (buf,"-STA1");
			/* ... mando il messaggio a sta4 ... */
			Send (_mac_sta4,buf,strlen (buf));
			
			/* ... e aspetto la risposta */
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
				
			/* Aggiungo il mio identificativo al messaggio ... */
			strcat (buf,"-STA1");
			/* ... mando il messaggio conclusivo a sta4 ... */
			Send (_mac_sta4,buf,strlen (buf));	
			break;
		}
	}
	
	return 0;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void* app_thread2 (void* param) {
	applica_t s = *(applica_t*)param;	
	int na = s.napp-1; 					/* togliamo 1 così ns è allineato con l'indice dell'array */
	char buf [_pframe_data_len+1];
	char mittmac [18];					/* mac del mittente */
	int msg_len;
	
	tidentity [na] = pthread_self ();
	printf (_Capp "Applicazione %d attiva (ID %u)\n" _CColor_Off,na+1,(unsigned int)tidentity [na]);
	
	/* -------- */
	
	switch (s.ricetta) {
		case  _sim_ping_pong : {
			while (1) {
				/* Aspetto il messaggio ... */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
				
				/* ... aggiungo il mio identificativo ... */
				strcat (buf,"-sta2");
				/* ... e spedisco la risposta */
				Send (_mac_sta1,buf,strlen (buf));
			}
			break;
		}
		case _sim_underground : {
			/* Non partecipo */
			break;
		}
		case _sim_pain_chain : {
			/* Inizia la stazione 2 */
			strcpy (buf,"Simulazione \"Pain Chain\" ");
			while (1) {
				/* Aggiungo il mio identificativo al messaggio ... */
				strcat (buf,"-STA2");
				/* ... mando il messaggio a sta4 ... */
				Send (_mac_sta4,buf,strlen (buf));
				
				/* ... e aspetto la risposta */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
			}
			break;
		}
		case _sim_crazy_ball : {
			while (1) {
				/* Aspetto il messaggio ... */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
				
				/* ... aggiungo il mio identificativo ... */
				strcat (buf,"-sta2");
				/* ... e spedisco la risposta a sta1 o 4 */
				switch (rand() % 2) {
					case (0) : Send (_mac_sta1,buf,strlen (buf)); break;
					case (1) : Send (_mac_sta4,buf,strlen (buf)); break;
				}
			}
			break;
		}
		case _sim_fast_n_furious : {
			/* Aspetto il messaggio ... */
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
			
			/* ... aggiungo il mio identificativo ... */
			strcat (buf,"-sta2");
			/* ... e spedisco la risposta */
			Send (_mac_sta1,buf,strlen (buf));
			break;
		}
		case _sim_fast_n_furious_2 : {
			/* Aspetto il messaggio ... */
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
			
			/* ... aggiungo il mio identificativo ... */
			strcat (buf,"-sta2");
			/* ... e spedisco la risposta */
			Send (_mac_sta4,buf,strlen (buf));
			break;
		}
		case _sim_simple_crash : 
		case _sim_mega_crash : {
			/* Le stazioni 1,2,3 spediscono contemporaneamente un messaggio a sta4 */
			if (s.ricetta == _sim_simple_crash) strcpy (buf,"Simulazione \"Simple Crash\" ");
			else 								strcpy (buf,"Simulazione \"Mega Crash\" ");

			/* Aggiungo il mio identificativo al messaggio ... */
			strcat (buf,"-STA2");
			/* ... mando il messaggio a sta4 ... */
			Send (_mac_sta4,buf,strlen (buf));
			
			/* ... e aspetto la risposta */
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
				
			/* Aggiungo il mio identificativo al messaggio ... */
			strcat (buf,"-STA2");
			/* ... mando il messaggio conclusivo a sta4 ... */
			Send (_mac_sta4,buf,strlen (buf));	
				
			break;
		}
	}

	return 0;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void* app_thread3 (void* param) {
	applica_t s = *(applica_t*)param;	
	int na = s.napp-1; 					/* togliamo 1 così ns è allineato con l'indice dell'array */
	char buf [_pframe_data_len+1];
	char mittmac [18];					/* mac del mittente */
	int msg_len;
	
	tidentity [na] = pthread_self ();
	printf (_Capp "Applicazione %d attiva (ID %u)\n" _CColor_Off,na+1,(unsigned int)tidentity [na]);
	
	/* -------- */
	
	switch (s.ricetta) {
		case  _sim_ping_pong : {
			/* Non partecipo */
			break;
		}
		case _sim_underground : 
		case _sim_pain_chain : 
		case _sim_crazy_ball : {
			while (1) {
				/* Aspetto il messaggio ... */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
				
				/* ... aggiungo il mio identificativo ... */
				strcat (buf,"-sta3");
				/* ... e spedisco la risposta a sta4 */
				Send (_mac_sta4,buf,strlen (buf));
			}
			break;
		}
		case _sim_fast_n_furious : {
			/* Non partecipo */
			break;
		}
		case _sim_fast_n_furious_2 : {
			/* Aspetto il messaggio ... */
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
			
			/* ... aggiungo il mio identificativo ... */
			strcat (buf,"-sta3");
			/* ... e spedisco la risposta */
			Send (_mac_sta4,buf,strlen (buf));
			break;
		}
		case _sim_simple_crash : 
		case _sim_mega_crash : {
			/* Le stazioni 1,2,3 spediscono contemporaneamente un messaggio a sta4 */
			if (s.ricetta == _sim_simple_crash) strcpy (buf,"Simulazione \"Simple Crash\" ");
			else 								strcpy (buf,"Simulazione \"Mega Crash\" ");

			/* Aggiungo il mio identificativo al messaggio ... */
			strcat (buf,"-STA3");
			/* ... mando il messaggio a sta4 ... */
			Send (_mac_sta4,buf,strlen (buf));
			
			/* ... e aspetto la risposta */
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
				
			/* Aggiungo il mio identificativo al messaggio ... */
			strcat (buf,"-STA3");
			/* ... mando il messaggio conclusivo a sta4 ... */
			Send (_mac_sta4,buf,strlen (buf));	
				
			break;
		}
	}
	
	return 0;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void* app_thread4 (void* param) {
	applica_t s = *(applica_t*)param;	
	int na = s.napp-1; 					/* togliamo 1 così ns è allineato con l'indice dell'array */
	char buf [_pframe_data_len+1];
	char mittmac [18];					/* mac del mittente */
	char mac6 [6];						/* mac in formato ascii */
	int msg_len;
	
	tidentity [na] = pthread_self ();
	printf (_Capp "Applicazione %d attiva (ID %u)\n" _CColor_Off,na+1,(unsigned int)tidentity [na]);
	
	/* -------- */
	
	switch (s.ricetta) {
		case  _sim_ping_pong : {
			/* Non partecipo */
			break;
		}
		case _sim_underground :
		case _sim_pain_chain : {
			while (1) {
				/* Aspetto il messaggio ... */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
				
				/* ... aggiungo il mio identificativo ... */
				strcat (buf,"-sta4");
				/* ... e spedisco la risposta a sta1 o sta3 a seconda del mittente */
				mac2str (mittmac,mac6);
				if (mac2nsta (mac6) == 3)	Send (_mac_sta1,buf,strlen (buf));
				else						Send (_mac_sta3,buf,strlen (buf));
			}
			break;
		}
		case _sim_crazy_ball : {
			/* Inizia la stazione 4 */
			strcpy (buf,"Simulazione \"Crazy Ball\" ");
			while (1) {
				/* Aggiungo il mio identificativo al messaggio ... */
				strcat (buf,"-STA4");
				/* ... mando il messaggio a sta1, 2 o 3 ... */
				switch (rand() % 3) {
					case (0) : Send (_mac_sta1,buf,strlen (buf)); break;
					case (1) : Send (_mac_sta2,buf,strlen (buf)); break;
					case (2) : Send (_mac_sta3,buf,strlen (buf)); break;
				}
				
				/* ... e aspetto la risposta */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
			}
			break;
		}
		case _sim_fast_n_furious : {
			/* Aspetto il messaggio ... */
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
			
			/* ... aggiungo il mio identificativo ... */
			strcat (buf,"-sta4");
			/* ... e spedisco la risposta */
			Send (_mac_sta1,buf,strlen (buf));
			break;
		}
		case _sim_fast_n_furious_2 : {
			/* Inizia la stazione 4 */
			strcpy (buf,"Simulazione \"Fast & Furious 2\" - STA4");
			/* ... mando il messaggio a sta1, sta2 e sta3... */
			Send (_mac_sta1,buf,strlen (buf));
			Send (_mac_sta3,buf,strlen (buf));
			Send (_mac_sta2,buf,strlen (buf));
			
			/* ... e aspetto le risposte */
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione FAST : %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione FURIOUS : %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
			Recv (mittmac,buf,&msg_len);
			printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione FURIOUS 2 : %s\n" _CColor_Off,na+1,mittmac);
			printf (_Capp "%s \n\n" _CColor_Off,buf);
			printf ("\n Success !! \n\n");
			exit (0);
			break;
		}
		case _sim_simple_crash : {
			int i;
			for (i=0;i<2;i++) {
				/* Aspetto il messaggio ... */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
				
				/* ... aggiungo il mio identificativo ... */
				strcat (buf,"-sta4");
				/* ... e spedisco la risposta al mittente */
				mac2str (mittmac,mac6);
				if (mac2nsta (mac6) == 3)	Send (_mac_sta3,buf,strlen (buf));
				else						Send (_mac_sta2,buf,strlen (buf));
			}
			
			for (i=0;i<2;i++) {
				/* Aspetto le risposte conclusive ... */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
			}
			
			printf ("\n Success !! \n\n");
			exit (0);
			break;
		}
		case _sim_mega_crash : {
			int i;
			for (i=0;i<3;i++) {
				/* Aspetto il messaggio ... */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
				
				/* ... aggiungo il mio identificativo ... */
				strcat (buf,"-sta4");
				/* ... e spedisco la risposta al mittente */
				mac2str (mittmac,mac6);
				if (mac2nsta (mac6) == 1)		Send (_mac_sta1,buf,strlen (buf));
				else if (mac2nsta (mac6) == 2)	Send (_mac_sta2,buf,strlen (buf));
				else							Send (_mac_sta3,buf,strlen (buf));
			}
			
			for (i=0;i<3;i++) {
				/* Aspetto le risposte conclusive ... */
				Recv (mittmac,buf,&msg_len);
				printf (_Capp "Applicazione %d - Ricevuto messaggio da applicazione %s\n" _CColor_Off,na+1,mittmac);
				printf (_Capp "%s \n\n" _CColor_Off,buf);
			}
			
			printf ("\n Success !! \n\n");
			exit (0);
			break;
		}
	}
	
	return 0;
}

/* ------------------------------------------------------------------------- */
void start_app_thread (int ricetta) {
	/* Lancia i thread di ogni applicazione.
		Ai thread verrà  passato solo un numero progressivo (1..4) che troverá una
		associazione implicita con il numero delle stazioni. La struttura
		dati relativa è accessibile a livello globale */
	int r,i;
	applica_t *s;

	for (i=1; i<=_nsta; i++) {
		LTL [i-1] = fifo_create ();
    	LRL [i-1] = fifo_create ();
    	prog_crc [i-1] = 5;
	
		s = (applica_t*) malloc (sizeof(applica_t));
		(*s).napp = i;
		(*s).ricetta = ricetta;
		
		switch (i) {
			case 1 : r = pthread_create (&mc_thread_g, NULL, app_thread1, s); break;
			case 2 : r = pthread_create (&mc_thread_g, NULL, app_thread2, s); break;
			case 3 : r = pthread_create (&mc_thread_g, NULL, app_thread3, s); break;
			case 4 : r = pthread_create (&mc_thread_g, NULL, app_thread4, s); break;
		}
		if (r) {
			printf ("Errore nella creazione del thread dell'applicazione numero %d\n", i);
			printf ("Codice di errore riportato da pthread_create(): %d\n",r);
			exit(-1);
		}
	}
}
