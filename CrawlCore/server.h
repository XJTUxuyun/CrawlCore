//
//  server.h
//  CourtCrawlCore
//  
//  Created by 王媛莉 on 2018/8/20.
//  Copyright © 2018 徐云. All rights reserved.
//

#ifndef server_h
#define server_h

#include <uv.h>
#include <assert.h>
#include "hashmap.h"
#include "list.h"
#include "assistant.h"
#include "server_tcp.h"
#include "server_udp.h"
#include "tools.h"

enum server_type
{
    TCP_SERVER = 0,
    UDP_SERVER
};

struct server
{
    uv_mutex_t mutex;  // data consistency
    uv_loop_t *loop;
    char name[256];
    enum server_type type;
    struct sockaddr_in addr;
    char ip[16];
    int port;
    uv_tcp_t tcp_handle;
    uv_udp_t udp_handle;
    uv_timer_t monitor_timer;
    void (*server_monitor_cb)(struct server *s);
    map_t udp_assistants;
    uv_mutex_t udp_assistants_mutex;
    list(struct assistant *, tcp_assistants);
    uv_mutex_t tcp_assistants_mutex;
};

//void server_init(struct server s, enum server_type type);  // initial a server

int server_init(struct server *s,
                char *name,
                enum server_type type,
                uv_loop_t *loop,
                char *ip,
                int port);



#endif /* server_h */
