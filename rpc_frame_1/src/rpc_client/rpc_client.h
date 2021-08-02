/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-31 14:14:40
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-31 14:14:40
 */

#include <future>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../rpc_base/serialize.h"
#include "../rpc_base/nocopyable.h"
#include "../rpc_base/tuple_serialize.h"
#include "../rpc_base/rpc_result.h"

class CLRPCClient final: public CLNoCopyable{
private:
    const std::string m_rpc_server_ip;
    const std::size_t m_rpc_server_port;
    sockaddr_in m_rpc_server_addr;
    int32_t m_fd;

    std::vector<std::thread> m_threads;
public:
    CLRPCClient(): m_rpc_server_ip("127.0.0.1"), m_rpc_server_port(8888), m_fd(-1){   
        if(!init_rpc_server_addr()) exit(EXIT_FAILURE);
    }
    CLRPCClient(const std::string& _ip, const std::size_t& _port): m_rpc_server_ip(_ip), m_rpc_server_port(_port), m_fd(-1){  
        if(!init_rpc_server_addr()) exit(EXIT_FAILURE);
    }
    ~CLRPCClient(){ 
        if(-1 != m_fd) close(m_fd);
    }

public:

    // rpc_client_stub
    // 客户端给出函数名和对应参数，将参数类型打包成一个元组，然后序列化 slz << func_name << args_tuple
    // 最后，通过rpc_net_call(slz)请求rpc_server调用对应函数

    // 异步调用
    // 1、返回future对象
    template<typename Rtype, typename... Args>
    std::future<Rtype> rpc_client_future_stub(const std::string& _func_name, Args&&... _func_args);

    // 2、设置回调函数（以函数返回值为回调函数参数）
    template<typename Rtype, typename CALLBACK, typename... Args>
    void rpc_client_callback_stub(const std::string& _func_name, CALLBACK&& _callback, Args&&... _func_args);

    // 同步调用
    template<typename Rtype, typename... Args>
    Rtype rpc_client_sync_stub(const std::string& _func_name, Args&&... _func_args);


private:

    // rpc_client将参数序列化后发送给rpc_server
    template<typename Rtype>
    std::future<Rtype> rpc_net_call(CLSerializer* _pResponseSlz);


    bool init_rpc_server_addr();

};

bool CLRPCClient::init_rpc_server_addr(){
    
    m_rpc_server_addr.sin_family = AF_INET;
    m_rpc_server_addr.sin_port = htons(m_rpc_server_port);
    if(-1 == inet_pton(AF_INET, m_rpc_server_ip.c_str(), &m_rpc_server_addr.sin_addr)){
        // LOG
        puts("ip assign failed!!!");
        return false;
    }

    return true;
}


template<typename Rtype, typename... Args>
std::future<Rtype> CLRPCClient::rpc_client_future_stub(const std::string& _func_name, Args&&... _args){
    CLSerializer slz;

    // 首先序列化函数名称
    slz << _func_name;

    // 序列化函数参数
    using Type_In_Args_Tuple = std::tuple<typename std::decay<Args>::type...>;
    Type_In_Args_Tuple args_tuple = std::make_tuple(_args...);

    // 直接序列化参数元组(也可以再定义一个函数用来展开Args，并逐个序列化参数)
    CLTupleSerialize tslz;
    tslz.serialize_tuple(&slz, args_tuple);
    
    return this->rpc_net_call<Rtype>(&slz);
}

template<typename Rtype, typename CALLBACK, typename... Args>
void CLRPCClient::rpc_client_callback_stub(const std::string& _func_name, CALLBACK&& _callback, Args&&... _args){
    auto async_result = std::make_shared<std::future<Rtype>> (this->rpc_client_future_stub<Rtype>(_func_name, std::move(_args)...));
    // TODO: threadpool
    std::thread _(
            [async_result](CALLBACK&& _cb){
                Rtype v = async_result->get();
                puts("in thread");
                _cb(v);
            },
            _callback
    );
    _.detach();
}



// 同步调用(转异步调用后等待返回值)
template<typename Rtype, typename... Args>
Rtype CLRPCClient::rpc_client_sync_stub(const std::string& _func_name, Args&&... _func_args){
    std::future<Rtype> async_result = this->rpc_client_future_stub<Rtype>(_func_name, std::move(_func_args)...);
    return async_result.get();
}


template<typename Rtype>
std::future<Rtype> CLRPCClient::rpc_net_call(CLSerializer* _pSlz){
    if(-1 == (m_fd = socket(AF_INET, SOCK_STREAM, 0))){
        puts("fd create failed!!!");
        exit(EXIT_FAILURE);
    }

    // 连接rpc_server
    if(-1 == connect(m_fd, (sockaddr*)&m_rpc_server_addr, sizeof(sockaddr))){
        puts("connect server failed!!!");
        exit(EXIT_FAILURE);
    }

    // 发送序列化之后的消息给rpc_server
    int send_bytes = send(m_fd, _pSlz->data(), _pSlz->size(), 0);
    if(send_bytes != _pSlz->size()){
        puts("send failed!!!");
        exit(EXIT_FAILURE);
    }

    // 接收rpc_server的response
    // 
    auto p = std::make_shared< std::packaged_task<Rtype()> > (

            [this]()->Rtype{
                char buffer[256];
                int recv_bytes = recv(this->m_fd, buffer, 256, 0);

                if(recv_bytes <= 0){
                    puts("recv failed");
                    exit(EXIT_FAILURE);
                }
                close(this->m_fd);
                CLRPCResult<Rtype> v;
                CLSerializer slz(buffer, recv_bytes);

                try{
                    slz >> v;
                }catch(CLException e){
                    // 函数返回值错误
                    // slz在CLRPCResult中的Rtype时失败
                    Rtype _;
                    return _;
                }

                // 是否返回整个相应消息
                return v.get_value(); 
            }
        
    );
    (*p)();
    sleep(2);
    return p->get_future();
}
