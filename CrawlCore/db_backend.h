//
//  db_backend.h
//  CrawlCore
//
//  Created by 王媛莉 on 2018/9/10.
//  Copyright © 2018 徐云. All rights reserved.
//

#ifndef db_backend_h
#define db_backend_h

#include <stdio.h>
#include <sqlite3.h>
#include "assistant.h"

struct db_backend
{
    sqlite3 *db;
    sqlite3_mutex *mutex;
};

int db_backend_init(char *db_path, struct db_backend *db_backend);

int db_backend_destory(struct db_backend *db_backend);

int db_backend_put(struct db_backend *db_backend, struct task *task);

int db_backend_puts(struct db_backend *db_backend, list(struct task, task));

//int db_backend_get(struct db_backend *db_backend, struct task *task);

#endif /* db_backend_h */
