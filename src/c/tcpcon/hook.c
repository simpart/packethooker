/**
 * @file   hook.c
 * @brief  check hook for tcp-connection
 * @author simpart
 */
/*** include ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protopia.h"
#include "com.h"
#include "conf.h"
#include "tcpcon.h"
#include "json.h"

/*** global ***/
extern pkhtcn_cnf_t g_pkhtcn_cnf;
extern int g_pkhtcn_count;

/*** function ***/
int pkhtcn_chkhook (uint8_t *pkt, size_t siz) {
    pia_ipv4hdr_t *iphdr  = NULL;
    pia_tcphdr_t  *tcphdr = NULL;
    pkhtcn_counter_t *cnt = NULL;
    int  ret       = 0;
    char json[512] = {0};
    
    siz = siz;
    
    memset(&json[0], 0x00, sizeof(json));
    
    /* get header */
    ret = pkh_gettcp(pkt, (uint8_t **) &tcphdr);
    if (COM_NG == ret) {
        return COM_NG;
    }
    ret = pkh_getip(pkt, (uint8_t **) &iphdr);
    if (COM_NG == ret) {
        return COM_NG;
    }
    
    /* check port */
    if (COM_TRUE != pkhtcn_islisten(tcphdr)) {
        return COM_OK;
    }
//pia_ip_dump(iphdr);
//pia_tcp_dump_detail(tcphdr);
    
    /* update connection status */
    ret = pkhtcn_chkconn(iphdr, tcphdr);
    if (COM_NG == ret) {
        printf("tcp-connection error \n");
        ttrchn_close();
        exit(-1);
    } else if (DPKHTCN_NOCHG != ret) {
        /* changed counter */
        /* get connection counter */
        cnt = pkhtcn_getcounter(iphdr, ret);
        if (NULL == cnt) {
            printf("tcp-connection error \n");
            ttrchn_close();
            exit(-1);
        }
        /* check kick hook */
        if (COM_TRUE == pkhtcn_iskick(cnt)) {
            ret = pkhtcn_getjson(cnt, json);
            if (COM_NG == ret) {
                printf("tcp-connection error \n");
                ttrchn_close();
                exit(-1);
            }
            ret = pkh_hook(json);
            if (COM_OK != ret) {
                return COM_NG;
            }
        }
    }
    
    return COM_OK;
}

int pkhtcn_islisten (pia_tcphdr_t *tcphdr) {
    int dport = 0;
    int sport = 0;
    
    if (NULL == tcphdr) {
         return COM_NG;
    }
    /* get dest port */
    dport = pia_tcp_getport(tcphdr, PIA_TCP_DPORT);
    if (PIA_NG == dport) {
        return COM_NG;
    }
    /* get src port */
    sport = pia_tcp_getport(tcphdr, PIA_TCP_SPORT);
    if (PIA_NG == sport) {
        return COM_NG;
    }
    
    /* check port */
    if ( (g_pkhtcn_cnf.port == dport) ||
         (g_pkhtcn_cnf.port == sport) ) {
        return COM_TRUE;
    }
    
    return COM_FALSE;
}

pkhtcn_counter_t * pkhtcn_getcounter(pia_ipv4hdr_t *iphdr, int con_sts) {
    ttrchn_t         *chain = NULL;
    pkhtcn_counter_t *count = NULL;
    int ret = 0;
    int hit = COM_FALSE;
    
    if (NULL == iphdr) {
        return NULL;
    }
    
    /* get counter */
    chain = ttrchn_gethead(g_pkhtcn_count);
    if (NULL == chain) {
        return NULL;
    }
    chain = (ttrchn_t *) chain->next;  // head is empty
    while (NULL != chain) {
        count = (pkhtcn_counter_t *) chain->conts;
        if (NULL == count) {
            return NULL;
        }
//        /* check dest ip (dst is server) */
        if (0 == memcmp(&(count->ipaddr[0]), &(iphdr->dip[0]), PIA_IP_IPSIZE)) {
            hit = COM_TRUE;
            break;
        }
        /* get next chain */
        chain = (ttrchn_t *) chain->next;
    }
    if (COM_TRUE != hit) {
        /* add counter */
        count = (pkhtcn_counter_t *) malloc(sizeof(pkhtcn_counter_t));
        if (NULL == count) {
            return NULL;
        }
        memset(count, 0x00, sizeof(pkhtcn_counter_t));
        memcpy(&(count->ipaddr[0]), &(iphdr->dip[0]), PIA_IP_IPSIZE);
        ret = ttrchn_add(g_pkhtcn_count, count);
        if (TTR_NG == ret) {
            return NULL;
        }
    }
    
    if (DPKHTCN_ESTB == con_sts) {
        /* increase counter */
        count->count++;
        count->diff = 1;
    } else if (DPKHTCN_REL == con_sts) {
        /* decrease counter */
        if (0 != count->count) {
            count->count--;
        }
        count->diff = -1;
    } else {
        /* not change count */
        count->diff = 0;
    }
     
    return count;
}

int pkhtcn_iskick (pkhtcn_counter_t *cnt) {
    int loop = 0;
    
    /* check parameter */
    if (NULL == cnt) {
        return COM_NG;
    }
    
    if (0 == cnt->diff) {
        /* count is not change */
        return COM_FALSE;
    }
    
    if (DPKHTCN_TPID_MAX == g_pkhtcn_cnf.type) {
        if (0 >= cnt->diff) {
            /* hook is only increase */
            return COM_FALSE;
        }
        for (loop=DPKHTCN_CNF_MAXCNT - 1; loop >= 0 ;loop--) {
            if (0 == g_pkhtcn_cnf.count[loop]) {
                continue;
            }
            if (cnt->count == g_pkhtcn_cnf.count[loop]) {
                return COM_TRUE;
            }
        }
        
    } else if (DPKHTCN_TPID_MIN == g_pkhtcn_cnf.type) {
        /* not supported yet */
    } else {
        /* unknown hook type */
        return COM_NG;
    }
    
    return COM_FALSE;
}

int pkhtcn_getjson (pkhtcn_counter_t *cnt, char *json) {
    char addr_str[32] = {0};
    char cnt_str[16]  = {0};
    char tmp_str[64]  = {0};
    
    if ((NULL == cnt) || (NULL == json)) {
        return COM_NG;
    }
    
    /* set analyzer parameter */
    pkhjsn_add("analyzer", "\\\"tcp-connection-counter\\\"");
    
    /* set type parameter */
    pkhjsn_add("type", "\\\"exceed\\\"");
    
    /* set serv_ip parameter */
    memset(&addr_str[0], 0x00, sizeof(addr_str));
    snprintf(
        &addr_str[0], sizeof(addr_str),
        "\\\"%u.%u.%u.%u\\\"",
        cnt->ipaddr[0],cnt->ipaddr[1],cnt->ipaddr[2],cnt->ipaddr[3]
    );
    pkhjsn_add("serv_ip", &addr_str[0]);
    
    /* set count parameter */
    memset(&cnt_str[0], 0x00, sizeof(cnt_str));
    snprintf(
        &cnt_str[0], sizeof(cnt_str),
        "%u",
        cnt->count
    );
    pkhjsn_add("count", &cnt_str[0]);
    
    /* message parameter */
    memset(&tmp_str[0], 0x00, sizeof(tmp_str));
    snprintf(
        &tmp_str[0], sizeof(tmp_str),
        "\\\"%u.%u.%u.%u tcp-connection exceeded %s counts.\\\"",
        cnt->ipaddr[0],cnt->ipaddr[1],cnt->ipaddr[2],cnt->ipaddr[3],
        &cnt_str[0]
    );
    pkhjsn_add("message", &tmp_str[0]);
    
    pkhjsn_get(json);
    pkhjsn_init();
    
    return COM_OK;
}
/* end of file */
