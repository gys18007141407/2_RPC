/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-29 14:48:24
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-29 14:48:24
 */

#include <iostream>
#include <unistd.h>
#include <string>
#include "rpc_application.h"

#define SHOW_ARGS_TIPS {puts("format: command -i <config_file>");}

CLRPCConfig CLRPCApplication::ms_config;

void CLRPCApplication::init(int argc, char** argv){
    if(argc < 2){
        SHOW_ARGS_TIPS;
        exit(EXIT_FAILURE);
    }

    int opt;
    while(-1 != (opt = getopt(argc, argv, "i:"))){
        if(opt != 'i'){
            SHOW_ARGS_TIPS;
            exit(EXIT_FAILURE);
        }else{
            ms_config.load_config_file(optarg);
            break;
        }
    }

    // print 
    std::cout << "RPC_LOGIN_SERVER_IP: " << ms_config.load("RPC_LOGIN_SERVER_IP") << std::endl;
    std::cout << "RPC_LOGIN_SERVER_PORT: " << ms_config.load("RPC_LOGIN_SERVER_PORT") << std::endl;
    std::cout << "RPC_FRIEND_SERVER_IP: " << ms_config.load("RPC_FRIEND_SERVER_IP") << std::endl;
    std::cout << "RPC_FRIEND_SERVER_PORT: " << ms_config.load("RPC_FRIEND_SERVER_PORT") << std::endl;
    std::cout << "ZOOKEEPER_IP: " << ms_config.load("ZOOKEEPER_IP") << std::endl;
    std::cout << "ZOOKEEPER_PORT: " << ms_config.load("ZOOKEEPER_PORT") << std::endl;
}

