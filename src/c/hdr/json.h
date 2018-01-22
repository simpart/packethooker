/**
 * @file json.h
 * @brief  json header
 * @author simpart
 */
/*** include ***/

#ifndef __PKH_JSON_H__
#define __PKH_JSON_H__

/*** define ***/
#define DPKH_KEYLEN 32
#define DPKH_VALLEN 64
#define DPKH_ELMLEN 10

/*** struct ***/
typedef struct pkh_jsnelm {
    char key[DPKH_KEYLEN];
    char val[DPKH_VALLEN];
} pkh_jsnelm_t;


/*** prototype ***/
void pkhjsn_init (void);
int pkhjsn_add (char *, char *);
int pkhjsn_get (char *);

#endif
/* end of file */
