/**
 * @file json.c
 * @brief json generator
 * @note this is workaround file
 */
/*** include ***/
#include <string.h>
#include "com.h"
#include "json.h"

/*** global ***/
pkh_jsnelm_t g_jsnlst[DPKH_ELMLEN];

/*** function ***/
void pkhjsn_init () {
    memset(&g_jsnlst[0], 0x00, sizeof(pkh_jsnelm_t));
}

int pkhjsn_add (char *key, char *val) {
    int loop = 0;
    pkh_jsnelm_t chk_emp;
    
    /* check parameter */
    if ((NULL == key) || (NULL == val)) {
        return COM_NG;
    }
    
    /* init local */
    memset(&chk_emp, 0x00, sizeof(pkh_jsnelm_t));
    
    /* get index  */
    for (loop=0;loop < DPKH_ELMLEN;loop++) {
        if (0 != memcmp(&(g_jsnlst[loop]), &chk_emp, sizeof(pkh_jsnelm_t)) ) {
            continue;
        }
        /* this is empty element, copy key-val */
        strncpy(&(g_jsnlst[loop].key[0]), key, DPKH_KEYLEN);
        strncpy(&(g_jsnlst[loop].val[0]), val, DPKH_VALLEN);
        
        return COM_OK;
    }
    
    return COM_NG;
}

int pkhjsn_get (char *out) {
    int loop = 0;
    int seek = 0;
    pkh_jsnelm_t chk_emp;
    char ret[512] = {0};
    
    /* check parameter */
    if (NULL == out) {
        return COM_NG;
    }
    
    /* init local */
    memset(&chk_emp, 0x00, sizeof(pkh_jsnelm_t));
    memset(&ret[0], 0x00, sizeof(ret));
    
    ret[seek] = '{';
    seek++;
    
    for (loop=0;loop < DPKH_ELMLEN;loop++) {
        if (0 == memcmp(&(g_jsnlst[loop]), &chk_emp, sizeof(pkh_jsnelm_t)) ) {
            /* this is empty element, copy key-val */
            break;
        }
        /* set key */
        ret[seek]   = '\\';
        ret[seek+1] = '"';
        seek += 2;
        strncpy(&(ret[seek]), &(g_jsnlst[loop].key[0]), DPKH_KEYLEN);
        seek += strnlen(&(g_jsnlst[loop].key[0]),DPKH_KEYLEN);
        ret[seek]   = '\\';
        ret[seek+1] = '"';
        seek += 2;
        
        ret[seek] = ':';
        seek++;
        
        /* set value */
        strncpy(&(ret[seek]), &(g_jsnlst[loop].val[0]), DPKH_VALLEN);
        seek += strnlen(&(g_jsnlst[loop].val[0]),DPKH_VALLEN);
        
        ret[seek] = ',';
        seek++;
    }
    
    ret[seek-1] = ' ';
    ret[seek] = '}';
    strncpy(out, &ret[0], sizeof(ret));
    
    return COM_OK;
}
/* end of file */
