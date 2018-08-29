//
//  assistant.h
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/27.
//  Copyright © 2018 徐云. All rights reserved.
//

#ifndef assistant_h
#define assistant_h

#include <stdio.h>
#include <assert.h>
#include <uv.h>
#include <time.h>

struct menu
{
    
};

/**
 * one-on-one service for each remote client
 * be careful with its life
 */
struct assistant
{
    uv_mutex_t mutex;  // consistency
    struct sockaddr addr;  // address of the remote client
    char remote_ip[16];
    int port;
    uv_tcp_t tcp_handle;  // just for tcp close use
    time_t frist_serve_time;
    time_t last_serve_time;
    long serve_times;
    int time_slice;  // monitor will descrease this and when less than zero, this assistant is dead
};

/**
 * intial struct assistant
 * @param assis target variable
 */
int assistant_init(struct assistant *assis);

#endif /* assistant_h */
