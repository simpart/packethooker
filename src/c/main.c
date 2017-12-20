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
extern char g_ifname[64];
/**
 * @brief hooker id list
 */
int gpkh_hooker_lst[] = {
    COM_HOOKID_TCPCON
};

/*** function ***/
int main (void) {
    int ret  = 0;
    int sock = 0;
    uint8_t buf[PIA_ETH_MAXFRM_SIZ] = {0};
    
    /* load config */
    ret = pkh_init();
    if (COM_OK != ret) {
        printf("failed init\n");
        return COM_NG;
    }
    
    /* init network */
    sock = ttr_nw_init(g_ifname, buf, sizeof(buf));
    if (TTR_NG == sock) {
        return COM_NG;
    }
    /* set packet receive callback function */
    ret = ttr_nw_rcvloop(sock, rcv_callback);
    if (TTR_NG == ret) {
        return COM_NG;
    }
    
    close(sock);
    return COM_OK;
}

/**
 * receive callback function
 * 
 * @param[in] buf : head pointer to receive frame
 * @param[in] size : receive size
 */
void rcv_callback(uint8_t *buf, size_t size) {
    int loop     = 0;
    int hook_cnt = sizeof(gpkh_hooker_lst)/sizeof(int);
    
    /* check size */
    if (sizeof(pia_ethhdr_t)+sizeof(pia_ipv4hdr_t) > size) {
        printf("minimum receive size : %u byte\n", (int)size);
        return;
    }
    
    /* check packet hooker */
    for (loop=0; loop < hook_cnt ;loop++) {
        if (COM_TRUE == pkh_istgt_hook(gpkh_hooker_lst[loop])) {
            pkh_chkhook(gpkh_hooker_lst[loop], buf, size);
        }
    }
}
/* end of file */
