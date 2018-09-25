//
//  process_request.c
//  CrawlCore
//
//  Created by 王媛莉 on 2018/9/12.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "process_request.h"

uv_buf_t process_request_parse_error(cJSON *req, char *error, int status)
{
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL)
    {
        printf("Error before: %s\n", error_ptr);
    }
    cJSON_Delete(req);
    cJSON *r = cJSON_CreateObject();
    cJSON *str = cJSON_CreateString(error);
    cJSON_AddItemToObject(r, "message", str);
    cJSON_AddNumberToObject(r, "status", status);
    uv_buf_t ret = json2uv_buf_t(r);
    cJSON_Delete(r);
    return ret;
}

uv_buf_t process_request_get_task(struct assistant *assistant, cJSON *req)
{
    cJSON *mid = cJSON_GetObjectItem(req, "mid");
    struct task *task = assistant_get(assistant, mid->valueint);
    cJSON *r = cJSON_CreateObject();
    if (!task)
    {
        cJSON *m = cJSON_CreateString("cannot find one task");
        cJSON_AddItemToObject(r, "message", m);
        cJSON_AddNumberToObject(r, "status", PRS_GET_NOTHING);
    }
    else
    {
        cJSON *m = cJSON_CreateString("ok");
        cJSON_AddItemToObject(r, "message", m);
        cJSON_AddNumberToObject(r, "status", PRS_OK);
        cJSON *data = cJSON_Parse(task->data);
        printf("data->%s\n", task->data);
        cJSON_AddRawToObject(r, "data", data);
    }
    uv_buf_t ret = json2uv_buf_t(r);
    cJSON_Delete(r);
    return ret;
}

uv_buf_t process_request_post_task(struct assistant *assistant, cJSON *req)
{
    cJSON *uuid = cJSON_GetObjectItem(req, "uuid");
    if (NULL == uuid)
    {
        return process_request_parse_error(req, "must contains uuid", PRS_NO_UUID);
    }
    if (!cJSON_IsString(uuid))
    {
        return process_request_parse_error(req, "uuid must be string", PRS_UUID_ERROR);
    }
    
    cJSON *data = cJSON_GetObjectItem(req, "data");
    if (NULL == data)
    {
        return process_request_parse_error(req, "must contains data", PRS_NO_DATA);
    }

    char *data_c = cJSON_PrintUnformatted(data);
    if (NULL == data_c)
    {
        return process_request_parse_error(req, "render data error", PRS_DATA_ERROR);
    }
    
    struct task *post;
    post = (struct task *)malloc(sizeof(struct task));
    task_init(post, uuid->valuestring);
    post->status = 1;
    post->mid = cJSON_GetObjectItem(req, "mid")->valueint;
    post->len = strlen(data_c);
    post->data = malloc(post->len + 1);
    strcpy(post->data, data_c);
    char *mess;
    int status = assistant_post(assistant, post);
    if (status != 0)
    {
        // error occur.
        mess = "post task failed";
    }
    else
    {
        // ok.
        mess = "post task success";
    }
    cJSON *r = cJSON_CreateObject();
    cJSON *m = cJSON_CreateString(mess);
    cJSON_AddItemToObject(r, "message", m);
    cJSON_AddNumberToObject(r, "status", status);
    uv_buf_t ret = json2uv_buf_t(r);
    cJSON_Delete(r);
    return ret;  // everything is ok.
}

uv_buf_t process_request_ack_task(struct assistant *assistant, cJSON *req)
{
    cJSON *uuid = cJSON_GetObjectItem(req, "uuid");
    if (NULL == uuid)
    {
        return process_request_parse_error(req, "must contains uuid", PRS_NO_UUID);
    }
    if (!cJSON_IsString(uuid))
    {
        return process_request_parse_error(req, "uuid must be string", PRS_UUID_ERROR);
    }
    
    cJSON *status = cJSON_GetObjectItem(req, "status");
    if (NULL == status)
    {
        return process_request_parse_error(req, "must contains status", PRS_NO_STATUS);
    }
    if (!cJSON_IsNumber(status) || !(status->valueint == 0 || status->valueint == 1))
    {
        return process_request_parse_error(req, "status error", PRS_STATUS_ERROR);
    }
    int s = assistant_ack(assistant, uuid->valuestring, status->valueint);
    char *mess;
    cJSON *r = cJSON_CreateObject();
    if (s)
    {
        mess = "uuid missing";
        cJSON_AddNumberToObject(r, "status", PRS_UUID_MISSING);
    }
    else
    {
        mess = "ok";
        cJSON_AddNumberToObject(r, "status", PRS_OK);
    }
    cJSON *m = cJSON_CreateString(mess);
    cJSON_AddItemToObject(r, "message", m);
    uv_buf_t ret = json2uv_buf_t(r);
    cJSON_Delete(r);
    return ret;  // everything is ok.
}

uv_buf_t process_request(uv_buf_t *request,ssize_t nread)
{
    cJSON *req = cJSON_Parse(request->base);
    if (req == NULL)  // process request error.
    {
        return process_request_parse_error(req, "parse error", PRS_JSON_PARSE_ERROR);
    }
    
    // process version
    cJSON *ver = cJSON_GetObjectItem(req, "version");
    if (!ver)
    {
        return process_request_parse_error(req, "must contains version", PRS_NO_VERSION);
    }
    if (!cJSON_IsNumber(ver))
    {
        return process_request_parse_error(req, "version is a integer", PRS_VERSION_ERROR);
    }
    if (ver->valueint != version())
    {
        return process_request_parse_error(req, "version unmatched", PRS_VERSION_UNMATCHED);
    }
    
    // process mid
    cJSON *mid = cJSON_GetObjectItem(req, "mid");
    if (!mid)
    {
        return process_request_parse_error(req, "must contains mid", PRS_NO_MID);
    }
    if (!cJSON_IsNumber(mid))
    {
        return process_request_parse_error(req, "mid is a integer", PRS_MID_ERROR);
    }
    char key[64] = {0};
    snprintf(key, 64,"mid->%d", mid->valueint);
    if (MAP_MISSING == hashmap_key_exist(mid_set, key))
    {
        return process_request_parse_error(req, "req mid is invalid", PRS_MID_INVALID);
    }
    
    cJSON *type = cJSON_GetObjectItem(req, "type");
    if (!type)
    {
        return process_request_parse_error(req, "must contains type", PRS_NO_TYPE);
    }
    if (!cJSON_IsNumber(type) || !(type->valueint == 0 || type->valueint == 1 || type->valueint == 2))
    {
        return process_request_parse_error(req, "type is 0, 1 or 2, 0 for get a task, 1 for post a task and 2 for ack", PRS_TYPE_INVALID);
    }
    struct assistant *assistant = get_assistant_instance(&container, key);
    printf("assistant->%p  key->%s\n", assistant, assistant->key);
    
    switch (type->valueint)
    {
        case 0:  // for get one task
            return process_request_get_task(assistant, req);
            break;
        case 1:  // for post one task
            return process_request_post_task(assistant, req);
            break;
        case 2:  // for acknowledgement number, main purpose is verfiy one crawl task is success or fail, just for type get.
            return process_request_ack_task(assistant, req);
            break;
        default:
            break;
    }
    
    return process_request_parse_error(req, "request error unknown", PRS_UNKNOWN);
}
