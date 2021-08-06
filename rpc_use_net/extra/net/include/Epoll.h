/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 20:09:52
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 09:01:31
 */

#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include <vector>
#include <memory>

#include "Channel.h"
#include "Config.h"
#include "Helper.h"


class Epoll{
public:
    using UPtrEpoll = std::unique_ptr<Epoll>;
    explicit Epoll();
    ~Epoll();

    void update(Channel* channel);

    void add(Channel* channel);

    void remove(Channel* channel);

    std::size_t wait(std::vector<Channel*>& channels);

private:
    epoll_event _epollEvents[MAXEVENTS];

    epoll_event _ev;

    const std::size_t _epollfd;

};

#endif