//
//  server.c
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/20.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "server.h"
#include <stdlib.h>
#include <json-c/json.h>



/**
 * when readed data, do something on the data
 */
void after_read_cb(uv_stream_t* handle, ssize_t nread, uv_buf_t *buf)
{
    int r;
    if (nread == 0)
    {
        // everything is ok, but read nothing
        if (buf->base != NULL)
        {
            free(buf->base);
        }
        printf("read nothing...\n");
        return;
    }
    
    if (nread < 0)
    {
        // some error occured, handle it
        if (nread == UV_EOF)  // client disconnection
        {
            printf("client disconnection...\n");
        }
        else if (nread == UV_ECONNRESET)  // client disconnection due to some error.
        {
            printf("client disconnection due to some error...\n");
        }
        else
        {
            printf("client disconnection due to some unkown error...\n");
        }
        // printf("error->%s:", uv_err_name(nread), uv_strerror(nread));
        if (buf->base != NULL)
        {
            free(buf->base);
        }
        printf("read occured error...\n");
        return;
    }
    
    // read data success
    // do actual job in threadpool
    uv_work_t *work = (uv_work_t *)malloc(sizeof(uv_work_t));  // local variable is suit
    work->data = (void **)malloc(2 * sizeof(void *) + sizeof(ssize_t));  // data is a pointer to two pointer, first is _client, second is uv_buf_t, third is nread remember free...
    if (work->data == NULL)
    {
        printf("malloc work data error...\n");
        goto error0;
    }
    
    // set data
    ((void **)work->data)[0] = handle->data;
    ((void **)work->data)[1] = buf;
    * (ssize_t *)((char *)(work->data) + 2 * sizeof(void *)) = nread;
    
    //r = uv_queue_work(handle->loop, work, client_work_cb, client_work_after_cb);
    if (r)
    {
        printf("uv_queue_work error...\n");
        goto error0;
    }
    
    return;  // everything is ok...
    
error0:
    if (buf->base)
    {
        free(buf->base);
    }
    if (work->data)
    {
        free(work->data);
    }
    return;
}

/**
 * when server listen a new connection from remote client, do something on the new connection
 * actually do these job
 * new a client and put it in the client set
 * listening the client handle
 */
void accept_connection(uv_stream_t *server, int status)
{
    int r;
    if (status != 0)
    {
        printf("connect error...\n");
        return;
    }
    
    uv_stream_t * stream = (uv_stream_t *)malloc(sizeof(uv_tcp_t));  // when close this stream, remember free it
    if (stream == NULL)
    {
        printf("malloc stream error...\n");
        goto error0;
    }
    
    r = uv_tcp_init(server->loop, (uv_tcp_t *)stream);
    if (r)
    {
        printf("initialize uv_tcp_t error...\n");
        goto error0;
    }
    
    stream->data = server;  //
    
    r = uv_accept(server, stream);
    if (r)
    {
        printf("uv_accept error...\n");
        goto error0;
    }
    
    // synchronized segment
    // be careful with operate on client set
    //uv_mutex_lock(&cs.mutex);  // lock cs & remember unlock
    
   // struct client *client_ = (struct client *)malloc(sizeof(struct client));
   // if (client_ == NULL)
   // {
   //     printf("malloc client error...\n");
   //     goto error1;
   // }
    
    int name_lens;
   // client_->stream = (uv_tcp_t *)stream;
   // r = uv_tcp_getpeername((uv_tcp_t *)stream, &client_->addr, &name_lens);
    if (r)
    {
        printf("get peername error...\n");
        goto error1;
    }
    
    //client_->port = ntohs(((struct sockaddr_in *) & client_->addr)->sin_port);
   // r = uv_ip4_name((struct sockaddr_in *) & client_->addr, client_->ip, 16);
    if (r)
    {
        printf("parse readable ip address error...\n");
        goto error1;
    }
    
    //r = uv_mutex_init(& client_->mutex);
    if (r)
    {
        printf("initialize client mutex error...\n");
        goto error1;
    }
    
   // stream->data = client_;
    
   // post(&client_->wq);  // client put in client set queue
    
   // uv_mutex_unlock(&cs.mutex);  // unlock
    
    // listenning the client
    r = uv_read_start(stream, alloc_cb, after_read_cb);
    if (r)
    {
        printf("read from client error...\n");
        goto error1;
    }
    
    return;  // everything is ok...
    
error0:  // clean context
    if (stream)
    {
        free(stream);
    }
    return;
    
error1:
  //  if (client_)
  //  {
  //      free(client_);
  //  }
    goto error0;
    return;
}


/**
 * monitor resource for remote client, if remote client dead...
 * @param handle server which will be monitored
 */
void server_monitor_cb(uv_timer_t *handle)
{
    printf("monitor...\n");
    struct server *s = (struct server *)handle->data;
    assert(NULL != s);
    if (NULL == s)
    {
        printf("monitor error...\n");
        return;
    }
    uv_mutex_lock(&s->tcp_assistants_mutex);
    list_each_elem (s->tcp_assistants, assis)
    {
        struct assistant *tcp_assis = *assis;
        printf("here->%s\n", tcp_assis->remote_ip);
        tcp_assis->time_slice --;
        printf("time slice->%d\n", tcp_assis->time_slice);
        if (tcp_assis->time_slice <= 0)  // assistant is dead...
        {
            uv_read_stop((uv_stream_t *)&tcp_assis->tcp_handle);
            list_elem_remove (assis);
        }
        
    }
    uv_mutex_unlock(&s->tcp_assistants_mutex);
}








/**
 * initialize a server
 * @param s the server which want to be initial, user maintain memeory
 * @param name the server name
 * @param type server type, UDP or TCP
 * @param loop server loop
 * @param ip binding NIC
 * @param port binding port
 * return 0 if success
 */
int server_init(struct server *s,
                char *name,
                enum server_type type,
                uv_loop_t *loop,
                char *ip,
                int port)
{
    int r;
    memset(s, 0, sizeof(struct server));  // initial memeory
    strcpy(s->name, name);
    s->type = type;
    assert(loop != NULL);
    if (loop == NULL)
    {
        printf("server desire loop is null...\n");
        return -1;
    }
    s->loop = loop;
    s->port = port;
    strcpy(s->ip, ip);
    r = uv_ip4_addr(s->ip, s->port, &s->addr);
    assert(0 == r);
    if (r)
    {
        printf("ip->%s or port->%d is invalid...\n", s->ip, s->port);
        return -1;
    }
    switch (s->type) {
        case TCP_SERVER:
            r = server_init_tcp(s);
            break;
        case UDP_SERVER:
            r = server_init_udp(s);
            break;
        default:
            printf("server type invalid...\n");
            return -1;
            break;
    }
    assert(0 == r);
    if (r)
    {
        return -1;
    }
    
    r = uv_timer_init(s->loop, &s->monitor_timer);
    assert(0 == r);
    if (r)
    {
        return -1;
    }
    s->monitor_timer.data = s;  // pass server itself to timer callback
    r = uv_timer_start(& s->monitor_timer, server_monitor_cb, 3000, 2000);
    assert(0 == r);
    if (r)
    {
        return -1;
    }
    
    return 0;
    
error0:
    ;
}








