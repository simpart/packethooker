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
int pkh_getip (uint8_t *frm, uint8_t **out) {
    
    /* check parameter */ 
    if ((NULL == frm) || (NULL == out)) {
        return COM_NG;
    }
    
    /* check next protocol */
    if (PIA_TRUE != pia_eth_isip((pia_ethhdr_t *) frm)) {
        return COM_RET_NMCH;
    }
    
    /* get ip header */
    *out = pia_eth_seekpld((pia_ethhdr_t *) frm);
    if (NULL == *out) {
        return COM_NG;
    }
    
    return COM_OK;
}

int pkh_gettcp (uint8_t *frm, uint8_t **opkt) {
    int ret = 0;
    uint8_t *seek = NULL;
    
    if ((NULL == frm) || (NULL == opkt)) {
        return COM_NG;
    }
    
    ret = pkh_getip(frm, &seek);
    if (COM_NG == ret) {
        return COM_NG;
    }
    
    /* check next protocol */
    if (PIA_TRUE != pia_ip_istcp((pia_ipv4hdr_t *) seek)) {
        return COM_RET_NMCH;
    }
    
    /* get tcp header */
    *opkt = pia_ip_seekpld((pia_ipv4hdr_t *) seek);
    if (NULL == *opkt) {
        return COM_NG;
    }
    
    return COM_OK;
}
/* end of file */
