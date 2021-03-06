/**
 * @file  hookctl.c
 * @brief hook controller
 */
/*** include ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tetraring.h"
#include "com.h"
#include "conf.h"
#include "json.h"

/*** global ***/
char g_ifname[64] = {0};
int  g_hookswh[] = {
    COM_FALSE      //! tcp-connection
};


/*** function ***/
int pkh_init (void) {
    int ret = 0;
    
    ret = pkh_init_com();
    if (COM_OK != ret) {
        return COM_NG;
    }
    
    ret = pkh_init_hook();
    if (COM_OK != ret) {
        return COM_NG;
    }
    
    return COM_OK;
}


int pkh_init_com (void) {
    int ret = 0;
    char cnf_val[64]     = {0};
    ttr_conf_t * com_cnf = NULL;

    memset(&g_ifname[0], 0x00, sizeof(g_ifname));
    pkhjsn_init();
    
    /* load config */
    com_cnf = pkh_getconf(CNF_NAME_COM);
    if (NULL == com_cnf) {
        printf("failed load common config\n");
        return COM_NG;
    }
    
    /* get interface name */
    ret = pkh_getval(com_cnf, "CAPTURE_IFACE", &cnf_val[0]);
    if (COM_NG == ret) {
        return COM_NG;
    }
    
    strncpy(&g_ifname[0], &cnf_val[0], sizeof(cnf_val));
    
    ttrcnf_free(com_cnf);
    return COM_OK;
}

int pkh_init_hook (void) {
    int ret = 0;
    /* load tcp-connection */
    ret = pkhtcn_init();
    if (COM_OK != ret) {
        return ret;
    }
    
    return COM_OK;
}

int pkh_istgt_hook (int hid) {
    /* check parameter */
    if ( (0 > hid) ||
         (((int) (sizeof(g_hookswh)/sizeof(int))) <= hid) ) {
        return COM_NG;
    }
    return g_hookswh[hid];
}

int pkh_chkhook (int hid, uint8_t *buf, size_t siz) {
    
    /* check parameter */
    if ( (0 > hid) ||
         (((int) (sizeof(g_hookswh)/sizeof(int))) <= hid) ) {
        return COM_NG;
    }
    
    if (COM_HOOKID_TCPCON == hid) {
        pkhtcn_chkhook(buf, siz);
    } else {
        return COM_NG;
    }
    
    return COM_OK;
}

int phk_swhhook (int hid, int flag) {
    /* check parameter */
    if ((COM_TRUE != flag) && (COM_FALSE != flag)) {
        return COM_NG;
    }
    if ( (0 > hid) ||
         (((int) (sizeof(g_hookswh)/sizeof(int))) <= hid) ) {
        return COM_NG;
    }
    g_hookswh[hid] = flag;
    return COM_OK;
}


int pkh_hook (char *msg) {
    int  ret = 0;
    char hook_cmd[DPKH_HOKMSG_STRCNT] = {0};
    
    /* check parameter */
    if ((NULL == msg) || (0 == strlen(msg))) {
        return COM_NG;
    }
    
    memset(&hook_cmd[0], 0x00, sizeof(hook_cmd));
    snprintf(
        &hook_cmd[0],
        DPKH_HOKMSG_STRCNT,
        "nohup php %sphp/webhook.php \"%s\" &",
        DPKH_SRCPATH,
        msg
    );
    
    ret = system(hook_cmd);
    if (COM_OK != ret) {
        return COM_NG;
    }
    
    return COM_OK;
}
/* end of file */
