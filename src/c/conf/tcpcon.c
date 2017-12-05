/**
 * @file tcpcon.c
 * @brief tcp connection config loader
 * @author simpart
 */
/*** include ***/
#include "com.h"
#include "conf.h"
#include "tcp.h"

/*** global ***/
int g_tcpcon_enable = CNF_INI_ENVAL;

/*** function ***/
int tcpcon_isenable (void) {
    if (CNF_INI_ENVAL == g_tcpcon_enable) {
        /* check enable */
        g_tcpcon_enable = COM_TRUE;
    }
    
    return g_tcpcon_enable;
}

int tcpcon_load (void) {
//    
//    
//    //g_cnf_hooklst
//    
//    
    return COM_OK;
}
/* end of file */
