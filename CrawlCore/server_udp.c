//
//  server_udp.c
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/28.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "server_udp.h"

/**
 * true action on the udp recieve data.
 * @param req all data encapsulate
 */
void server_udp_work_cb(uv_work_t *req)
{
    /*
    int r;
    struct sockaddr addr = *(struct sockaddr *)((char *)(req->data));
    uv_buf_t buf = *(uv_buf_t *)((char *)(req->data) + sizeof(struct sockaddr));
    ssize_t nread = *(ssize_t *)((char *)(req->data) + sizeof(struct sockaddr) + sizeof(uv_buf_t));
    char sender[16];
    r = uv_ip4_name(((struct sockaddr_in*)(&addr)), sender, 16);
    assert(0 == r);
    if (r)
    {
        return;
    }
    int port = ntohs(((struct sockaddr_in*) &addr)->sin_port);
    printf("remote ip->%s port->%d",sender, port);
    */
    printf("fuck u...\n");
}

/**
 * action after server_udp_work_cb
 * @param req remember free data to avoid leak
 * @param status server_udp_work_cb excute status
 */
void server_udp_work_after_cb(uv_work_t *req,
                              int status)
{
    uv_buf_t buf = *(uv_buf_t *)((char *)(req->data));
    if (buf.base != NULL)
    {
        free(buf.base);
    }
    if (req->data != NULL)
    {
        free(req->data);
    }
    if (req)
    {
        free(req);
    }
}

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
                     unsigned flags)
{
    int r;
    // assert(NULL == addr);
    if (NULL == addr)  // addr null represent read data error, free memory to avoid leak.
    {
        printf("ssssssssss");
        if (buf->base)
        {
            free(buf->base);
        }
        return;
    }
    
    // assert(UV_UDP_PARTIAL == flags);
    if (UV_UDP_PARTIAL == flags)
    {
        printf("read partial data due to less memory...\n");
        if (buf->base)
        {
            free(buf->base);
        }
        return;
    }
    
    // assert(nread >= 0);
    if (nread < 0)
    {
        printf("read udp data error...\n");
        if (buf->base)
        {
            free(buf->base);
        }
        return;
    }
    if (0 == nread)
    {
        printf("read udp data empty...\n");
        if (buf->base)
        {
            free(buf->base);
        }
        return;
    }
    // true job will be done with threadpool.
    uv_work_t *req = malloc(sizeof(uv_work_t));
    if (NULL == req)
    {
        return;
    }
    req->data = malloc(sizeof(struct sockaddr) + sizeof(uv_buf_t) + sizeof(ssize_t));  // remember free to avoid leak.
    assert(req->data != NULL);
    if (req->data == NULL)
    {
        printf("malloc udp req memory error...\n");
        if (buf->base)
        {
            free(buf->base);
        }
        return;
    }
    *(struct sockaddr *)((char *)(req->data)) = *addr;
    *(uv_buf_t *)((char *)(req->data) + sizeof(struct sockaddr)) = *buf;
    *(ssize_t *)((char *)(req->data) + sizeof(struct sockaddr) + sizeof(uv_buf_t)) = nread;
    r = uv_queue_work(udp->loop, req, server_udp_work_cb, server_udp_work_after_cb);
    assert(0 == r);
    if (r)
    {
        printf("post udp req error...\n");
        // do some clean work
        if (buf->base)
        {
            free(buf->base);
        }
        if (req->data)
        {
            free(req->data);
        }
        return;
    }
}

/**
 * do some udp type server initial
 * @param s server
 */
int server_init_udp(struct server *s)
{
    int r;
    r = uv_udp_init(s->loop, &s->udp_handle);
    assert(0 == r);
    if (r)
    {
        printf("initial udp_handle error...\n");
        return r;
    }
    r = uv_udp_bind(& s->udp_handle, (struct sockaddr *) & s->addr, UV_UDP_REUSEADDR);
    assert(0 == r);
    if (r)
    {
        printf("bind udp_handle error...\n");
        return r;
    }
    
    s->udp_handle.data = s;
    
    r = uv_udp_recv_start(& s->udp_handle, alloc_cb, server_udp_read);
    assert(0 == r);
    if (r)
    {
        printf("udp recv start error...\n");
        return r;
    }
    return 0;
}
