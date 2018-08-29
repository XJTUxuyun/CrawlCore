//
//  config.h
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/20.
//  Copyright © 2018 徐云. All rights reserved.
//

#ifndef config_h
#define config_h

#include <stdio.h>

typedef struct __CONFIG__
{
    char local_ip[128];
    int port;
} Config;

/**
 * load & parse config file
 * @param path config file path
 * @param config pointer to config object
 * return 0 if success
 **/
int load_config(char *path, Config *config);  // 加载配置

#endif /* config_h */
