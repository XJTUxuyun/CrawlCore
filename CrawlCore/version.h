//
//  version.h
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/20.
//  Copyright © 2018 徐云. All rights reserved.
//

#ifndef version_h
#define version_h

#include <stdio.h>

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 3
#define VERSION_IS_RELEASE 1
#define VERSION_SUFFIX ""

#define VERSION_HEX  ((VERSION_MAJOR << 16) | (VERSION_MINOR <<  8) | (VERSION_PATCH))

#define STRINGIFY(v) STRINGIFY_HELPER(v)
#define STRINGIFY_HELPER(v) #v

#define VERSION_STRING_BASE  STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH)

#if VERSION_IS_RELEASE
# define VERSION_STRING  VERSION_STRING_BASE
#else
# define VERSION_STRING  VERSION_STRING_BASE "-" VERSION_SUFFIX
#endif

/**
 * return version int
 **/
static unsigned int version(void) {
    return VERSION_HEX;
}

/**
 * return version string
 **/
static const char* version_string(void) {
    return VERSION_STRING;
}

#endif /* version_h */
