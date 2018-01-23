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
/* end of file */
