/**
 * @file conf.h
 * @brief  define for config function
 * @author simpart
 */
/*** include ***/
#include "tcpcon.h"

#ifndef __CNF_H__
#define __CNF_H__

/*** define ***/
#define CNF_INI_ENVAL -1
#define CNF_PATH     "/etc/pkthook/"
#define CNF_NAME_COM "pkthook"
#define CNF_NAME_TCN "tcpcon"


/*** struct ***/


/*** prototype ***/
/* ctrl */
int cnf_load(void);

/*** macro ***/

#endif
/* end of file */
