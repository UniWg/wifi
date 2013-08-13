#ifndef _COSTANTI_H
#define _COSTANTI_H

/* frame buffer : lunghezza parte data */
#define _pframe_data_len 2000
/* frame buffer : lunghezza tutto il resto */
#define _pframe_other_len 39
/* dimensione massima in byte del frame buffer */
#define	_max_frame_buffer_size _pframe_data_len+_pframe_other_len
/* indice iniziale di Addr1, seqctrl, payload all'interno del frame buffer */
#define _addr1_pos 10
#define _seqctrl_pos 34
#define _payload_pos 38

#endif
