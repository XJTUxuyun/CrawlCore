//
//  main.c
//  CourtCrawlCore
//  项目为裁判文书网核心调度器
//  别问我为啥这么干，任性
//  Created by 徐云 on 2018/8/5.
//  Copyright © 2018 徐云. All rights reserved.
//

#include <stdio.h>
#include <log4c.h>
#include <uv.h>
#include "config.h"
#include "server.h"
#include "version.h"
#include <time.h>
#include "server.h"
#include "hashmap.h"
#include <stdlib.h>
#include "db_backend.h"

static uv_loop_t * loop = NULL;
Config config;  // 全局配置


int pfan(any_t item, any_t data)
{
    printf("get value->%d\n",*(char *)data);
    return MAP_OK;
}

void work2_cb(uv_work_t *req)
{
    printf("work2cb\n");

    
}
void work2_cb_after(uv_work_t *req, int status)
{
    printf("work2cbafter\n");
}

void work1_cb(uv_work_t *req)
{
    printf("work1cb\n");
    uv_work_t work1;
    work1.data = req->data;
    //uv_queue_work(req->data, &work1, work2_cb, work2_cb_after);
    
}
void work1_cb_after(uv_work_t *req, int status)
{
    printf("work1cbafter\n");
}

int main(int argc, char ** argv)
{
    struct db_backend db;
    db_backend_init("/Users/wyl/git-workspace/CourtCrawlCore/test.db3", &db);
    struct task task;
    
    task_init(&task, NULL);
    task.mid = 10;
    task.data = "fuck";
    task.len = strlen(task.data) + 1;
    db_backend_put(&db, &task);
    struct task task1;
    task_init(&task1, NULL);
    
    db_backend_get(&db, &task1);
    printf("task1->uuid-> %s  data->%s\n", task1.uuid, task1.data);
    int r;
    r = log4c_init();
    assert(0 == r);
    if (r)
    {
        printf("inital log4c error...\n");
        return -1;
    }
    else
    {
        printf("intial log4c success...\n");
    }
    log4c_category_t *cat = log4c_category_get("core.crawl");
    log4c_category_log(cat, LOG4C_PRIORITY_INFO, "shit");
    log4c_category_log(cat, LOG4C_PRIORITY_ERROR, "fuck");
    printf("version %d\n", version());
    printf("version %s\n", version_string());
    
    
    map_t m = hashmap_new();
    char *a = "hello";
    char *b = "fuck";
 
    r = hashmap_put(m, a, b);
    assert(MAP_OK == r);
    if (r != MAP_OK)
    {
        printf("map put error...\n");
    }
    char *c;
    r = hashmap_get(m, a, (void **)(&c));
    if (MAP_OK == r)
    {
        printf("fuck");
    }
    else
    {
        printf("shit");
    }
    printf("test->%s", c);
   // hashmap_iterate(m, pfan, NULL);
    
    /*
    int r;
    time_t t;
    time(&t);
    printf("time->%d", ctime(&t));
    // 主代码
    r = load_config("config.xml", &config);
    if (r)
    {
        // 加载配置失败
        printf("load config error...\n");
        return -1;
    }
    else
    {
        // 加载配置成功
        printf("load config success..\n");
        printf("local ip->%s\tport->%d", config.local_ip, config.port);
        
    }
    
    printf("version %d\n", VERSION_HEX);
    printf("version %s\n", version_string());
    
    
    loop = uv_default_loop();
    
    client_set_init(&cs);
    
    // server
    struct sockaddr_in addr;
    
    r = uv_ip4_addr("0.0.0.0", 9001, &addr);
    if (r)
    {
        printf("initialize sockaddr_in error, process will terminate...\n");
    }
    
    r = uv_tcp_init(loop, &server);
    if (r)
    {
        printf("initialize tcp handle error, process will terminate...\n");
    }
    
    r = uv_tcp_bind(&server, (struct sockaddr *)&addr, 0);
    if (r)
    {
        printf("bin listening port error, process will terminate...\n");
    }
    
    r = uv_listen(&server, 128, accept_connection);
    if (r)
    {
        printf("listening designed port error, process will terminate...\n");
    }
    
    uv_timer_t monitor_timer;
    r = uv_timer_init(loop, &monitor_timer);
    if (r)
    {
        printf("initialize monitor timer error...\n");
    }
    
    r = uv_timer_start(&monitor_timer, monitor_cb, 3*1000, 3*1000);
    if (r)
    {
        printf("start monitor timer error...\n");
    }
    
    r = uv_run(loop, UV_RUN_DEFAULT);
    if (r)
    {
        printf("run loop error, process will terminate...\n");
    }
    
    r = log4c_fini();  // 去初始化
    if(r)
    {
        printf("关闭日志失败...\n");
    }
    else
    {
        printf("关闭日志成功，程序即将退出...\n");
    }
    
    return 0;*/
    loop = uv_default_loop();
    
    struct assistants_container container;
    r = assistants_container_init(&container, loop, &db);
    get_assistant_instance(&container, "fuck");
    
    uv_work_t work;
    work.data = loop;
    uv_queue_work(loop, &work, work1_cb, work1_cb_after);
    
    struct server s;
    server_init(&s, "test---------", TCP_SERVER, loop, "127.0.0.1", 9001);
    uv_run(loop, UV_RUN_DEFAULT);
    r = log4c_fini();
    assert(0 == r);
    if (r)
    {
        printf("clean log error...\n");
    }
    else
    {
        printf("clean log success...\n");
    }
    return 0;
}

