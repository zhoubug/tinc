/*
    connection.h -- header for connection.c
    Copyright (C) 2000,2001 Guus Sliepen <guus@sliepen.warande.net>,
                  2000,2001 Ivo Timmermans <itimmermans@bigfoot.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    $Id: connection.h,v 1.1.2.12 2001/09/24 14:11:59 guus Exp $
*/

#ifndef __TINC_CONNECTION_H__
#define __TINC_CONNECTION_H__

#include <avl_tree.h>
#include <list.h>

#include "config.h"

#ifdef HAVE_OPENSSL_EVP_H
# include <openssl/evp.h>
#else
# include <evp.h>
#endif

#ifdef HAVE_OPENSSL_RSA_H
# include <openssl/rsa.h>
#else
# include <rsa.h>
#endif

#include "net.h"
#include "conf.h"

typedef struct status_bits_t {
  int pinged:1;                    /* sent ping */
  int meta:1;                      /* meta connection exists */
  int active:1;                    /* 1 if active.. */
  int outgoing:1;                  /* I myself asked for this conn */
  int termreq:1;                   /* the termination of this connection was requested */
  int remove:1;                    /* Set to 1 if you want this connection removed */
  int timeout:1;                   /* 1 if gotten timeout */
  int validkey:1;                  /* 1 if we currently have a valid key for him */
  int waitingforkey:1;             /* 1 if we already sent out a request */
  int dataopen:1;                  /* 1 if we have a valid UDP connection open */
  int encryptout:1;		   /* 1 if we can encrypt outgoing traffic */
  int decryptin:1;                 /* 1 if we have to decrypt incoming traffic */
  int unused:18;
} status_bits_t;

#define OPTION_INDIRECT		0x0001
#define OPTION_TCPONLY		0x0002

typedef struct connection_t {
  char *name;                      /* name of this connection */
  ipv4_t address;                  /* his real (internet) ip */
  short unsigned int meta_port;    /* port number of meta connection */
  char *hostname;                  /* the hostname of its real ip */
  int protocol_version;            /* used protocol */
  short unsigned int port;         /* port number for UDP traffic */
  long int options;                /* options turned on for this connection */

  int socket;                      /* our udp vpn socket */
  int meta_socket;                 /* our tcp meta socket */
  status_bits_t status;            /* status info */

  RSA *rsa_key;                    /* the public/private key */
  EVP_CIPHER_CTX *cipher_inctx;    /* Context of encrypted meta data that will come from him to us */
  EVP_CIPHER_CTX *cipher_outctx;   /* Context of encrypted meta data that will be sent from us to him */
  char *cipher_inkey;              /* His symmetric meta key */
  char *cipher_outkey;             /* Our symmetric meta key */
  EVP_CIPHER *cipher_pkttype;      /* Cipher type for encrypted vpn packets */ 
  char *cipher_pktkey;             /* Cipher key and iv */
  int cipher_pktkeylength;         /* Cipher key and iv length*/

  char *buffer;                    /* metadata input buffer */
  int buflen;                      /* bytes read into buffer */
  int tcplen;                      /* length of incoming TCPpacket */
  int allow_request;               /* defined if there's only one request possible */

  time_t last_ping_time;           /* last time we saw some activity from the other end */  

  list_t *queue;                   /* Queue for packets awaiting to be encrypted */

  char *mychallenge;               /* challenge we received from him */
  char *hischallenge;              /* challenge we sent to him */

  struct connection_t *nexthop;    /* nearest meta-hop in this direction */
  struct connection_t *lastbutonehop; /* meta-hop closest to him */
  
  avl_tree_t *subnet_tree;         /* Pointer to a tree of subnets belonging to this connection */

  struct config_t *config;         /* Pointer to configuration tree belonging to this host */
} connection_t;

extern avl_tree_t *connection_tree;
extern avl_tree_t *active_tree;
extern connection_t *myself;

extern void init_connections(void);
extern connection_t *new_connection(void);
extern void free_connection(connection_t *);
extern void id_add(connection_t *);
extern void active_add(connection_t *);
extern void active_del(connection_t *);
extern void connection_add(connection_t *);
extern void connection_del(connection_t *);
extern void prune_add(connection_t *);
extern void prune_flush(void);
extern connection_t *lookup_id(char *);
extern connection_t *lookup_active(ipv4_t, short unsigned int);
extern void dump_connection_list(void);
extern int read_host_config(connection_t *);
extern void destroy_trees(void);

#endif /* __TINC_CONNECTION_H__ */
