//
//  tools.c
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/28.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "tools.h"

void alloc_cb(uv_handle_t *handle,
              size_t suggested_size,
              uv_buf_t *buf)
{
    suggested_size = 1024;
    buf->base = (char *)malloc(suggested_size);
    memset(buf->base, 0, suggested_size);
    buf->len = suggested_size;
}

uv_buf_t json2uv_buf_t(cJSON *object)
{
    uv_buf_t buf = {NULL, 0};
    char *t = cJSON_PrintUnformatted(object);
    buf.len = strlen(t);
    buf.base = malloc(buf.len + 1);
    strcpy(buf.base, t);
    if (NULL != t)  // void memeory leak.
    {
        free(t);
    }
    return buf;
}
