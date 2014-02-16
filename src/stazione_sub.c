#include "general_include.h"

/* ------------------------------------------------------------------------- */

int appendi_pacchetto (char* dst, char* src, int nnow, int nadd) {
	int i;
	
	printf ("Appendi pacchetto %d %d\n",nnow,nadd);
	for (i=0;i<nadd;i++)	{
		dst [nnow+i] = src [i];
	}
	return (nnow+nadd);
}

/* ------------------------------------------------------------------------- */
