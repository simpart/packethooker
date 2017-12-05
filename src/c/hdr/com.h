/**
 * @file com.h
 * @brief common defined
 * @author simpart
 */
/*** include ***/
#include "protopia.h"

#ifndef __COM_H__
#define __COM_H__

/*** define ***/
/**
 * @brief value of function return
 */
#define COM_OK  0  //! function is successful
#define COM_NG -1  //! function is failed

#define COM_TRUE  1
#define COM_FALSE 0

#define COM_RET_NMCH 10  //! not match

/*** prototype ***/
void rcv_callback(uint8_t *, size_t);
/* proto */
int comprt_get_tcppkt (uint8_t *, uint8_t **);

/*** macro ***/

#endif
/* end of file */
