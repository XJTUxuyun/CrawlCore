//
//  process_request.c
//  CrawlCore
//
//  Created by 王媛莉 on 2018/9/12.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "process_request.h"

uv_buf_t process_request_parse_error(cJSON *req, char *error)
{
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL)
    {
        printf("Error before: %s\n", error_ptr);
    }
    cJSON_Delete(req);
    uv_buf_t ret = {NULL, 0};
    cJSON *r = cJSON_CreateObject();
    cJSON *str = cJSON_CreateString(error);
    cJSON_AddItemToObject(r, "status", str);
    char *t = cJSON_PrintUnformatted(r);
    ret.len = strlen(t);
    ret.base =  malloc(ret.len);
    strcpy(ret.base, t);
    cJSON_Delete(r);
    free(t);
    return ret;
}

uv_buf_t process_request(uv_buf_t *request,ssize_t nread)
{
    uv_buf_t ret = {NULL, 0};
    cJSON *req = cJSON_Parse(request->base);
    if (req == NULL)  // process request error.
    {
        return process_request_parse_error(req, "parse error");
    }
    
    // process version
    cJSON *ver = cJSON_GetObjectItem(req, "version");
    if (!ver)
    {
        return process_request_parse_error(req, "must contains version");
    }
    if (!cJSON_IsNumber(ver))
    {
        return process_request_parse_error(req, "version is a integer");
    }
    if (ver->valueint != version())
    {
        return process_request_parse_error(req, "version unmatched");
    }
    
    // process mid
    cJSON *mid = cJSON_GetObjectItem(req, "mid");
    if (!mid)
    {
        return process_request_parse_error(req, "must contains mid");
    }
    if (!cJSON_IsNumber(mid))
    {
        return process_request_parse_error(req, "mid is a integer");
    }
    char key[64] = {0};
    snprintf(key, 64,"mid->%d", mid->valueint);
    printf("key -> %s\n", key);
    
    
    
    cJSON *type = cJSON_GetObjectItem(req, "type");
    if (!type)
    {
        return process_request_parse_error(req, "must contains type");
    }
    if (!cJSON_IsNumber(type) || !(type->valueint == 0 || type->valueint == 1 || type->valueint == 2))
    {
        return process_request_parse_error(req, "type is 0, 1 or 2, 0 for get a task, 1 for post a task and 2 for ack");
    }
    struct assistant *assistant = get_assistant_instance(&container, key);
    printf("assistant->%p\n", assistant);
    return process_request_parse_error(req, "request mid is invalid");
    switch (type->valueint)
    {
        case 0:  // for get one task
            ;
            break;
        case 1:  // for post one task
            ;
            break;
        case 2:  // for acknowledgement number, main purpose is verfiy one task is success or fail.
            break;
        default:
            break;
    }
    
    return ret;
    
}
