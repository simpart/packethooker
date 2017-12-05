/**
 * @file ctrl.c
 * @brief config manager
 * @author simpart
 */
/*** include ***/
#include "com.h"
#include "conf.h"

/*** global ***/

/*** function ***/
int cnf_load(void) {
    
    if (COM_TRUE == tcpcon_isenable()) {
        // load tcp-connection-hooker config
        tcpcon_load();
    }
    
    return COM_OK;
}
/* end of file */
