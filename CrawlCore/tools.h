//
//  tools.h
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/28.
//  Copyright © 2018 徐云. All rights reserved.
//

#ifndef tools_h
#define tools_h

#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include "cJSON.h"

/**
 * when connected socket can read, alloc sufficient memory to save data
 * suggested_size if 64k, we donot need such large memeory
 * now 1k is enough
 * @param handle triggered handle
 * @param suggested_size suggested_size is pass from xxx function, default 64K
 * @param buf buf->base which need alloc
 */
void alloc_cb(uv_handle_t *handle,
              size_t suggested_size,
              uv_buf_t *buf);

/*
 * render cJSON object to uv_buf_t for transfer
 * not maintain object memeory
 * @param object which to render
 * @return uv_buf_t
 */
uv_buf_t json2uv_buf_t(cJSON *object);

#endif /* tools_h */
