/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-28 22:05:44
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-28 22:05:44
 */

#ifndef RPC_CHANNEL_H
#define RPC_CHANNEL_H

#include <google/protobuf/service.h>

class CLRPCChannel final: public google::protobuf::RpcChannel{

public:
    // protobuf ---> stub ---> call

    // 重写父类CallMethod
    
    void CallMethod(
        const google::protobuf::MethodDescriptor* _pMethod,
        google::protobuf::RpcController* _pController,
        const google::protobuf::Message* _pRequest,
        google::protobuf::Message* _pResponse,
        google::protobuf::Closure* _callback
    );
};

#endif