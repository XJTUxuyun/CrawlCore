//
//  server_udp.h
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/28.
//  Copyright © 2018 徐云. All rights reserved.
//

#ifndef server_udp_h
#define server_udp_h

#include <stdio.h>
#include "server.h"

/**
 * true action on the udp recieve data.
 * @param req all data encapsulate
 */
void server_udp_work_cb(uv_work_t *req);

/**
 * action after server_udp_work_cb
 * @param req remember free data to avoid leak
 * @param status server_udp_work_cb excute status
 */
void server_udp_work_after_cb(uv_work_t *req,
                              int status);

/**
 * server udp read callback
 * @param udp triggered udp handle
 * @param nread num of data readed
 * @param buf readed data stored
 * @param addr peer address
 * @param flags data read flags, decide next behavor
 */
void server_udp_read(uv_udp_t *udp,
                     ssize_t nread,
                     const uv_buf_t *buf,
                     const struct sockaddr *addr,
                     unsigned flags);

/**
 * do some udp type server initial
 * @param s server
 */
int server_init_udp(struct server *s);



#endif /* server_udp_h */
