/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-03 09:27:17
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 09:27:18
 */

#ifndef HELPER_H
#define HELPER_H

#include <cstddef>
#include <thread>
#include <sys/socket.h>

std::size_t get_thread_id();

void setFdReuse(std::size_t fd);

#endif
