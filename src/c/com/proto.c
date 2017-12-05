/**
 * @file proto.c
 * @brief common protocol function
 * @author simpart
 */
/*** include ***/
#include <stdio.h>
#include "protopia.h"
#include "com.h"


/*** function ***/
int comprt_get_tcppkt (uint8_t *frm, uint8_t **opkt) {
    uint8_t *seek = NULL;
    
    if ((NULL == frm) || (NULL == opkt)) {
        return COM_NG;
    }
    
    /* check next protocol */
    if (PIA_TRUE != pia_eth_isip((pia_ethhdr_t *) frm)) {
printf("is not ip-pkt\n");
        return COM_RET_NMCH;
    }
    
    /* get ip header */
    seek = pia_eth_seekpld((pia_ethhdr_t *) frm);
    if (NULL == seek) {
        return COM_NG;
    }
    
    /* check next protocol */
    if (PIA_TRUE != pia_ip_istcp((pia_ipv4hdr_t *) seek)) {
printf("is not tco-pkt\n");
        return COM_RET_NMCH;
    }
    
    /* get tcp header */
    *opkt = pia_ip_seekpld((pia_ipv4hdr_t *) seek);
    if (NULL == opkt) {
        return COM_NG;
    }
    
    return COM_OK;
}
/* end of file */
