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
#include "cJSON.h"
#include "process_request.h"

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
    list(uv_tcp_t *, tcp_connections_list);
};

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
                int port);

/**
 * destory a server
 * @param s the server which want to be destory
 */
int server_destory(struct server *s);

/**
 * monitor resource for remote client, if remote client dead...
 * @param handle server which will be monitored
 */
void server_monitor_cb(uv_timer_t *handle);




#endif /* server_h */
