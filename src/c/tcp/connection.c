/**
 * @file connection.c
 * @brief tcp connection manager
 * @author simpart
 */
/*** include ***/
#include <stdio.h>
#include "com.h"
#include "tcp.h"
#include "protopia.h"

/*** global ***/

/*** function ***/
void tcpcon_pktrcv (uint8_t *frm) {
    int ret = 0;
    uint8_t *tcphdr = NULL;

    if (NULL == frm) {
        return;
    }
    
    ret = comprt_get_tcppkt(frm, &tcphdr);
//printf("[dbg] ret:%d\n", ret);
    if (COM_OK != ret) {
        return;
    }
    
    pia_tcp_dump((pia_tcphdr_t *) tcphdr);
}


/* end of file */
