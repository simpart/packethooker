/**
 * @file tcn/init.c
 */
/*** include ***/
#include <string.h>
#include "protopia.h"
#include "tetraring.h"
#include "com.h"
#include "tcpcon.h"

/*** global ***/
pkhtcn_cnf_t    g_pkhtcn_cnf;
/**
 * @brief connection chain
 */
int g_pkhtcn_conchn;   //! connection status chain
int g_pkhtcn_count;    //! connection counter chain

uint8_t g_pkhtcn_contgt[PIA_IP_IPSIZE];   // connection target ipaddress

/*** function ***/
int pkhtcn_init (void) {
    int ret = 0;
    
    /* create connection chain */
    g_pkhtcn_conchn = ttrchn_create();
    g_pkhtcn_count  = ttrchn_create();
    
    /* init global */
    memset(&g_pkhtcn_cnf,       0x00, sizeof(pkhtcn_cnf_t));
    memset(&g_pkhtcn_contgt[0], 0x00, PIA_IP_IPSIZE);
    
    ret = pkhtcn_loadconf();
    if (COM_NG == ret) {
       return COM_NG;
    }
    
    return COM_OK;
}
/* end of file */
