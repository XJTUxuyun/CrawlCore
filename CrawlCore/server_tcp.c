//
//  server_tcp.c
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/28.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "server_tcp.h"

/**
 * do some tcp type server initial
 * default backlog is 128
 * @param s server
 */
int server_init_tcp(struct server *s)
{
    int r;
    s->tcp_handle.data = s;  // pass server to tcp handle
    memset(& s->tcp_assistants, 0, sizeof(s->tcp_assistants));
    assert(0 == list_length(s->tcp_assistants));
    if (0 != list_length(s->tcp_assistants))
    {
        printf("create tcp assistants list error...\n");
        return -1;
    }
    r = uv_mutex_init(& s->tcp_assistants_mutex);
    assert(0 == r);
    if (r)
    {
        printf("intial tcp assistants mutex error...\n");
        return r;
    }
    r = uv_tcp_init(s->loop, &s->tcp_handle);
    assert(0 == r);
    if(r)
    {
        printf("initial tcp_handle error...\n");
        uv_mutex_destroy(&s->tcp_assistants_mutex);
        return r;
    }
    r = uv_tcp_bind(&s->tcp_handle, (struct sockaddr*)&s->addr, 0);
    assert(0 == r);
    if (r)
    {
        printf("bind tcp_handle error...\n");
        uv_mutex_destroy(&s->tcp_assistants_mutex);
        return r;
    }
    r = uv_listen((uv_stream_t *)&s->tcp_handle, 128, server_tcp_accept_connection);
    assert( 0 == r);
    if (r)
    {
        printf("listen tcp handle error...\n");
        uv_mutex_destroy(&s->tcp_assistants_mutex);
        return r;
    }
    return 0;  // everything is ok.
}

/**
 * callback when a new tcp connection from remote client
 * @param tcp triggered tcp handle
 * @param status status pass from uv
 */
void server_tcp_accept_connection(uv_stream_t *tcp,
                                  int status)
{
    int r;
    assert(status == 0);
    if (status)
    {
        printf("tcp connection error...\n");
        return;
    }
    
    // ready for accept a remote connection
    uv_tcp_t tcp_incoming;
    r = uv_tcp_init(tcp->loop, (uv_tcp_t *)(&tcp_incoming));
    assert(r == 0);
    if (r)
    {
        printf("intial remote tcp client handle error...\n");
        return;
    }
    
    r = uv_accept(tcp, (uv_stream_t *)(&tcp_incoming));
    assert(0 == r);
    if (r)
    {
        printf("accept a remote client error...\n");
        return;
    }
    
    struct assistant *assis = (struct assistant *)malloc(sizeof(struct assistant));
    assert(NULL != assis);
    if (NULL == assis)
    {
        printf("malloc assistant error...\n");
        return;
    }
    r = assistant_init(assis);
    assert(0 == r);
    if (r)
    {
        printf("inital struct assistant error...\n");
        if (assis != NULL)
        {
            goto leak;
        }
    }
    memcpy(&assis->tcp_handle, &tcp_incoming, sizeof(uv_tcp_t));
    assis->time_slice = 3;  //
    int name_lens;
    r = uv_tcp_getpeername(&tcp_incoming, &assis->addr, &name_lens);
    assert(0 == r);
    if (r)
    {
        printf("tcp get peername error...\n");
        goto leak;
    }
    assis->port = ntohs(((struct sockaddr_in *) & assis->addr)->sin_port);
    r = uv_ip4_name((struct sockaddr_in *) & assis->addr, assis->remote_ip, 16);
    if (r)
    {
        printf("parse readable ip address error...\n");
        goto leak;
    }
    struct server *s = (struct server *)(tcp->data);
    
    uv_mutex_lock(& s->tcp_assistants_mutex);
    list_push(s->tcp_assistants, assis);
    uv_mutex_unlock(& s->tcp_assistants_mutex);
    
    tcp_incoming.data = assis;
    r = uv_read_start((uv_stream_t *)(&tcp_incoming), alloc_cb, server_tcp_read_cb);
    assert(0 == r);
    if (r)
    {
        printf("server start read remote client error...\n");
        goto leak;
    }
    return;  // everything is ok.
leak:
    if (assis)
    {
        free(assis);
    }
    return;
}

/**
 * true action on the tcp recieve data.
 * @param req all data encapsulate
 */
void server_tcp_work_cb(uv_work_t *req)
{
    printf("heheh...\n");
}

/**
 * action after server_tcp_work_cb
 * @param req remember free data to avoid leak
 * @param status server_tcp_work_cb excute status
 */
void server_tcp_work_after_cb(uv_work_t *req,
                              int status)
{
    
}

/**
 * server tcp read callback
 * @param handle connected tcp handle
 * @param nread num of data readed
 * @param buf readed data stored
 */
void server_tcp_read_cb(uv_stream_t* handle,
                        ssize_t nread,
                        const uv_buf_t *buf)
{
    int r;
    
    uv_work_t req;
    // pass data
    
    struct assistant * assis = (struct assistant *)handle->data;
    uv_mutex_lock(&assis->mutex);
    assis->time_slice = 3;
    uv_mutex_unlock(&assis->mutex);
    printf("remote port:%d,...\n", ((struct assistant *)handle->data)->port);
    //printf("const char *restrict, %d   %d...\n", nread, *(int *)(handle->data));
    r = uv_queue_work(handle->loop, &req, server_tcp_work_cb, server_tcp_work_after_cb);
    assert(0 == r);
    if (r)
    {
        printf("post tcp req error...\n");
        return;
    }

}

/**
 * close a dead tcp connection, collect resource
 * @param handle the tcp handle which need close
 */
void assistant_tcp_close_cb(uv_handle_t *handle)
{
    uv_tcp_t * tcp = (uv_tcp_t *)handle;
    printf("嘎嘎->%s\n", ((struct assistant *)tcp->data)->remote_ip);
}


