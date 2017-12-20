/**
 * @file
 * @brief
 * @author simpart
 */
/*** include ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "com.h"
#include "conf.h"
#include "tcpcon.h"

/*** global ***/
extern pkhtcn_cnf_t g_pkhtcn_cnf;

int pkhtcn_loadconf (void) {
    ttr_conf_t *cnf  = NULL;
    int  ret         = 0;
    char cnf_val[64] = {0};
    
    /* init value */
    memset(&cnf_val[0],   0x00, sizeof(cnf_val));
    
    /* load config */
    cnf = pkh_getconf(CNF_NAME_TCN);
    if (NULL == cnf) {
        return COM_NG;
    }
    
    /* enabled tcp-conn hooker */
    phk_swhhook(COM_HOOKID_TCPCON, COM_TRUE);
    
    /* get HOOK_TYPE value */
    ret = pkh_getval(cnf, "HOOK_TYPE", &cnf_val[0]);
    if (COM_NG == ret) {
        return COM_NG;
    }
    
    ret = pkhtcn_gettpid(&cnf_val[0]);
    if (COM_NG == ret) {
        return COM_NG;
    }
    g_pkhtcn_cnf.type = ret;
    
    ret = pkhtcn_loadconf_count(cnf);
    if (COM_NG == ret) {
        return COM_NG;
    }
//    /* get COUNT value */
//    ret = pkh_getval(cnf, "COUNT", &cnf_val[0]);
//    if (COM_NG == ret) {
//        return COM_NG;
//    }
//    ret = atoi(&cnf_val[0]);
//    if ((0 == ret) || (0 > ret)) {
//        return COM_NG;
//    }
//    g_pkhtcn_cnf.count = ret;
    
    /* get LISTEN_PORT */
    ret = pkh_getval(cnf, "LISTEN_PORT", &cnf_val[0]);
    if (COM_NG == ret) {
        return COM_NG;
    }
    ret = atoi(&cnf_val[0]);
    if ((0 > ret) || (65535 < ret)) {
        return COM_NG;
    }
    g_pkhtcn_cnf.port = ret;
    
    return COM_OK;
}

int pkhtcn_loadconf_count (ttr_conf_t *cnf) {
    int  ret         = 0;
    int  loop        = 0;
    int  buf_idx     = 0;
    int  cnt_idx     = 0;
    char cnf_val[64] = {0};
    char cnf_buf[64] = {0};
    int  cnt_lst[DPKHTCN_CNF_MAXCNT] = {0};
    
    if (NULL == cnf) {
        return COM_NG;
    }
    
    /* init value */
    memset(&cnf_val[0], 0x00, 64);
    memset(&cnf_buf[0], 0x00, 64);
    memset(&cnt_lst[0], -1, sizeof(int) * DPKHTCN_CNF_MAXCNT);
    
    /* get COUNT value */
    ret = pkh_getval(cnf, "COUNT", &cnf_val[0]);
    if (COM_NG == ret) {
        return COM_NG;
    }
    
//    printf("%s\n", cnf_val);
//    
    for (loop=0;loop < 64;loop++) {
        if (',' == cnf_val[loop]) {
            /* get number */
            ret = atoi(&cnf_buf[0]);
            if ((0 == ret) || (0 > ret)) {
                return COM_NG;
            }
            cnt_lst[cnt_idx] = ret;
            cnt_idx++;
            buf_idx = 0;
            memset(&cnf_buf[0], 0x00, 64);
            continue;
        }
        /* set buff area */
        cnf_buf[buf_idx] = cnf_val[loop];
        buf_idx++;
    }
    ret = atoi(&cnf_buf[0]);
    if ((0 == ret) || (0 > ret)) {
        return COM_NG;
    }
    cnt_lst[cnt_idx] = ret;
    
    for (loop=0;loop < DPKHTCN_CNF_MAXCNT;loop++) {
        if (-1 == cnt_lst[loop]) {
            break;
        }
        //printf("count: %u\n", cnt_lst[loop]);
        g_pkhtcn_cnf.count[loop] = cnt_lst[loop];
    }
    
    return COM_OK;
}

int pkhtcn_gettpid (char *type) {
    /* check parameter */
    if (NULL == type) {
        return COM_NG;
    }
    /* convert type id from type string */
    if (0 == strncmp(DPKHTCN_TPSTR_MAX, type, TTRCNF_VALCNT)) {
        return DPKHTCN_TPID_MAX;
    } else if (0 == strncmp(DPKHTCN_TPSTR_MIN, type, TTRCNF_VALCNT)) {
        return DPKHTCN_TPID_MIN;
    } else {
        return COM_NG;
    }
    return COM_OK;
}

/* end of file */
