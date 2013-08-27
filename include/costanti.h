#ifndef _COSTANTI_H
#define _COSTANTI_H

typedef struct sockaddr sa_t;
typedef struct sockaddr_in sain_t;

/* ------------------------------------------------------------------------- */
/* Costanti */

/* Porta ed indirizzo per la connessione con il mezzo condiviso */
#define _portaIP 6000
#define _indirizzoIP "127.0.0.1"
/* Dimensione del buffer di scmabio mezzo - stazioni */
#define _maxbuflen 2000
#define _Cerror _CBIRed		/* Colore dei messaggi di errore */
#define TRUE 1
#define FALSE 0

/* ------------------------------------------------------------------------- */
/* Sezione per le variabili di DEBUG */

/* impostazione stampa di debug */
#define STAMPE_DEBUG 1
#define MC_DEBUG 1
#define STA_DEBUG 1

/* impostazione macro */

/*------------------------------------*/
#if STAMPE_DEBUG == 1
	#define DEBUG_PRINT printf(_CBIYellow "<DEB> "
	#define END_DP _CColor_Off);
#else
	#define DEBUG_PRINT null_print(
	#define END_DP );
#endif
/*------------------------------------*/
#if MC_DEBUG == 1
	#define DEBUG_MC printf(_CBIYellow "<DEB> "
	#define END_MC _CColor_Off);
#else
	#define DEBUG_MC null_print(
	#define END_MC );
#endif
/*------------------------------------*/
#if STA_DEBUG == 1
	#define DEBUG_STA printf(_CBIYellow "<DEB> "
	#define END_STA _CColor_Off);
#else
	#define DEBUG_STA null_print(
	#define END_STA );
#endif

/*------------------------------------*/


/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
void null_print (const char *format, ...);
/* ------------------------------------------------------------------------- */

#endif


