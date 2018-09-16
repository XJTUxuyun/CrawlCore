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
    char *err_msg;
    memset(db_backend, 0 , sizeof(struct db_backend));
    r = sqlite3_open_v2(db_path, &db_backend->db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);
    if (SQLITE_OK != r)
    {
        printf("open->%s error, %s\n", db_path, sqlite3_errmsg(db_backend->db));
        return -1;
    }
    char *sql = "create table if not exists test(uuid char(37) primary key not null, mid int, status int, ctime long, mtime long, retry int, data text)";
    r = sqlite3_exec(db_backend->db, sql, NULL, 0, &err_msg);
    if (SQLITE_OK != r)
    {
        printf("create table error, %s.\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    return 0;
}

int db_backend_destory(struct db_backend *db_backend)
{
    sqlite3_close(db_backend->db);
    return 0;
}

int db_backend_put(struct db_backend *db_backend, struct task *task)
{
    int r;
    char *err_msg;
    sqlite3_stmt *stmt;
    char *sql = "insert or replace into test(uuid, mid, status, ctime, mtime, retry, data) values(?, ?, ?, ?, ?, ?, ?)";
    while( (r = sqlite3_prepare_v2(db_backend->db, sql, -1, &stmt, NULL)) != SQLITE_OK);
    if (SQLITE_OK != r)
    {
        printf("db_backend_put prepare error->%s\n", sqlite3_errmsg(db_backend->db));
        return -1;
    }
    r = sqlite3_bind_text(stmt, 1, task->uuid, UUID4_LEN, NULL);
    if (SQLITE_OK != r)
    {
        goto bind_err;
    }
    r = sqlite3_bind_int(stmt, 2, task->mid);
    if (SQLITE_OK != r)
    {
        goto bind_err;
    }
    r = sqlite3_bind_int(stmt, 3, task->status);
    if (SQLITE_OK != r)
    {
        goto bind_err;
    }
    r = sqlite3_bind_int64(stmt, 4, task->ctime);
    if (SQLITE_OK != r)
    {
        goto bind_err;
    }
    r = sqlite3_bind_int64(stmt, 5, task->mtime);
    if (SQLITE_OK != r)
    {
        goto bind_err;
    }
    r = sqlite3_bind_int64(stmt, 6, task->retry);
    if (SQLITE_OK != r)
    {
        goto bind_err;
    }
    r = sqlite3_bind_text(stmt, 7, task->data, task->len, NULL);
    if (SQLITE_OK != r)
    {
        goto bind_err;
    }
    while(SQLITE_DONE != (r = sqlite3_step(stmt)));
    if (SQLITE_DONE != r)
    {
        goto step_err;
    }
    sqlite3_finalize(stmt);
    return 0;
bind_err:
    printf("db_backend_put bind error->%s\n", sqlite3_errmsg(db_backend->db));
    sqlite3_finalize(stmt);
    return -1;
step_err:
    printf("error2->%s\n", sqlite3_errmsg(db_backend->db));
    sqlite3_finalize(stmt);
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
    list_each_elem(task, t)
    {
        
    }
    return 0;
}

int db_backend_get(struct db_backend *db_backend, int mid, struct task *task)
{
    int r;
    sqlite3_stmt *stmt;
    char tmp[128] = {0};
    char *sql = "select * from test where mid = %d and status = 2";
    sprintf(tmp, sql, mid);
    while( (r = sqlite3_prepare_v2(db_backend->db, tmp, -1, &stmt, NULL)) != SQLITE_OK);
    if (SQLITE_OK != r)
    {
        printf("db_backend_get error->%s\n", sqlite3_errmsg(db_backend->db));
        sqlite3_finalize(stmt);
        return -1;
    }
    while (SQLITE_ROW != (r = sqlite3_step(stmt)));
    const unsigned char *uuid = sqlite3_column_text(stmt, 0);
    strcpy(task->uuid, uuid);
    task->mid = sqlite3_column_int(stmt, 1);
    task->status = sqlite3_column_int(stmt, 2);
    task->ctime = sqlite3_column_int64(stmt, 3);
    task->mtime = sqlite3_column_int64(stmt, 4);
    task->retry = sqlite3_column_int(stmt, 5);
    const unsigned char *data = sqlite3_column_text(stmt, 6);
    int len = sqlite3_column_bytes(stmt, 6);
    task->len = len;
    task->data = malloc(sizeof(len));
    memcpy(task->data, data, len);
    
    sqlite3_finalize(stmt);
    return 0;
}
