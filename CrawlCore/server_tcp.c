//
//  server_tcp.c
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/28.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "server_tcp.h"

int server_init_tcp(struct server *s)
{
    int r;
    r = uv_tcp_init(s->loop, &s->tcp_handle);
    assert(0 == r);
    if(r)
    {
        printf("initial tcp_handle error...\n");
        return r;
    }
    s->tcp_handle.data = s;  // pass server to tcp handle,
    r = uv_tcp_bind(&s->tcp_handle, (struct sockaddr*)&s->addr, 0);
    assert(0 == r);
    if (r)
    {
        printf("bind tcp_handle error...\n");
        return r;
    }
    r = uv_listen((uv_stream_t *)&s->tcp_handle, 128, server_tcp_accept_connection);
    assert( 0 == r);
    if (r)
    {
        printf("listen tcp handle error...\n");
        return r;
    }
    return 0;  // everything is ok.
}

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
    uv_tcp_t *tcp_incoming = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    r = uv_tcp_init(tcp->loop, (uv_tcp_t *)(tcp_incoming));
    assert(r == 0);
    if (r)
    {
        printf("intial remote tcp client handle error...\n");
        free(tcp_incoming);
        return;
    }
    r = uv_accept(tcp, (uv_stream_t *)(tcp_incoming));
    assert(0 == r);
    if (r)
    {
        printf("accept a remote client error...\n");
        free(tcp_incoming);
        return;
    }
    tcp_incoming->data = malloc(sizeof(void **) * 2);
    assert(NULL != tcp_incoming->data);
    if (NULL == tcp_incoming->data)
    {
        printf("malloc tcp_incoming data error...\n");
        free(tcp_incoming);
        return;
    }
    ((void **)(tcp_incoming->data))[0] = tcp->data;
    ((void **)(tcp_incoming->data))[1] = tcp_incoming;  // trace ptr to avoid leak.
    //uv_tcp_keepalive(tcp_incoming, 1, 3);
    r = uv_read_start((uv_stream_t *)tcp_incoming, alloc_cb, server_tcp_read_cb);
    assert(0 == r);
    if (r)
    {
        printf("server start read remote client error...\n");
        uv_close((uv_handle_t *)tcp_incoming, NULL);
        free(tcp_incoming->data);
        free(tcp_incoming);
    }
    return;  // everything is ok.
}

void server_tcp_write_cb(uv_write_t *req, int status)
{
    uv_write_t *write = *(void **)req->data;
    uv_buf_t buf = *(uv_buf_t *)((char *)req->data + sizeof(void **));
    if (buf.base)
    {
        free(buf.base);
    }
    free(write->data);
    free(write);
}

void server_tcp_work_cb(uv_work_t *req)
{
    struct server *server = (struct server *)((void **)req->data)[0];
    uv_tcp_t *tcp_incoming = (uv_tcp_t *)((void **)req->data)[1];
    uv_buf_t buf = *((uv_buf_t *)((char *)req->data + sizeof(void **) *2));
    ssize_t nread = *((ssize_t *)((char *)req->data + sizeof(void **) * 2 + sizeof(uv_buf_t)));
    struct sockaddr addr = *((struct sockaddr *)((char *)req->data + sizeof(void **) * 2 + sizeof(uv_buf_t) + sizeof(ssize_t)));
    int port = ntohs(((struct sockaddr_in *) & addr)->sin_port);
    
    // parse req data
    // true job
    uv_buf_t buf1 = process_request(&buf, nread);
    uv_write_t *write = malloc(sizeof(uv_write_t));
    if (NULL == write)
    {
        return;
    }
    write->data = malloc(sizeof(void **) + sizeof(uv_buf_t));
    if (NULL == write->data)
    {
        free(write);
        return;
    }
    *((void **)write->data) = write;
    
    *(uv_buf_t *)((char *)write->data + sizeof(void **)) = buf1;
    uv_write(write, (uv_stream_t *)tcp_incoming, &buf1, 1, server_tcp_write_cb);
    printf("fuck port------>%d\n", port);
    printf("work_cb ->%p\n", tcp_incoming);
    printf("%lu, %s %s\n", nread, buf.base, server->name);
}

void server_tcp_work_after_cb(uv_work_t *req,
                              int status)
{
    if (NULL == req->data)
        return;
    uv_buf_t buf = *((uv_buf_t *)((char *)req->data + sizeof(void **) *2));
    if (buf.base)
        free(buf.base);
    free(req->data);
    if (req)
    {
        free(req);
    }
}

void tcp_connection_close_cb(uv_handle_t *tcp_connection)
{
    if (tcp_connection->data)
    {
        uv_tcp_t * t = ((void **)(tcp_connection->data))[1];
        if (t)
        {
            free(t);
        }
        free(tcp_connection->data);
    }
}

void server_tcp_read_cb(uv_stream_t* handle,
                        ssize_t nread,
                        const uv_buf_t *buf)
{
    int r;
    if (nread < 0)  // tcp read error, maybe close
    {
        uv_close(((void **)(handle->data))[1], tcp_connection_close_cb);
        return;
    }
    
    if (nread == 0) // read nothing or shutdown by peer client
    {
        uv_close(((void **)(handle->data))[1], tcp_connection_close_cb);
        return;
    }
    
    int name_lens;
    struct sockaddr addr;
    r = uv_tcp_getpeername((uv_tcp_t *)handle, &addr, &name_lens);
    assert(0 == r);
    if (r)
    {
        printf("get peername error...\n");
        return;
    }
    
    uv_work_t *req = malloc(sizeof(uv_work_t));
    if (NULL == req)
    {
        return;
    }
    req->data = malloc(sizeof(void **) * 2 + sizeof(uv_buf_t) + sizeof(ssize_t) + sizeof(struct sockaddr));
    assert(NULL != req->data);
    if (NULL == req->data)
    {
        printf("malloc tcp work data error...\n");
        return;
    }
    memcpy(req->data, handle->data, sizeof(void **) * 2);
    *((uv_buf_t *)((char *)req->data + sizeof(void **) * 2)) = *buf;
    *((ssize_t *)((char *)req->data + sizeof(void **) * 2 + sizeof(uv_buf_t))) = nread;
    *((struct sockaddr *)((char *)req->data + sizeof(void **) * 2 + sizeof(uv_buf_t) + sizeof(ssize_t))) = addr;
    r = uv_queue_work(handle->loop, req, server_tcp_work_cb, server_tcp_work_after_cb);
    assert(0 == r);
    if (r)
    {
        printf("queue tcp work error...\n");
        free(req->data);
    }
}



