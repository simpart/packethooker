/**
 * @file  com/conf.c
 * @brief util function for config
 * @author simpart
 */
#include <stdio.h>
#include <string.h>
#include "com.h"
#include "conf.h"

ttr_conf_t * pkh_getconf (char *cnf_nm) {
    char cnf_pth[64] = {0};
    
    if (NULL == cnf_nm) {
        return NULL;
    }
    
    memset(&cnf_pth[0],  0x00, sizeof(cnf_pth));
    
    snprintf(
        &cnf_pth[0],
        sizeof(cnf_pth),
        "%s%s.cnf",
        CNF_PATH,
        cnf_nm
    );
    
    return ttrcnf_load(cnf_pth);
}

int pkh_getval (ttr_conf_t *cnf, char *key, char *out) {
    int loop    = 0;
    int set_idx = 0;
    ttr_conf_t *get_cnf = NULL;
    
    /* check parameter */
    if ((NULL == cnf) || (NULL == key) || (NULL == out)) {
        return COM_NG;
    }
    
    /* init parameter */
    memset(out, 0x00, TTRCNF_VALCNT);
    
    /* search value */
    get_cnf = ttrcnf_find(cnf, key);
    if (NULL == get_cnf) {
        printf("could not find key\n");
        return COM_NG;
    }
    
    /* remove double quotation */
    for (loop=0;loop < TTRCNF_VALCNT;loop++) {
        if ('\0' == get_cnf->val[loop]) {
            break;
        }
        if ('"' == get_cnf->val[loop]) {
            continue;
        }
        out[set_idx] = get_cnf->val[loop];
        set_idx++;
    }
    
    return COM_OK;
}
/* end of file */
