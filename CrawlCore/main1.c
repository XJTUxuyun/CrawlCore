
#include <stdio.h>
#include <uv.h>
#include <stdlib.h>
#include <assert.h>
#include "tools.h"

void tcp_read_cb(uv_stream_t* handle,
                 ssize_t nread,
                 const uv_buf_t *buf)
{
    printf("nread->%d\n", nread);
}

void connection_cb(uv_stream_t *tcp,
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
    
    r = uv_accept(tcp, (uv_stream_t *)(&tcp_incoming));
    //assert(0 == r);
    if (r)
    {
        printf("accept a remote client error...\n");
        return;
    }
    
    tcp_incoming.data = malloc(sizeof(void **) * 2);
    //assert(NULL != tcp_incoming->data);
    if (NULL == tcp_incoming.data)
    {
        printf("malloc tcp_incoming data error...\n");
        //free(tcp_incoming);
        return;
    }
    //((void **)(tcp_incoming->data))[0] = tcp->data;
    //((void **)(tcp_incoming->data))[1] = tcp_incoming;
    //uv_tcp_keepalive(tcp_incoming, 1, 3);
    r = uv_read_start((uv_stream_t *)&tcp_incoming, alloc_cb, tcp_read_cb);
    assert(0 == r);
    if (r)
    {
        printf("server start read remote client error...\n");
        uv_close((uv_handle_t *) &tcp_incoming, NULL);
    }
    return;  // everything is ok.
}

int main111(int argc, char **argv)
{
    int r;
    uv_loop_t *loop = uv_default_loop();
    uv_tcp_t server1;
    struct sockaddr_in addr;
    r = uv_ip4_addr("127.0.0.1", 9001, &addr);
    assert(0 == r);
    r = uv_tcp_init(loop, &server1);
    assert(0 == r);
    r = uv_tcp_bind(&server1, (const struct sockaddr*) &addr, 0);
    assert(0 == r);
    r = uv_listen((uv_stream_t *)&server1, 128, connection_cb);
    printf("1111%s\n", uv_err_name(r));
    assert(0 == r);
    r = uv_run(loop, UV_RUN_DEFAULT);
    assert(0 == r);
    return 0;
}


/*
void test_worker(uv_work_t *req)
{
    char sender[17] = { 0 };
    printf("呵呵->%d...\n", * (ssize_t *)(req->data));
    uv_buf_t * buf = (uv_buf_t *)((char *)req->data + sizeof(ssize_t));
    printf("嘿嘿->%s...\n", buf->base);
    struct sockaddr *addr = (struct sockaddr *)((char *)req->data + sizeof(ssize_t) + sizeof(uv_buf_t));
    uv_ip4_name((struct sockaddr_in*) addr, sender, 16);
    int port = ntohs(((struct sockaddr_in*) addr)->sin_port);
    printf("data from addr->%s, port->%d\n", sender, port);
    printf("recieve data-> %s\n", buf->base);
}

void test_worker_after(uv_work_t *req, int status)
{
    assert(status == 0);
}

void alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    //suggested_size = 3;
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

void server_read_cb(uv_udp_t *handle, ssize_t nread, \
                    const uv_buf_t *buf, const struct sockaddr *addr, \
                    unsigned flags)
{
    if(!nread)  // avoid memory leak
    {
        if (buf->base)
            free(buf->base);
        return;
    }
    // user need maintained memory
    uv_work_t *work = (uv_work_t *)malloc(sizeof(uv_work_t));
    work->data = (char *)malloc(sizeof(ssize_t) + sizeof(uv_buf_t) + sizeof(struct sockaddr));
    memcpy('l'->data, &nread, sizeof(ssize_t));
    memcpy(work->data + sizeof(ssize_t), buf, sizeof(uv_buf_t));
    memcpy(work->data + sizeof(ssize_t) + sizeof(uv_buf_t), addr, sizeof(struct sockaddr));
    uv_queue_work(handle->loop, work, test_worker, test_worker_after);
    char sender[17] = { 0 };
    if (flags == UV_UDP_PARTIAL)
        printf("flag->%d\n", flags);
    printf("nread->%d\n", nread);
    
    // uv_ip4_name((struct sockaddr_in*) addr, sender, 16);
    // int port = ntohs(((struct sockaddr_in*) addr)->sin_port);
    // printf("data from addr->%s, port->%d\n", sender, port);
    // printf("recieve data-> %s\n", buf->base);
    if (buf->base)
    {
        free(buf->base);
    }
}

void idle_cb(uv_idle_t *handle)
{
    printf("hello world...\n");
}

int main11(int argc, char ** argv)
{
    int r;
    uv_loop_t * loop = NULL;
    uv_udp_t server;
    loop = uv_default_loop();
    struct sockaddr_in addr;
    r = uv_ip4_addr("127.0.0.1", 9000, &addr);
    r = uv_udp_init(loop, &server);
    r = uv_udp_bind(&server, (const struct sockaddr*) &addr, UV_UDP_REUSEADDR);
    if(r)
    {
        printf("bind error...\n");
    }
    r = uv_udp_recv_start(&server, alloc_cb, server_read_cb);
    uv_idle_t idle;
    uv_idle_init(loop, &idle);
    // uv_idle_start(&idle, idle_cb);
    r = uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}*/



