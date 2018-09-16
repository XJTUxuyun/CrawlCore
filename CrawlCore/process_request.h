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

/**
 * process one request
 * @param request request data buf
 * @param nread request len
 */
uv_buf_t process_request(uv_buf_t *request,ssize_t nread);

#endif /* process_request_h */
