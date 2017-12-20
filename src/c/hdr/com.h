/**
 * @file com.h
 * @brief common defined
 * @author simpart
 */
/*** include ***/
#include "protopia.h"
#include "tetraring.h"

#ifndef __COM_H__
#define __COM_H__

/*** define ***/
/**
 * @brief value of function return
 */
#define COM_OK  0  //! function is successful
#define COM_NG -1  //! function is failed
#define COM_RET_NMCH 10  //! not matched
/**
 * @brief flag value
 */
#define COM_TRUE  1
#define COM_FALSE 0
/**
 * @brief defined hook id
 */
#define COM_HOOKID_TCPCON 0x00



/*** global ***/

/*** prototype ***/
/* main.c */
void rcv_callback(uint8_t *, size_t);
/* hookctl.c */
int pkh_init (void);
int pkh_init_com (void);
int pkh_init_hook (void);
int pkh_istgt_hook (int);
int pkh_chkhook (int, uint8_t *, size_t);
int phk_swhhook (int, int);
/* proto.c */
int pkh_getip (uint8_t *, uint8_t **);
int pkh_gettcp (uint8_t *, uint8_t **);
/* conf.c */
ttr_conf_t * pkh_getconf (char *); 
int pkh_getval (ttr_conf_t *, char *, char *);
/*** macro ***/

#endif
/* end of file */
