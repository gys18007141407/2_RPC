/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-29 13:30:16
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-29 13:30:16
 */

#include "logger.h"
#include <unistd.h>

int main(){

    char info[64] = "today is Thursdy";
    char error[64] = "haste to staudy";
    LOG_INFO("INFO LOG: %s", info);
    LOG_ERROR("ERROR LOG: %s", error);
    return 0;
}