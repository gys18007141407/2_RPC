/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-29 10:27:22
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-29 10:27:22
 */

#include <ctime>
#include <iostream>
#include <fstream>
#include <cstring>
#include "logger.h"

CLLockQueue<std::string> CLLogger::ms_q;
std::size_t CLLogger::ms_log_level = ELogLevel::INFO;

std::thread CLLogger::ms_writeThread = std::move(
    std::thread(
        [](){

            time_t now = time(nullptr);
            tm* nowtm = localtime(&now);

            char file_name[128] = "\0";
            std::fstream ofs;
            while(true){
                std::string msg = ms_q.pop();
                if(msg.empty()){
                    // std::cout << "lockqueue empty" << std::endl;
                    break;
                }
                now = time(nullptr);
                nowtm = localtime(&now);

                // 如果隔天了，重置ofs，写入新文件
                char log_time[128];
                snprintf(log_time, 128, "%04d-%02d-%02d_log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);
                if(0 != strcmp(file_name, log_time)){
                    ofs.close();
                    strcpy(file_name, log_time);
                    ofs.open(file_name, std::ios::out | std::ios::app);
                    if(!ofs.is_open()){
                        std::cout << "log file open failed! file_name = " << file_name << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
                
                snprintf(log_time, 128, "%02d:%02d:%02d => ", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec);
                msg.insert(0, log_time);
                msg.append("\n");
                ofs.write(msg.c_str(), msg.size());
                ofs.flush();
            }
            ofs.close();
        }
    )
);

CLLogger::CLLogger(){
    ms_writeThread.detach();
}