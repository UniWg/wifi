#ifndef _COSTANTI_H
#define _COSTANTI_H

/* impostazione stampa di debug */
#define STAMPE_DEBUG 1





/* impostazione macro */

/*------------------------------------*/
#if STAMPE_DEBUG == 1
	#define DEBUG_PRINT printf(
	#define END_DP )
#else
	#define DEBUG_PRINT null_print(
	#define END_DP )
#endif
/*------------------------------------*/


/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
void null_print (const char *format, ...);
/* ------------------------------------------------------------------------- */

#endif


