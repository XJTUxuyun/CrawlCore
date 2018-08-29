//
//  config.c
//  CourtCrawlCore
//
//  Created by 王媛莉 on 2018/8/20.
//  Copyright © 2018 徐云. All rights reserved.
//

#include "config.h"
#include <libxml/tree.h>
#include <libxml/parser.h>

int load_config(char *path, Config *config)
{
    memset(config, 1, sizeof(Config));  // 初始化配置文件
    xmlDocPtr pdoc = NULL;
    xmlNodePtr proot = NULL, pcur = NULL;
    xmlKeepBlanksDefault(0);  // 必须加上，防止程序把元素前后的空白文本符号当作一个node
    pdoc = xmlReadFile (path, "UTF-8", XML_PARSE_RECOVER);  // libxml只能解析UTF-8格式数据
    if (pdoc == NULL)
    {
        printf ("error:can't open file!\n");
        return -1;
    }
    
    proot = xmlDocGetRootElement (pdoc);
    
    if (proot == NULL)
    {
        printf("error: file is empty!\n");
        return -1;
    }
    
    pcur = proot->xmlChildrenNode;
    
    while (pcur != NULL)
    {
        if (!xmlStrcmp(pcur->name, BAD_CAST("network")))
        {
            xmlNodePtr nptr=pcur->xmlChildrenNode;
            while (nptr != NULL)
            {
                if (!xmlStrcmp(nptr->name, BAD_CAST("title")))
                {
                    printf("title: %s\n",((char*)XML_GET_CONTENT(nptr->xmlChildrenNode)));
                    nptr = nptr->next;
                    continue;
                }
                
                if (!xmlStrcmp(nptr->name, BAD_CAST("ip")))
                {
                    printf("ip: %s\n",((char*)XML_GET_CONTENT(nptr->xmlChildrenNode)));
                    strcpy(config->local_ip, (char *)XML_GET_CONTENT(nptr->xmlChildrenNode));
                    nptr = nptr->next;
                    continue;
                }
                
                if (!xmlStrcmp(nptr->name, BAD_CAST("port")))
                {
                    printf("port: %s\n",((char*)XML_GET_CONTENT(nptr->xmlChildrenNode)));
                    config->port = atoi(((char*)XML_GET_CONTENT(nptr->xmlChildrenNode)));
                    nptr = nptr->next;
                    continue;
                }
                
            }
            
        }
        
        if (xmlStrcmp(pcur->name, BAD_CAST("stroage")))
        {
            xmlNodePtr nptr=pcur->xmlChildrenNode;
            while (nptr != NULL)
            {
                if (!xmlStrcmp(nptr->name, BAD_CAST("path")))
                {
                    printf("path: %s\n",((char*)XML_GET_CONTENT(nptr->xmlChildrenNode)));
                    nptr = nptr->next;
                    continue;
                }
                
            }
        }
        
        pcur = pcur->next;
    }
    
   
    // 释放资源
    xmlFreeDoc (pdoc);
    xmlCleanupParser ();
    xmlMemoryDump ();
    return 0;
}
