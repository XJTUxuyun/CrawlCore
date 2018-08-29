//
//  server_tcp.h
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/28.
//  Copyright © 2018 徐云. All rights reserved.
//

#ifndef server_tcp_h
#define server_tcp_h

#include <stdio.h>
#include "server.h"

/**
 * server tcp read callback
 * @param handle connected tcp handle
 * @param nread num of data readed
 * @param buf readed data stored
 */
void server_tcp_read_cb(uv_stream_t* handle,
                        ssize_t nread,
                        const uv_buf_t *buf);

/**
 * action after server_tcp_work_cb
 * @param req remember free data to avoid leak
 * @param status server_tcp_work_cb excute status
 */
void server_tcp_work_after_cb(uv_work_t *req,
                              int status);

/**
 * true action on the tcp recieve data.
 * @param req all data encapsulate
 */
void server_tcp_work_cb(uv_work_t *req);

/**
 * callback when a new tcp connection from remote client
 * @param tcp triggered tcp handle
 * @param status status pass from uv
 */
void server_tcp_accept_connection(uv_stream_t *tcp,
                                  int status);

/**
 * do some tcp type server initial
 * default backlog is 128
 * @param s server
 */
int server_init_tcp(struct server *s);

/**
 * close a dead tcp connection, collect resource
 * @param handle the tcp handle which need close
 */
void assistant_tcp_close_cb(uv_handle_t *handle);

#endif /* server_tcp_h */
