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

#define TASK_MAX_TICK 2
#define ASSISTANT_MAX_TICK 3

enum TASK_STATUS
{
    TASK_DONE=0,  // task is done
    TASK_RUNING,  // task is running
    TASK_READY    // task is ready for running
};

/**
 * one crawl task
 */
struct task
{
    unsigned char uuid[UUID4_LEN];  // unique id;
    int mid;        // distinguish a task.
    int status;     // 0 for unsuccess and other success.
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
    map_t tasks_running_map;
    list(struct task *, task_ready_list);
    list(struct task *, task_done_list);
    struct db_backend *db_backend;
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
    struct db_backend *db_backend;
};

/**
 * intial struct task.
 * @param task task instance who want to be initial.
 * @param uuid designed uuid to initial task instance, if NULL, create one.
 */
int task_init(struct task *task, char *uuid);

/**
 * destory struct task.
 * @param task task instance who want to be destory.
 */
int task_destory(struct task *task);

/**
 * intial struct assistant
 * @param assistant target variable
 * @param db_backend database backend
 */
int assistant_init(struct assistant *assistant, char *key, struct db_backend *db_backend);

/**
 * post task into assistant
 * @param assistant object assistant
 * @param task object task
 */
int assistant_post(struct assistant *assistant, struct task *task);

/*
 * ack one task
 * @param assistant object assistant
 * @param key task key
 * @param status ack status
 */
int assistant_ack(struct assistant *assistant, char *key, int status);

/*
 * get one task
 * @param assistant object assistant
 * @param mid request mid
 * @return task
 */
struct task *assistant_get(struct assistant *assistant, int mid);

/**
 * destory assistant
 * @param assistant assitant instance
 */
int assistant_destory(struct assistant *assistant);

/**
 * initial assistants contanier
 * @param container container instance
 * @param loop container ref loop
 * @param db_backend database backend
 */
int assistants_container_init(struct assistants_container *container,
                              uv_loop_t *loop, struct db_backend *db_backend);

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
