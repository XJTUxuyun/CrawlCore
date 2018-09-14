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
#include "db_backend.h"

#define TASK_MAX_TICK 20
#define ASSISTANT_MAX_TICK 2

/**
 * one crawl task
 */
struct task
{
    char uuid[UUID4_LEN];  // unique id;
    int mid;        // distinguish a task.
    char status;    // 0 for unsuccess and other success.
    time_t ctime;   // create time.
    time_t mtime;   // modify time.
    int retry;      // retry times.
    void *data;     // our main purpose is a general queue, so we donot care data content, user maintain it.
    int len;        // data length.
    int tick;       // task life time.
};

/**
 * one-on-one service for each task_id
 * be careful with its life
 */
struct assistant
{
    uv_mutex_t mutex;         // consistency
    char key[128];
    time_t frist_serve_time;  // long
    time_t last_serve_time;   // long
    long serve_times;
    long tick;
    map_t tasks_ready_map;
    list(struct task *, task_ready_list);
    map_t tasks_running_map;
    list(struct task *, task_running_list);
    list(struct task *, task_done_list);
};

/**
 * assistants container
 */
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

/**
 * destory assistant
 * @param assistant assitant instance
 */
int assistant_destory(struct assistant *assistant);

/**
 * initial assistants contanier
 * @param container container instance
 * @param loop container ref loop
 */
int assistants_container_init(struct assistants_container *container,
                              uv_loop_t *loop);

/**
 * destory assistants container
 * @param container container instance
 */
int assistants_container_destory(struct assistants_container *container);

/**
 * get a assistant instance from assistants container
 * @param container container
 * @param key assistant key
 */
struct assistant *get_assistant_instance(struct assistants_container *container,
                                         char *key);



#endif /* assistant_h */
