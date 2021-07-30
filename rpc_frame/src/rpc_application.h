/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-28 22:10:29
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-28 22:10:29
 */


#ifndef RPC_APPLICATION_H
#define RPC_APPLICATION_H

#include "rpc_config.h"
#include "rpc_channel.h"
#include "rpc_controller.h"

// init rpc frame
class CLRPCApplication final{
private:
    static CLRPCConfig ms_config;

    explicit CLRPCApplication(){ }

    CLRPCApplication& operator=(const CLRPCApplication&) = delete;
    CLRPCApplication(const CLRPCApplication&) = delete;
    CLRPCApplication(CLRPCApplication&&) = delete;
    CLRPCApplication& operator=(CLRPCApplication&&) = delete;

public:
    static void init(int argc, char** argv);

    static CLRPCApplication& get_instance(){
        static CLRPCApplication rpc_app;
        return rpc_app;
    }

    static CLRPCConfig& get_config(){
        return CLRPCApplication::ms_config;
    }
};

#endif