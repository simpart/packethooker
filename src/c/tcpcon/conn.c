/**
 * @file   conn.c
 * @brief  connection functions
 * @author simpart
 */
/*** include ***/
#include <stdlib.h>
#include <string.h>
#include "protopia.h"
#include "tetraring.h"
#include "com.h"
#include "conf.h"
#include "tcpcon.h"

/*** global ***/
extern int g_pkhtcn_conchn;

/*** function ***/
int pkhtcn_chkconn (pia_ipv4hdr_t *iphdr, pia_tcphdr_t *tcphdr) {
    int ret = 0;
    int idx = 0;
    pkhtcn_conmng_t *con_sts = NULL;
    
    /* check parameter */
    if ((NULL == iphdr) || (NULL == tcphdr)) {
        return COM_NG;
    }
    
    idx = pkhtcn_getmngidx(iphdr, tcphdr);
    if (COM_NG == idx) {
        return COM_NG;
    }
    if (DPKHTCN_NOTMCH == idx) {
        /* create connection chain */
        con_sts = (pkhtcn_conmng_t *) malloc(sizeof(pkhtcn_conmng_t));
        if (NULL == con_sts) {
            return COM_NG;
        }
        memset(con_sts, 0x00, sizeof(pkhtcn_conmng_t));
        memcpy(con_sts->sip, iphdr->sip, PIA_IP_IPSIZE);
        memcpy(con_sts->dip, iphdr->dip, PIA_IP_IPSIZE);
        con_sts->sport = pia_tcp_getport(tcphdr, PIA_TCP_SPORT);
        con_sts->dport = pia_tcp_getport(tcphdr, PIA_TCP_DPORT);
        /* add connection chain */
        ret = ttrchn_add(g_pkhtcn_conchn, con_sts);
        if (TTR_NG == ret) {
            return COM_NG;
        }
    }
    
    if (NULL == con_sts) {
        con_sts = (pkhtcn_conmng_t *) ttrchn_get(g_pkhtcn_conchn, idx);
        if (NULL == con_sts) {
            return COM_NG;
        }
    }

    /* update connection status */
    return pkhtcn_updsts (con_sts, tcphdr);
}

int pkhtcn_getmngidx (pia_ipv4hdr_t *iphdr, pia_tcphdr_t *tcphdr) {
    ttrchn_t        *chain  = NULL;
    pkhtcn_conmng_t *conmng = NULL;
    int sport = 0;
    int dport = 0;
    
    /* get chain */
    chain = ttrchn_gethead(g_pkhtcn_conchn);
    if (NULL == chain) {
        return COM_NG;
    }
    chain = (ttrchn_t *) chain->next;  // head is empty
    
    /* get packet value */
    sport = pia_tcp_getport(tcphdr, PIA_TCP_SPORT);
    if (PIA_NG == sport) {
        return COM_NG;
    }
    dport = pia_tcp_getport(tcphdr, PIA_TCP_DPORT);
    if (PIA_NG == dport) {
        return COM_NG;
    }
    
    /* find matched chain */
    while (NULL != chain) {
        conmng = (pkhtcn_conmng_t *) chain->conts;
        if (NULL == conmng) {
            printf("contents is null\n");
            return COM_NG;
        }
        
        /* check sip */
        if ( (0 != memcmp(&(conmng->sip[0]), iphdr->sip, PIA_IP_IPSIZE)) &&
             (0 != memcmp(&(conmng->dip[0]), iphdr->sip, PIA_IP_IPSIZE)) ) {
            goto PKHTCN_NXTCHN_LBL;
        }
        /* check dip */
        if ( (0 != memcmp(&(conmng->sip[0]), iphdr->dip, PIA_IP_IPSIZE)) &&
             (0 != memcmp(&(conmng->dip[0]), iphdr->dip, PIA_IP_IPSIZE)) ) {
            goto PKHTCN_NXTCHN_LBL;
        }
        /* check sport */
        if ( (conmng->sport != sport) &&
             (conmng->dport != sport) ) {
            goto PKHTCN_NXTCHN_LBL;
        }
        /* check dport */
        if ( (conmng->sport != dport) && 
             (conmng->dport != dport) ) {
            goto PKHTCN_NXTCHN_LBL;
        }
        
        return (chain->idx)-1;
        
        PKHTCN_NXTCHN_LBL: chain = (ttrchn_t *) chain->next;
    }
    
    return DPKHTCN_NOTMCH;
}

int pkhtcn_updsts (pkhtcn_conmng_t *sts, pia_tcphdr_t *tcphdr) {
    if ((NULL == sts) || (NULL == tcphdr)) {
        return COM_NG;
    }

    /* check reset flag */
    if (PIA_TRUE == pia_tcp_isrst(tcphdr)) {
        if (DPKHTCN_CONSTS_ETB == sts->status) {
            /* release connection */
            sts->status = DPKHTCN_CONSTS_INIT;
            return DPKHTCN_REL;
        }
        sts->status = DPKHTCN_CONSTS_INIT;
        return DPKHTCN_NOCHG;
    }
    
    /* check connection flag */
    if (DPKHTCN_CONSTS_INIT == sts->status) {
        /* init status */
        if (PIA_TRUE == pia_tcp_issyn(tcphdr)) {
            sts->status = DPKHTCN_CONSTS_SYN;
        }
    } else if (DPKHTCN_CONSTS_SYN == sts->status) {
        /* syn status */
        if (PIA_TRUE == pia_tcp_issynack(tcphdr)) {
            sts->status = DPKHTCN_CONSTS_SYNACK;
        }
    } else if (DPKHTCN_CONSTS_SYNACK == sts->status) {
        /* syn-ack status */
        if (PIA_TRUE == pia_tcp_isack(tcphdr)) {
            /* connection established */
            sts->status = DPKHTCN_CONSTS_ETB;
            return DPKHTCN_ESTB;
        }
    } else if (DPKHTCN_CONSTS_ETB == sts->status) {
        /* connection established */
        if (PIA_TRUE == pia_tcp_isfinack(tcphdr)) {
            sts->status = DPKHTCN_CONSTS_FINACK;
        }
    } else if (DPKHTCN_CONSTS_FINACK == sts->status) {
        /* fin-ack status */
        if (PIA_TRUE == pia_tcp_isack(tcphdr)) {
           /* connection release */
           sts->status = DPKHTCN_CONSTS_INIT;
           return DPKHTCN_REL;
        }
    }
    
    return DPKHTCN_NOCHG;
}

//int pkhtcn_updinf (pia_ipv4hdr_t *iphdr) {
//    uint8_t *seek = NULL;
//    int      ret  = 0;
//    
//    /* check parameter */
//    if (NULL == iphdr) {
//        return COM_NG;
//    }
//    
//    /* get tcp header */
//    seek = pia_ip_seekpld (iphdr);
//    if (NULL == seek) {
//        return COM_NG;
//    }
//    
//    /* get index of connection info list */
//    ret = pkhtcn_getidx(iphdr, (pia_tcphdr_t *) seek);
//    if (COM_NG == ret) {
//        return COM_NG;
//    }
//
//pia_ip_dump(iphdr);
//    
//    /* check tcp 3way */
//    if (PIA_TRUE == pia_tcp_issyn((pia_tcphdr_t *) seek)) {
//        /* this pkt sends ack flag */
//        printf("syn\n");
//    } else if (PIA_TRUE == pia_tcp_issynack((pia_tcphdr_t *) seek)) {
//        /* this pkt sends syn flag */
//        printf("synack\n");
//    } else if (PIA_TRUE == pia_tcp_isack((pia_tcphdr_t *) seek)) {
//        /* this pkt sends ack flag */
//        printf("ack\n");
//    } else if (PIA_TRUE == pia_tcp_isfin((pia_tcphdr_t *) seek)) {
////        printf("fin\n");
//    } else {
///        /* this pkt sends data */
//    }
//    
//    pia_ip_dump(iphdr);
//    pia_tcp_dump_detail((pia_tcphdr_t *) seek);
//    
//    return COM_OK;
//}

//int pkhtcn_getidx (pia_ipv4hdr_t *iphdr, pia_tcphdr_t * tcphdr) {
//    int loop       = 0;
//    int hit        = COM_FALSE;
//    int inf_cnt    = ((int) sizeof(g_pkhtcn_conlst)/sizeof(pkhtcn_coninf_t));
//    uint16_t sport = pia_tcp_getport(tcphdr, PIA_TCP_SPORT);
//    uint16_t dport = pia_tcp_getport(tcphdr, PIA_TCP_DPORT);
//    pkhtcn_coninf_t emp_inf;
//    
//    /* check parameter */
//    if ((NULL == iphdr) || (NULL == tcphdr)) {
//        return COM_NG;
//    }
//    /* init val */
//    memset(&emp_inf, 0x00, sizeof(pkhtcn_coninf_t));
//    
//    for (loop=0;loop < inf_cnt;loop++) {
//        /* check table empty */
//        if (0 == memcmp(&g_pkhtcn_conlst[loop], &emp_inf, sizeof(pkhtcn_coninf_t))) {
//            break;
//        }
//        
//        /* check sip */
//        if ( (0 != memcmp(&(g_pkhtcn_conlst[loop].ip1[0]), iphdr->sip, PIA_IP_IPSIZE)) &&
//             (0 != memcmp(&(g_pkhtcn_conlst[loop].ip2[0]), iphdr->sip, PIA_IP_IPSIZE)) ) {
//            continue;
//        }
//        /* check dip */
//        if ( (0 != memcmp(&(g_pkhtcn_conlst[loop].ip1[0]), iphdr->dip, PIA_IP_IPSIZE)) &&
//             (0 != memcmp(&(g_pkhtcn_conlst[loop].ip2[0]), iphdr->dip, PIA_IP_IPSIZE)) ) {
//            continue;
//        }
//        /* check sport */
//        if ( (g_pkhtcn_conlst[loop].port1 == sport) &&
//             (g_pkhtcn_conlst[loop].port2 == sport)) {
//            continue;
//        }
//        /* check dport */
//        if ( (g_pkhtcn_conlst[loop].port1 == dport) &&
//             (g_pkhtcn_conlst[loop].port2 == dport)) {
//            continue;
//        }
//        
//        /* hit table  */
//        hit = COM_TRUE;
//        break;
//        
//    }
//    
//   if (COM_TRUE != hit) {
//        /* add conection info */
//        memcpy(&(g_pkhtcn_conlst[loop].ip1[0]), iphdr->sip, PIA_IP_IPSIZE);
//        memcpy(&(g_pkhtcn_conlst[loop].ip2[0]), iphdr->dip, PIA_IP_IPSIZE);
//        g_pkhtcn_conlst[loop].port1 = sport;
//        g_pkhtcn_conlst[loop].port2 = dport;
//    }
//    
//    return loop;
//}

//int phktcn_ismatched_port (uint16_t port) {
//    int loop       = 0;
//    int inf_cnt    = ((int) sizeof(g_pkhtcn_conlst)/sizeof(pkhtcn_coninf_t));
//    pkhtcn_coninf_t emp_inf;
//    
//    /* init val */
//    memset(&emp_inf, 0x00, sizeof(pkhtcn_coninf_t));
//    
//    for (loop=0;loop < inf_cnt;loop++) {
//        /* check table empty */
//        if (0 == memcmp(&g_pkhtcn_conlst[loop], &emp_inf, sizeof(pkhtcn_coninf_t))) {
//            break;
//        }
//        
//        /* check port */
//        if ( (g_pkhtcn_conlst[loop].port1 == sport) &&
//             (g_pkhtcn_conlst[loop].port2 == sport)) {
//            continue;
//        }
//    }
//    
//    return COM_FALSE;
//}
/* end of file */
