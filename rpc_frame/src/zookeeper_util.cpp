/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-29 15:59:36
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-29 15:59:36
 */

#include <iostream>
#include <semaphore.h>

#include "zookeeper_util.h"
#include "rpc_application.h"

// global watcher
void global_watcher(zhandle_t* _zhandle, int _type, int _state, const char* _path, void* _watchCtx){
    if(_type == ZOO_SESSION_EVENT){
        if(_state == ZOO_CONNECTED_STATE){
            sem_t* sem = (sem_t*) zoo_get_context(_zhandle);
            sem_post(sem);  // +1
        }
    }
}

// client start to connect to server
void CLZooKeeperClient::start(){
    std::string ip = CLRPCApplication::get_instance().get_config().load("ZOOKEEPER_IP");
    std::string port = CLRPCApplication::get_instance().get_config().load("ZOOKEEPER_PORT");
    std::string addr = ip+":"+port;

    /*  zookeeper_mt 多线程版本
        zookeeper的API客户端程序提供了3个线程
        1、API调用线程：只做内存的申请等前提工作（zookeeper_init），连接工作由下面的线程完成
        2、网络IO线程：负责连接zookeeper server。客户端不需要高并发，因此是通过poll实现
        3、watcher回调线程：负责执行我们设置的回调函数

        整个过程是异步的

        异步等待直到回调中 type == ZOO_SESSION_EVENT && state == ZOO_CONNECTED_STATE 才给出响应

    */


    mp_zhandle = zookeeper_init(addr.c_str(), global_watcher, 100, nullptr, nullptr, 0);
    // host： 逗号隔开的host:port对，每个代表一个zk server，比如：“127.0.0.1:3000，127.0.0.1:3001，127.0.0.1:3002”
    // fn： Watcher回调函数
    // clientid： 客户端尝试重连的先前的会话ID，如果不重连先前的会话，则设置为0。客户端可以通过调用zoo_client_id来访问一个已经连接上的、有效的会话ID，如果clientid对应的会话超时或变为无效，则zookeeper_init返回一个非法的zhandle_t，通过zhandle_t的状态可以获知zookeeper_init调用失败的原因（通常为ZOO_EXPIRED_SESSION_STATE）。
    // context：与zhandle_t实例相关联的“上下文对象”（可通过参数zhandle_t传入自定义类型的数据），应用程序可通过zoo_get_context访问它。Zookeeper内部不使用该参数，所以context可设置为NULL。
    // flags：保留参数，设置为0

    if(!mp_zhandle){
        puts("zookeeper_init failed!!!");
        exit(EXIT_FAILURE);
    } 

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(mp_zhandle, &sem);
    sem_wait(&sem);
    puts("zookeeper_init succeed!!!");
}


// zookeeper_client create zknode
void CLZooKeeperClient::create(const char* _path, const char* _data, std::size_t _size, int32_t _state){
    char path_buffer[128];
    int path_buffer_len = 128;
    int flag = zoo_exists(mp_zhandle, _path, 0, nullptr);

    // 检查节点状态有两个接口，分别是zoo_exists()和zoo_wexists()
    // 区别是后者可以指定单独的watcher_fn(监视回调函数)
    // 而前者只能用zookeeper_int()设置的全局监视器回调函数。

    if(flag == ZNONODE){
        flag = zoo_create(mp_zhandle, _path, _data, _size, &ZOO_OPEN_ACL_UNSAFE, _state, path_buffer, path_buffer_len);

        // zh： Zookeeper_init()返回的zookeeper句柄。
        // path： 节点路径。
        // value： 节点保存的数据。
        // value_len： 节点保存的数据大小。如果value为NULL(节点不包含数据)，则设置为-1。
        // acl： 节点初始ACL，不能为NULL或空。
        // flags： 可设置为0，或者为标识符ZOO_EPHEMERAL、ZOO_SEQUENCE的OR组合。
        // path_buffer： 保存返回节点新路径(因为设置了ZOO_SEQUENCE后zoo_create所创建的节点名称与参数path提供的名称不同，新的节点名称后面填充了序号)，path字符串以NULL结束。path_buffer可以设置为NULL，此时path_buffer_len等于0。
        // path_buffer_len： path_buffer的长度。如果新节点名称的长度大于path_buffer_len，则节点名称将会被截断，而服务器该节点的名称不会截断。

        if(flag == ZOK) puts("zknode create succeed!!!");
        else{
            puts("zknode create failed!!!");
            exit(EXIT_FAILURE);
        }
    }
}

std::string CLZooKeeperClient::get_data(const char* _path){
    char buffer[64];
    int buffer_len = 64;
    int flag = zoo_get(mp_zhandle, _path, 0, buffer, &buffer_len, nullptr);
    // 获取节点数据与检查节点状态API类似，分为两种：zoo_get()和zoo_wget()
    // zh： Zookeeper_init()返回的zookeeper句柄。
    // path： 节点路径。
    // watch： 如果非0，则在服务器端设置监视。当节点发生变化时客户端会得到通知。 
    // buffer： 保存从zookeeper服务器获取的节点数据。
    // buffer_len： Buffer大小。如果节点数据为空，则buffer_len为-1。
    // stat： 返回的stat信息

    if(ZOK == flag) return std::string(buffer, buffer_len);

    puts("get_data failed!!!");
    return "";
}