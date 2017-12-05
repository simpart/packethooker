/**
 * @file main.c
 * @brief packet hooker main function
 * @author simpart
 */

/*** include ***/
//#include <stdio.h>
#include "tetraring.h"
#include "protopia.h"
#include "com.h"
#include "conf.h"


/*** global ***/
// hooker_lst

/*** define ***/
#define IFACE_NAME "ens8"

/*** function ***/
int main (void) {
    int ret = 0;
    uint8_t buf[PIA_ETH_MAXFRM_SIZ] = {0};
    
    /* init network */
    ret = ttr_nw_init(IFACE_NAME, buf, sizeof(buf));
    if (TTR_NG == ret) {
        return -1;
    }
    /* set packet receive callback function */
    ret = ttr_nw_rcvloop(rcv_callback);
    if (TTR_NG == ret) {
        return -1;
    }
    
    /* load config */
    cnf_load();
    
    return 0;
}

/**
 * receive callback function
 * 
 * @param[in] buf : head pointer to receive frame
 * @param[in] size : receive size
 */
void rcv_callback(uint8_t *buf, size_t size) {
//printf("receive\n");
    //uint8_t *seek = NULL;
//    uint8_t *ppkt      = NULL;
//    pia_ipv4hdr_t *pip = NULL;

    buf = buf;

    /* check size */
    if (sizeof(pia_ethhdr_t)+sizeof(pia_ipv4hdr_t) > size) {
        printf("minimum receive size : %u byte\n", (int)size);
        return;
    }
    
    if (COM_TRUE == tcpcon_isenable()) {
        tcpcon_pktrcv(buf);
    }
    
    
    
//    
//    if (PIA_TRUE == pia_eth_isip((pia_ethhdr_t *)buf)) {
//        ppkt = pia_eth_seekpld((pia_ethhdr_t *) buf);
//        pip  = (pia_ipv4hdr_t *) ppkt;
//        if (NULL == ppkt) {
//            return;
//        }
//        pia_ip_dump((pia_ipv4hdr_t *) ppkt);
//        printf("\n");
//        if (PIA_TRUE == pia_ip_istcp((pia_ipv4hdr_t *) ppkt)) {
//            ppkt = pia_ip_seekpld((pia_ipv4hdr_t *) ppkt);
//            if (NULL == ppkt) {
//                return;
//            }
//            pia_tcp_dump_detail((pia_tcphdr_t *) ppkt);
//            printf("\n");
//        }
//    }
}
/* end of file */
