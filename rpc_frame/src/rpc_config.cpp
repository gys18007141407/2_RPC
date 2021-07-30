/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-29 14:35:18
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-29 14:35:18
 */

#include <string>
#include <iostream>
#include "rpc_config.h"

void CLRPCConfig::load_config_file(const char* _pf_config){
    FILE* pf = fopen(_pf_config, "r");

    if(nullptr == pf){
        std::cout << _pf_config << " is NOT EXISTED!\n";
        exit(EXIT_FAILURE);
    }

    while(!feof(pf)){
        char kv[128];
        fgets(kv, 128, pf);
        std::string skv(kv);
        this->trim_space(skv);
        if(skv.empty() || skv[0] == '#') continue;

        int pos;
        if(-1 == (pos = skv.find('='))){
            std::cout << "wrong format !!!\n";
            exit(EXIT_FAILURE);
        }

        std::string key = skv.substr(0, pos), value = skv.substr(pos+1);
        this->trim_space(key), this->trim_space(value);
        if(key.empty() || value.empty()){
            std::cout << "wrong format !!!\n";
            exit(EXIT_FAILURE);
        }

        this->m_config_map[key] = value;
    }
}