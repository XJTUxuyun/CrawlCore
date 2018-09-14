//
//  server.c
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/20.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "server.h"
#include <stdlib.h>

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
    /*
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
    uv_mutex_unlock(&s->tcp_assistants_mutex);*/
}

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
            r = -1;
            break;
    }
    assert(0 == r);
    if (r)
    {
        return r;
    }
    
    r = uv_timer_init(s->loop, &s->monitor_timer);
    assert(0 == r);
    if (r)
    {
        return -1;
    }
    s->monitor_timer.data = s;  // pass server itself to timer callback
    //r = uv_timer_start(& s->monitor_timer, server_monitor_cb, 3000, 2000);
    assert(0 == r);
    if (r)
    {
        return -1;
    }
    
    return 0;
    
}


int server_destory(struct server *s)
{
    return 0;
}








