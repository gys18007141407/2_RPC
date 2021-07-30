/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-28 21:56:45
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-28 21:56:46
 */

#ifndef RPC_CONTROLLER_H
#define RPC_CONTROLLER_H

#include <string>
#include <google/protobuf/service.h>

class CLRPCController final: public google::protobuf::RpcController{
private:
    // when call rpc method
    bool m_is_failed; 
    std::string m_err_msg;

public:
    explicit CLRPCController(){
        reset();
    }

    void reset(){
        m_is_failed = false;
        m_err_msg.clear();
    }
    bool is_failed() const {
        return m_is_failed;
    }

    std::string get_err_msg() const {
        return m_err_msg;
    }

    void set_failed(const std::string& _err_msg){
        m_is_failed = true;
        m_err_msg = _err_msg;
    }


    // TODO: undo
    void start_cancel(){ }
    bool is_canceled() const { return false; }
    void notifyOnCancel(google::protobuf::Closure* _callback){ }
};


#endif
