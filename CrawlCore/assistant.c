//
//  assistant.c
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/27.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "assistant.h"

int task_init(struct task *task, char *uuid)
{
    memset(task, 0, sizeof(struct task));
    if (NULL == uuid)
    {
        uuid4_generate(task->uuid);
    }
    else
    {
        memcpy(task->uuid, uuid, strlen(uuid));
    }
    time(&task->ctime);
    time(&task->mtime);
    task->status = 0;
    task->retry = 0;
    task->tick = 0;
    task->len = 0;
    return 0;
}

int task_destory(struct task *task)
{
    if (task->data)
    {
        free(task->data);
    }
    if (task)
    {
        free(task);
    }
    return 0;
}

int assistant_init(struct assistant *assistant, char *key, struct db_backend *db_backend)
{
    int r;
    memset(assistant, 0, sizeof(struct assistant));
    r = uv_mutex_init(&assistant->mutex);
    assert(0 == r);
    if (r)
    {
        uv_mutex_destroy(& assistant->mutex);
        return r;
    }
    strcpy(assistant->key, key);
    assistant->tick = 0;
    assistant->tasks_ready_map = hashmap_new();
    if (NULL == assistant->tasks_ready_map)
    {
        printf("create assistant task ready map error...\n");
        return -1;
    }
    assistant->tasks_running_map = hashmap_new();
    if (NULL == assistant->tasks_running_map)
    {
        printf("create assistant task running map error...\n");
        return -1;
    }
    if (NULL == db_backend)
    {
        printf("assistant designed db_backend is NULL...\n");
        return -1;
    }
    assistant->db_backend = db_backend;
    return 0;
}

int assistant_destory(struct assistant *assistant)
{
    // remember free memory
    uv_mutex_lock(&assistant->mutex);
    printf("recycle...\n");
    list_each_elem(assistant->task_running_list, task)
    {
        list_elem_remove(task);
        hashmap_remove(assistant->tasks_running_map, (*task)->uuid);
        db_backend_put(assistant->db_backend, *task);
        task_destory(*task);
    }
    list_each_elem(assistant->task_ready_list, task)
    {
        list_elem_remove(task);
        hashmap_remove(assistant->tasks_ready_map, (*task)->uuid);
        // put back into db
        db_backend_put(assistant->db_backend, *task);
        task_destory(*task);
    }
    list_each_elem(assistant->task_done_list, task)
    {
        list_elem_remove(task);
        // put back into db
        db_backend_put(assistant->db_backend, *task);
        task_destory(*task);
    }
    uv_mutex_unlock(&assistant->mutex);
    free(assistant);
    return 0;
}

void assistant_inspector(struct assistant *assistant)
{
    printf("assistant inspector, key->\"%s\" p->%p\n", assistant->key, assistant);
    uv_mutex_lock(&assistant->mutex);
    assistant->tick ++;
    list_each_elem(assistant->task_running_list, task)
    {
        if ((*task)->tick > TASK_MAX_TICK)
        {
            list_elem_remove(task);
            hashmap_remove(assistant->tasks_running_map, (*task)->uuid);
            list_push(assistant->task_ready_list, *task);
            hashmap_put(assistant->tasks_ready_map, (*task)->uuid, *task);
        }
        else
        {
            (*task)->tick ++;
        }
    }
    uv_mutex_unlock(&assistant->mutex);
}

void assistant_container_inspector_cb(uv_work_t *req)
{
    struct assistants_container *container = req->data;
    printf("assistant container inspector...\n");
    uv_mutex_lock(&container->mutex);
    list_each_elem(container->assistants_list, assistant)
    {
        if ((*assistant)->tick > ASSISTANT_MAX_TICK)
        {
            list_elem_remove (assistant);
            hashmap_remove(container->assistants_map, (*assistant)->key);
            // recyle assistant
            assistant_destory(*assistant);
        }
        else
        {
            // warning, user should not queue uv_work in an uv_work callback.
            assistant_inspector(*assistant);
        }
    }
    uv_mutex_unlock(&container->mutex);
}

void assistant_container_inspector_after_cb(uv_work_t *req, int status)
{
    // clean memeory
    if (req)
    {
        free(req);
    }
}

void assistant_container_inspector_timer_cb(uv_timer_t *handle)
{
    int r;
    struct assistants_container *container = handle->data;
    uv_work_t *req = malloc(sizeof(uv_work_t));
    if (NULL == req)
    {
        printf("malloc uv_work_t error...\n");
        return;
    }
    req->data = container;
    r = uv_queue_work(container->loop, req, assistant_container_inspector_cb, assistant_container_inspector_after_cb);
    if (r)
    {
        printf("queue assistant container inspector error...\n");
    }
}

struct assistant *get_assistant_instance(struct assistants_container *container, char *key)
{
    int r;
    struct assistant *assistant;
    uv_mutex_lock(&container->mutex);
    r = hashmap_get(container->assistants_map, key, (void **)(&assistant));
    if (MAP_OK != r)
    {
        printf("assistant is not in assistant_map, create one...\n");
        assistant = malloc(sizeof(struct assistant));
        if (NULL == assistant)
        {
            printf("malloc struct assistant memeory error...\n");
            return NULL;
        }
        r = assistant_init(assistant, key, container->db_backend);
        if (r)
        {
            printf("initial assistant error...\n");
            return NULL;
        }
        r = hashmap_put(container->assistants_map, key, assistant);
        if (MAP_OK != r)
        {
            printf("put new assistant into container's hashmap error...\n");
            return NULL;
        }
        list_push(container->assistants_list, assistant);
    }
    assistant->tick = 0;
    uv_mutex_unlock(&container->mutex);
    return assistant;
}


int assistants_container_init(struct assistants_container *container,
                              uv_loop_t *loop,
                              struct db_backend* db_backend)
{
    int r;
    memset(container, 0, sizeof(struct assistants_container));
    container->loop = loop;
    r = uv_mutex_init(&container->mutex);
    if (r)
    {
        printf("initial assistant container mutex error...\n");
        return r;
    }
    r = uv_timer_init(container->loop, &container->inspector);
    if (r)
    {
        printf("initial assistant container inspector error...\n");
        uv_mutex_destroy(&container->mutex);
        return r;
    }
    container->inspector.data = container;
    r = uv_timer_start(&container->inspector, assistant_container_inspector_timer_cb, 3000, 2000);
    if (r)
    {
        printf("start assistant container inspector error...\n");
        uv_mutex_destroy(&container->mutex);
        uv_timer_stop(&container->inspector);
        return r;
    }
    container->assistants_map = hashmap_new();
    if (NULL == container->assistants_map)
    {
        printf("create assistant container hashmap error...\n");
        uv_mutex_destroy(&container->mutex);
        uv_timer_stop(&container->inspector);
        return -1;
    }
    if (0 != list_length(container->assistants_list))
    {
        printf("create assistant container list error...\n");
        uv_mutex_destroy(&container->mutex);
        uv_timer_stop(&container->inspector);
        return -1;
    }
    if (NULL == db_backend)
    {
        printf("assistant container designed db_backend is NULL...\n");
        uv_mutex_destroy(&container->mutex);
        uv_timer_stop(&container->inspector);
        return -1;
    }
    container->db_backend = db_backend;
    return 0;
}

int assistants_container_destory(struct assistants_container *container)
{
    uv_mutex_lock(&container->mutex);
    uv_timer_stop(&container->inspector);  // stop timer
    list_each_elem(container->assistants_list, assistant)  // destory all assistant
    {
        list_elem_remove (assistant);
        hashmap_remove(container->assistants_map, (*assistant)->key);
        // recyle assistant
        assistant_destory(*assistant);
    }
    hashmap_free(container->assistants_map);  // free hashmap
    uv_mutex_unlock(&container->mutex);
    uv_mutex_destroy(&container->mutex);  // destory mutex
    return 0;
}
