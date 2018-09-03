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
#include "hashmap.h"
#include "list.h"
#include "uuid4.h"

#define TASK_MAX_JIFFIES 20
#define ASSISTANT_MAX_JIFFIES 100

struct task
{
    char uuid[UUID4_LEN];
    char type;  // 0 for get and 1 for post.
    void *data;  // our main purpose is a general queue, so we donot care data content, user maintain it.
    char status;  // 0 for unsuccess and other success.
    int try_times;
    int jiffies;
};

/**
 * one-on-one service for each task_id
 * be careful with its life
 */
struct assistant
{
    uv_mutex_t mutex;  // consistency
    char key[128];
    time_t frist_serve_time;  // long
    time_t last_serve_time;   // long
    long serve_times;
    long jiffies;
    map_t tasks_ready_map;
    list(struct task *, tasks_ready_list);
    map_t tasks_running_map;
    list(struct task *, task_running_list);
};

struct assistants_container
{
    uv_loop_t *loop;
    uv_mutex_t mutex;
    uv_timer_t inspector;
    map_t assistants_map;  // named assistant, use key to find the assistant.
    list(struct assistant *, assistants_list);
};

/**
 * intial struct assistant
 * @param assistant target variable
 */
int assistant_init(struct assistant *assistant, char *key);

int assistants_container_init(struct assistants_container *container,
                              uv_loop_t *loop);

struct assistant *get_assistant_instance(struct assistants_container *container, char *key);



#endif /* assistant_h */
