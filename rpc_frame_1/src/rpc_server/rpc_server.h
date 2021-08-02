/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-31 14:14:51
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-31 14:14:51
 */


#include <string>
#include <unordered_map>
#include <functional>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../rpc_base/rpc_result.h"
#include "../rpc_base/serialize.h"
#include "../rpc_base/nocopyable.h"
#include "../rpc_base/tuple_serialize.h"


class CLRPCServer final: public CLNoCopyable{
private:
    const std::string m_rpc_server_ip;
    const std::size_t m_rpc_server_port;
    sockaddr_in m_rpc_server_addr;
    int32_t m_listen_fd;

public:
    CLRPCServer(): m_rpc_server_ip("127.0.0.1"), m_rpc_server_port(8888), m_listen_fd(-1){   
        if(!init_rpc_server_addr()) exit(EXIT_FAILURE);
    }
    CLRPCServer(const std::string& _ip, const std::size_t& _port): m_rpc_server_ip(_ip), m_rpc_server_port(_port), m_listen_fd(-1){  
        if(!init_rpc_server_addr()) exit(EXIT_FAILURE);
    }
    ~CLRPCServer(){ 
        if(-1 != m_listen_fd) close(m_listen_fd);
    }

public:

    void run(){
        // net server connect and onmessage
        bool reuse = true;
        setsockopt(this->m_listen_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse,sizeof(bool));
        sockaddr_in addr;
        socklen_t len = sizeof(sockaddr);
        while(1){
        int fd = accept(this->m_listen_fd, (sockaddr*)&addr, &len);
        if(-1 == fd){
            puts("accept failed");
            return;
        }

        char buffer[128];
        int recv_bytes = recv(fd, buffer, 128, 0);

        if(recv_bytes < 1){
            puts("recv failed");
            close(fd);
            return;
        }


        printf("recv %d bytes: %s\n", recv_bytes, buffer);

        CLSerializer::CLSptrSlz sp_slz = this->rpc_server_stub(buffer, recv_bytes);
puts("server stub succeed");
        memcpy(buffer, sp_slz->data(), sp_slz->size());
        printf("send %d bytes: %s\n", sp_slz->size(), sp_slz->data());

        int send_bytes = send(fd, buffer, sp_slz->size(), 0);

        if(send_bytes != sp_slz->size()){
            puts("send failed");
            close(fd);
            return;
        }
        close(fd);
        }
        return;
    }

public:
    // rpc_server_stub
    // 接收来自rpc_client序列化后的数据构造CLSerializer。反序列化出rpc_client想要调用的方法和参数,进行方法调用并返回序列化结果
    CLSerializer::CLSptrSlz rpc_server_stub(const char* _data, std::size_t _size){
        // 定义返回结果
        CLSerializer::CLSptrSlz sp_Slz_response = std::make_shared<CLSerializer>();

        // 转为序列化数据
        CLSerializer slz_request(_data, _size);

        // 获取函数名称
        std::string func_name;
        slz_request >> func_name;

        // 查找注册方法
        if(this->m_name2func.count(func_name)){
            auto func = this->m_name2func[func_name];
            func(slz_request.data(), slz_request.size(), sp_Slz_response.get());
        }else{
            // LOG
            // rpc_server没有此方法
printf("rpc server has no method name=[%s]\n", func_name.c_str());
            CLRPCResult<bool> rpc_result;
            rpc_result.set_state_code(ERR_CODE::RPC_NOT_FIND);
            rpc_result.set_state_desc("RPC server do not have this method which name=" + func_name);
            *sp_Slz_response.get() << rpc_result;
        }
        return sp_Slz_response;
    }

    
    // 函数注册
    // 普通函数
    template<typename FUNC>
    void regist(FUNC _func, const std::string& _name){
        this->m_name2func[_name] = std::bind(
            &CLRPCServer::rpc_server_method_wrap<FUNC>,
            this,
            _func,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        );
    }

    // 成员函数
    template<typename FUNC, typename CLASS>
    void regist(FUNC _func, CLASS* _pObj, const std::string& _name){
        this->m_name2func[_name] = std::bind(
            &CLRPCServer::rpc_server_method_warp<FUNC, CLASS>,
            this,
            _func,
            _pObj,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        );
    }

    // 为了统一函数类型(返回值和参数类型[FUNC] -- [Rtype(Args...)]),将rpc_server的方法封装起来
    // 普通函数
    template<typename FUNC>
    void rpc_server_method_wrap(FUNC _func, const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz){
        this->rpc_server_method_call(_func, _pRequest, _size, _pResponseSlz);
    }

    // 成员函数
    template<typename FUNC, typename CLASS>
    void rpc_server_method_warp(FUNC _func, CLASS* _pObj, const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz){
        this->rpc_server_method_call(_func, _pObj, _pRequest, _size, _pResponseSlz);
    }

    // 真正的rpc_server_call
    // 普通函数
    template<typename Rtype, typename... Args>
    void rpc_server_method_call(Rtype(*_pFunc)(Args...), const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz){
        // 获取序列化数据
        CLSerializer Slz(_pRequest, _size);

        // 获取函数参数类型
        using Type_In_Args_Tuple = std::tuple<typename std::decay<Args>::type...>;
        Type_In_Args_Tuple args_tuple;

        // 反序列化函数参数
        CLTupleSerialize tslz;
        tslz.serialize_tuple_r(&Slz, args_tuple);

        // 调用函数
        Rtype result = this->rpc_call_with_tuple_args<Rtype>(_pFunc, args_tuple);
CLRPCResult<Rtype> v;
v.set_value(result);
        // 序列化函数执行结果
        // *_pResponseSlz << result;
        *_pResponseSlz << v;
    }

    // 成员函数
    template<typename Rtype, typename CLASS, typename... Args>
    void rpc_server_method_call(Rtype(CLASS::*_pFunc)(Args...), CLASS* _pObj, const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz){
        // 获取序列化数据
        CLSerializer Slz(_pRequest, _size);

        // 获取函数参数类型
        using Type_In_Args_Tuple = std::tuple<typename std::decay<Args>::type...>;
        Type_In_Args_Tuple args_tuple;

        // 反序列化函数参数
        CLTupleSerialize tslz;
        tslz.serialize_tuple_r(&Slz, args_tuple);

        // 成员函数的特殊性，必须要由成员调用
        auto _lambda = [&_pFunc,_pObj](Args... args)->Rtype{
            return (_pObj->*_pFunc)(args...);
        };

        // 调用函数
        Rtype result = this->rpc_call_with_tuple_args<Rtype>(_lambda, args_tuple);

CLRPCResult<Rtype> v;
v.set_value(result);
        // 序列化函数执行结果
        // *_pResponseSlz << result;
        *_pResponseSlz << v;
    }



    // std::function
    template<typename Rtype, typename... Args>
    void rpc_server_method_call(std::function<Rtype(Args...)> _func,  const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz){
        // 获取序列化数据
        CLSerializer Slz(_pRequest, _size);

        // 获取函数参数类型
        using Type_In_Args_Tuple = std::tuple<typename std::decay<Args>::type...>;
        Type_In_Args_Tuple args_tuple;

        // 反序列化函数参数
        CLTupleSerialize tslz;
        tslz.serialize_tuple_r(&Slz, args_tuple);

        // 调用函数
        Rtype result = this->rpc_call_with_tuple_args<Rtype>(_func, args_tuple);

CLRPCResult<Rtype> v;
v.set_value(result);
        // 序列化函数执行结果
        // *_pResponseSlz << result;
        *_pResponseSlz << v;
    }
    
private:
    bool init_rpc_server_addr();

    template<typename Rtype, typename FUNC, typename Tuple>
    Rtype rpc_call_with_tuple_args(FUNC _func, Tuple _tuple){
        return this->rpc_call_with_tuple_and_index<Rtype>(_func, std::forward<Tuple>(_tuple), std::make_index_sequence<std::tuple_size<Tuple>::value>{});
    }

    template<typename Rtype, typename FUNC, typename Tuple, std::size_t... Index>
    Rtype rpc_call_with_tuple_and_index(FUNC _func, Tuple _tuple, std::index_sequence<Index...>){
        return _func(std::get<Index>(std::forward<Tuple>(_tuple))...);
    }

private:
    // 类比google::protobuf的CallMethod(_pCOntroller, _pRequest, _pResponse, _pDone)
    std::unordered_map<std::string, 
                       std::function<void(const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz)>> m_name2func;

};


bool CLRPCServer::init_rpc_server_addr(){
    
    if(-1 == (this->m_listen_fd = socket(AF_INET, SOCK_STREAM, 0))){
        puts("socket create failed!!!");
        return false;
    }

    m_rpc_server_addr.sin_family = AF_INET;
    m_rpc_server_addr.sin_port = htons(m_rpc_server_port);
    if(-1 == inet_pton(AF_INET, m_rpc_server_ip.c_str(), &m_rpc_server_addr.sin_addr)){
        // LOG
        puts("ip assign failed!!!");
        close(this->m_listen_fd);
        this->m_listen_fd = -1;
        return false;
    }

    if(-1 == bind(this->m_listen_fd, (sockaddr*)&this->m_rpc_server_addr, sizeof(sockaddr))){
        // LOG
        puts("bind failed!!!");
        close(this->m_listen_fd);
        this->m_listen_fd = -1;
        return false;
    }

    if(-1 == listen(this->m_listen_fd, 10)){
        // LOG
        puts("listen failed!!!");
        close(this->m_listen_fd);
        this->m_listen_fd = -1;
        return false;
    }

    return true;
}