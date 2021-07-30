/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-28 21:52:22
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-28 21:52:22
 */

#ifndef RPC_CONFIG_H
#define RPC_CONFIG_H

#include <unordered_map>
#include <string>

// read config info
class CLRPCConfig final{
private:
    std::unordered_map<std::string, std::string> m_config_map;

    // remove space
    void trim_space(std::string& src_buf){
        int l = src_buf.find_first_not_of(' ');
        int r = src_buf.find_last_not_of(' ');

        if(l == -1) l = 0;
        if(r == -1) r = src_buf.size();
        else r += 1;
        src_buf = src_buf.substr(l, r-l);
        if(src_buf.size() && src_buf.back() == '\n') src_buf.erase(src_buf.size()-1, 1);
    }

public:

    // parse
    void load_config_file(const char* pf_config);

    // query config info
    std::string load(const std::string& _key){
        if(m_config_map.count(_key)) return m_config_map[_key];
        return "";
    }
};

#endif