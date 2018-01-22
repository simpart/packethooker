/**
 * @file conf.h
 * @brief  define for config function
 * @author simpart
 */
/*** include ***/
#include "protopia.h"

#ifndef __PKH_TCPCON_H__
#define __PKH_TCPCON_H__

/*** define ***/
#define DPKHTCN_CNF_MAXCNT  10

#define DPKHTCN_TPID_MAX       0x00  //! tcpcon hook type max connection
#define DPKHTCN_TPID_MIN       0x01  //! tcpcon hook type min connection

#define DPKHTCN_TPSTR_MAX      "max-count"
#define DPKHTCN_TPSTR_MIN      "min-count"

#define DPKHTCN_CONSTS_INIT    0x0000
#define DPKHTCN_CONSTS_SYN     0x0100
#define DPKHTCN_CONSTS_SYNACK  0x0101
#define DPKHTCN_CONSTS_ETB     0x0102
#define DPKHTCN_CONSTS_FINACK  0x0103

#define DPKHTCN_NOTMCH -100

/**
 * @brief return value for connection status
 */
#define DPKHTCN_ESTB   0x10   //! establish
#define DPKHTCN_REL    0x20   //! release
#define DPKHTCN_NOCHG  0x00   //! no change

/*** struct ***/
/**
 * @brief contents for tcp-connection config
 */
typedef struct pkhtcn_cnf {
    uint16_t type;
    uint16_t count[DPKHTCN_CNF_MAXCNT];
    uint16_t port;
} pkhtcn_cnf_t;


typedef struct pkhtcn_conmng {
    uint8_t  sip[PIA_IP_IPSIZE];
    uint8_t  dip[PIA_IP_IPSIZE];
    uint16_t sport;
    uint16_t dport;
    int      status;
} pkhtcn_conmng_t;

typedef struct pkhtcn_counter {
    uint8_t  ipaddr[PIA_IP_IPSIZE];
    uint16_t count;
    int diff;
} pkhtcn_counter_t;

/*** prototype ***/
/* init */
int pkhtcn_init (void);
/* conf */
int pkhtcn_loadconf (void);
int pkhtcn_loadconf_count (ttr_conf_t *);
int pkhtcn_gettpid (char *);
/* hook */
int pkhtcn_chkhook (uint8_t *, size_t);
int pkhtcn_islisten (pia_tcphdr_t *);
pkhtcn_counter_t * pkhtcn_getcounter(pia_ipv4hdr_t *, int);
int pkhtcn_iskick (pkhtcn_counter_t *);
int pkhtcn_getjson (pkhtcn_counter_t *, char *);
/* conn.c */
int pkhtcn_chkconn (pia_ipv4hdr_t *, pia_tcphdr_t *);
int pkhtcn_getmngidx (pia_ipv4hdr_t *, pia_tcphdr_t *);
int pkhtcn_updsts (pkhtcn_conmng_t *, pia_tcphdr_t *);
/*** macro ***/

#endif
/* end of file */
