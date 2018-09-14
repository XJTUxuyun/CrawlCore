//
//  db_backend.c
//  CrawlCore
//
//  Created by 王媛莉 on 2018/9/10.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "db_backend.h"


int db_backend_init(char *db_path, struct db_backend *db_backend)
{
    int r;
    const char *err_msg;
    memset(db_backend, 0 , sizeof(struct db_backend));
    r = sqlite3_open(db_path, &db_backend->db);
    if (SQLITE_OK != r)
    {
        err_msg = sqlite3_errmsg(db_backend->db);
        printf("open->%s error, %s\n", db_path, err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    db_backend->mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_FAST);
    if (NULL == db_backend->mutex)
    {
        printf("create sqlite mutext error.\n");
        sqlite3_close(db_backend->db);
        return -1;
    }
    char *sql = "create table if not exists test(uuid char(37) primary key, mid int, status char, ctime long, mtime long, retry int, data blob)";
    r = sqlite3_exec(db_backend->db, sql, NULL, 0, &err_msg);
    if (SQLITE_OK != r)
    {
        printf("create table error, %s.\n", err_msg);
        return -1;
    }
    return 0;
}

int db_backend_destory(struct db_backend *db_backend)
{
    sqlite3_mutex_free(db_backend->mutex);
    sqlite3_close(db_backend->db);
    return 0;
}

int db_backend_put(struct db_backend *db_backend, struct task *task)
{
    int r;
    const char *err_msg;
    sqlite3_mutex_enter(db_backend->mutex);
    sqlite3_stmt *stat;
    char *sql = "insert or replace into test(uuid, mid, status, ctime, mtime, retry, data) values(%s, %d, %c, %l, %l, %d, ?)";
    char tmp[256];
    sprintf(tmp, sql, task->uuid, task->mid, task->status, task->ctime, task->mtime, task->retry);
    printf("%s\n", task->uuid);
    r = sqlite3_prepare(db_backend->db, tmp, -1, &stat, 0 );
    if (SQLITE_OK != r)
    {
        err_msg = sqlite3_errmsg(db_backend->db);
        printf("error1->%s\n", err_msg);
        //sqlite3_free(err_msg);
        sqlite3_mutex_leave(db_backend->mutex);
        return -1;
    }
    r = sqlite3_bind_blob(stat, 1, task->data, task->len, NULL);
    if (SQLITE_OK != r)
    {
        goto bind_err;
    }
    r = sqlite3_step(stat);
    if (SQLITE_DONE != r)
    {
        goto step_err;
    }
    sqlite3_finalize(stat);
    sqlite3_mutex_leave(db_backend->mutex);
    return 0;
bind_err:
    err_msg = sqlite3_errmsg(db_backend->db);
    printf("error->%s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_finalize(stat);
    sqlite3_mutex_leave(db_backend->mutex);
    return -1;
step_err:
    err_msg = sqlite3_errmsg(db_backend->db);
    printf("error->%s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_finalize(stat);
    sqlite3_mutex_leave(db_backend->mutex);
    return -1;
}

int db_backend_puts(struct db_backend *db_backend, list(struct task, task))
{
    if (list_length(task) == 0)
    {
        printf("db_backend_puts task length is zero.\n");
        return 0;
    }
    int r;
    char *err_msg;
    sqlite3_stmt *stmt;
    sqlite3_mutex_enter(db_backend->mutex);
    list_each_elem(task, t)
    {
        
    }
    sqlite3_mutex_leave(db_backend->mutex);
    return 0;
}

int db_backend_get(struct db_backend *db_backend, struct task *task)
{
    int r;
    char *err_msg;
    int index = 10;
    sqlite3_mutex_enter(db_backend->mutex);
    sqlite3_stmt *stat;
    char *sql = "select * from test";
    r = sqlite3_prepare(db_backend->db, sql, -1, &stat, 0);
    if (SQLITE_OK != r)
    {
        err_msg = sqlite3_errmsg(db_backend->db);
        printf("error->%s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_mutex_leave(db_backend->mutex);
        sqlite3_finalize(stat);
        return -1;
    }

    
    sqlite3_mutex_leave(db_backend->mutex);
    return 0;
}
