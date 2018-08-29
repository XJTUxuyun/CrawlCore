//
//  assistant.c
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/27.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "assistant.h"

/**
 * intial struct assistant
 * @param assis target variable
 */
int assistant_init(struct assistant *assis)
{
    int r;
    memset(assis, 0, sizeof(struct assistant));
    r = uv_mutex_init(&assis->mutex);
    assert(0 == r);
    if (r)
    {
        uv_mutex_destroy(& assis->mutex);
        return -1;
    }
    return 0;
}
