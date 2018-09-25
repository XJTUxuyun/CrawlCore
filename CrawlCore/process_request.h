//
//  process_request.h
//  CrawlCore
//
//  Created by 王媛莉 on 2018/9/12.
//  Copyright © 2018 徐云. All rights reserved.
//

#ifndef process_request_h
#define process_request_h

#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include "cJSON.h"
#include "version.h"
#include "assistant.h"
#include "tools.h"

extern struct assistants_container container;  // defined in main.c

extern map_t mid_set;

enum PROCESS_REQUEST_STATUS
{
    PRS_OK=0,
    PRS_JSON_PARSE_ERROR,
    PRS_NO_VERSION,
    PRS_VERSION_ERROR,
    PRS_VERSION_UNMATCHED,
    PRS_NO_MID,
    PRS_MID_ERROR,
    PRS_MID_INVALID,
    PRS_NO_TYPE,
    PRS_TYPE_INVALID,
    PRS_NO_UUID,
    PRS_UUID_ERROR,
    PRS_UUID_MISSING,
    PRS_NO_DATA,
    PRS_DATA_ERROR,
    PRS_NO_STATUS,
    PRS_STATUS_ERROR,
    PRS_GET_NOTHING,
    PRS_UNKNOWN
};

/**
 * process one request
 * @param request request data buf
 * @param nread request len
 */
uv_buf_t process_request(uv_buf_t *request,ssize_t nread);

#endif /* process_request_h */
