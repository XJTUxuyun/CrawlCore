//
//  assistant.c
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/27.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "assistant.h"

/**
 * intial struct assistant
 * @param assistant target variable
 */
int assistant_init(struct assistant *assistant, char *key)
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
    assistant->jiffies = 0;
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
    return 0;
}



void assistant_inspector(struct assistant *assistant)
{
    uv_mutex_lock(&assistant->mutex);
    list_each_elem(assistant->task_running_list, task)
    {
        struct task *t = *task;
        if (t->jiffies > TASK_MAX_JIFFIES)
        {
            list_elem_remove(task);
            hashmap_remove(assistant->tasks_running_map, t->uuid);
            list_push(assistant->tasks_ready_list, task);
            hashmap_put(assistant->tasks_ready_map, t->uuid, t);
        }
        else
        {
            t->jiffies ++;
        }
    }
    uv_mutex_unlock(&assistant->mutex);
}

void assistant_inspector_cb(uv_work_t *req)
{
    assistant_inspector(req->data);
}

void assistant_inspector_after_cb(uv_work_t *req, int status)
{
    // do nothing
}




void assistant_container_inspector(struct assistants_container *container)
{
    int r;
    uv_mutex_lock(&container->mutex);
    list_each_elem(container->assistants_list, assistant)
    {
        if ((*assistant)->jiffies > ASSISTANT_MAX_JIFFIES)
        {
            list_elem_remove (assistant);
            hashmap_remove(container->assistants_map, (*assistant)->key);
            // recyle assistant
            uv_work_t work;
            work.data = *assistant;
        }
        else
        {
            uv_work_t work;
            work.data = *assistant;
            r = uv_queue_work(container->loop, &work, assistant_inspector_cb, assistant_inspector_after_cb);
            if (r)
            {
                printf("post assistant_inspector error...\n");
            }
        }
    }
    uv_mutex_unlock(&container->mutex);
}

void assistant_container_inspector_cb(uv_work_t *req)
{
    printf("assistant container inspector...\n");
    assistant_container_inspector(req->data);
}

void assistant_container_inspector_after_cb(uv_work_t *req, int status)
{
    // do nothing
}

void assistant_container_inspector_timer_cb(uv_timer_t *handle)
{
    struct assistants_container *container = handle->data;
    uv_work_t req;
    req.data = handle->data;
    int r = uv_queue_work(container->loop, &req, assistant_container_inspector_cb, assistant_container_inspector_after_cb);
    if (r)
    {
        printf("queue assistant container inspector error...\n");
    }
}

int assistant_recycle(struct assistant *assistant)
{
    return 0;
}

struct assistant *get_assistant_instance(struct assistants_container *container, char *key)
{
    int r;
    struct assistant *assistant;
    uv_mutex_lock(&container->mutex);
    if (MAP_OK != hashmap_get(container->assistants_map, key, (void **)(&assistant)))
    {
        assistant = malloc(sizeof(struct assistant));
        if (NULL == assistant)
        {
            printf("malloc struct assistant memeory error...\n");
            return NULL;
        }
    
        r = assistant_init(assistant, key);
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
    assistant->jiffies = 0;
    uv_mutex_unlock(&container->mutex);
    return assistant;
}

/**
 * initial assistant contanier
 * @param container container instance
 * @param loop container ref loop
 */
int assistants_container_init(struct assistants_container *container,
                              uv_loop_t *loop)
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
    return 0;
}
