//
//  process_request.c
//  CrawlCore
//
//  Created by 王媛莉 on 2018/9/12.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "process_request.h"

uv_buf_t process_request_version_error()
{
    uv_buf_t ret = {NULL, 0};
    cJSON *r = cJSON_CreateObject();
    cJSON *str = cJSON_CreateString("version unmatch");
    cJSON_AddItemToObject(r, "status", str);
    char *t = cJSON_PrintUnformatted(r);
    ret.len = strlen(t);
    ret.base =  malloc(ret.len);
    strcpy(ret.base, t);
    cJSON_Delete(r);
    free(t);
    return ret;
}

uv_buf_t process_request_parse_error(cJSON *req)
{
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL)
    {
        printf("Error before: %s\n", error_ptr);
    }
    cJSON_Delete(req);
    uv_buf_t ret = {NULL, 0};
    cJSON *r = cJSON_CreateObject();
    cJSON *str = cJSON_CreateString("request error");
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
        return process_request_parse_error(req);
    }
    
    // process version
    cJSON *ver = cJSON_GetObjectItem(req, "version");
    int req_ver = ver->valueint;
    if (req_ver != version())
    {
        cJSON_Delete(req);
        cJSON_Delete(ver);
        return process_request_version_error();
    }
    
    // 
    return ret;
    
}
