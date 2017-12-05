/**
 * @file conf.h
 * @brief  define for config function
 * @author simpart
 */
/*** include ***/

#ifndef __TCP_H__
#define __TCP_H__

/*** define ***/

/*** prototype ***/
/* connection */
void tcpcon_pktrcv (uint8_t *frm);
/* config */
int tcpcon_isenable (void);
int tcpcon_load (void);
/* checker */
void tcpcon_pktchk (uint8_t *frm);

/*** macro ***/

#endif
/* end of file */
