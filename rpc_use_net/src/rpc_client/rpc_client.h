/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-31 14:14:40
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-31 14:14:40
 */

#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

/*
    rpc_server:
    禁止注册返回值为空的方法(无意义)
    若要允许可以注册返回值为空的方法，则可对rpc_server和rpc_client做些许修改，强制返回CLResponse<int8_t>的消息

    添加线程池防止单线程多任务阻塞

    rpc_client:(使用TcpClient与rpc_server建立连接)
    同时开始两个异步任务，会产生ambiguous？是否要每次连接server时都采用不同的地址？将rpc_client_stub改为静态函数？

*/

#include <future>
#include <string>

#include "serialize.h"
#include "nocopyable.h"
#include "tuple_serialize.h"
#include "rpc_result.h"

#include "TcpConn.h"
#include "InetAddr.h"
#include "Logging.h"

enum CONNECT_MODE{  // 异步调用时，多次调用是否使用相同的socket。考虑到调用出错时，rpc_server关闭套接字的问题。OnMessage时ambiguous问题等等（暂不实现）
    SOCKET_SAME
};

class CLRPCClient final: public CLNoCopyable{
private:
    const std::string m_rpc_server_ip;
    const std::size_t m_rpc_server_port;
    const CONNECT_MODE m_mode;

    InetAddr m_rpc_server_addr;

public:

    using SptrTcpConn = typename TcpConn::SptrTcpConn;
    using ON_CONNECTION_CALLBACK = typename TcpConn::ON_CONNECTION_CALLBACK;
    using ON_MESSAGE_CALLBACK = typename TcpConn::ON_MESSAGE_CALLBACK;
    using ON_COMPLETE_CALLBACK = typename TcpConn::ON_COMPLETE_CALLBACK;
    using ON_CLOSE_CALLBACK = typename TcpConn::ON_CLOSE_CALLBACK;

    CLRPCClient(const std::string& _ip = "127.0.0.1", std::size_t _port = 8888, CONNECT_MODE _mode = CONNECT_MODE::SOCKET_SAME);
    ~CLRPCClient();

public:

    // rpc_client_stub
    // 客户端给出函数名和对应参数，将参数类型打包成一个元组，然后序列化 slz << func_name << args_tuple
    // 最后，通过rpc_net_call(slz)请求rpc_server调用对应函数

    // 异步调用
    // 1、返回future对象
    template<typename Rtype, typename... Args>
    std::future<Rtype> rpc_client_future_stub(const std::string& _func_name, Args&&... _func_args);
    // 异步调用
    // 2、设置回调函数（以函数返回值为回调函数参数）
    template<typename Rtype, typename CALLBACK, typename... Args>
    void rpc_client_callback_stub(const std::string& _func_name, CALLBACK&& _callback, Args&&... _func_args);

    // 同步调用
    // 等待返回结果
    template<typename Rtype, typename... Args>
    Rtype rpc_client_sync_stub(const std::string& _func_name, Args&&... _func_args);


private:

    // rpc_client将参数序列化后发送给rpc_server
    template<typename Rtype>
    std::future<Rtype> rpc_net_call(CLSerializer* _pResponseSlz);

public:

    std::string getIP() const{
        return m_rpc_server_ip;
    }

    int32_t getPort() const {
        return m_rpc_server_port;
    }

private:

    // 暂时不需要
    void defaultOnConnectCallBack(const SptrTcpConn& _ref_sptr_conn);
    void defaultOnMessageCallBack(const SptrTcpConn& _ref_sptr_conn);
    void defaultOnCompleteCallBack(const SptrTcpConn& _ref_sptr_conn);
    void defaultOnCloseCallBack(const SptrTcpConn& _ref_sptr_conn);

    ON_CONNECTION_CALLBACK _connect_callback;
    ON_MESSAGE_CALLBACK _message_callback;
    ON_COMPLETE_CALLBACK _complete_callback;
    ON_CLOSE_CALLBACK _close_callback;

private:
    bool connect(int32_t _fd);

};


// 异步调用
// 返回future对象
template<typename Rtype, typename... Args>
std::future<Rtype> CLRPCClient::rpc_client_future_stub(const std::string& _func_name, Args&&... _args){
    // 保存序列化的参数
    CLSerializer slz;

    // 首先序列化函数名称
    slz << _func_name;

    // 序列化函数参数
    using Type_In_Args_Tuple = std::tuple<typename std::decay<Args>::type...>;
    Type_In_Args_Tuple args_tuple = std::make_tuple(_args...);

    // 直接序列化参数元组(也可以再定义一个函数用来展开Args，并逐个序列化参数)
    CLTupleSerialize tslz;
    tslz.serialize_tuple(&slz, args_tuple);
    
    // 通过网络调用rpc_server的方法
    return this->rpc_net_call<Rtype>(&slz);
}

// 异步调用
// 绑定回调函数
template<typename Rtype, typename CALLBACK, typename... Args>
void CLRPCClient::rpc_client_callback_stub(const std::string& _func_name, CALLBACK&& _callback, Args&&... _args){

    // 获取future结果
    auto async_result = std::make_shared<std::future<Rtype>> (this->rpc_client_future_stub<Rtype>(_func_name, std::move(_args)...));
    
    // 可使用线程池
    auto _ = std::async(
        std::launch::async,
        [async_result](CALLBACK&& _cb)->void{
            // 等待结果
            Rtype result = async_result->get();

            // 调用回调函数
            _cb(result);
        },
        _callback
    );
}

// 同步调用(转异步调用后等待返回值)
// 等待rpc_server返回结果
template<typename Rtype, typename... Args>
Rtype CLRPCClient::rpc_client_sync_stub(const std::string& _func_name, Args&&... _func_args){
    std::future<Rtype> async_result = this->rpc_client_future_stub<Rtype>(_func_name, std::move(_func_args)...);
    return async_result.get();
}


// rpc_client将序列化后的参数通过网络传输到rpc_server
// 并异步等待执行结果
template<typename Rtype>
std::future<Rtype> CLRPCClient::rpc_net_call(CLSerializer* _pSlz){

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if(-1 == fd || !this->connect(fd)){
        // LOG
        if(-1 == fd) LOG_ERROR << TimeStamp::now().whenCreate_str() << " RPC client socket create error";
        else{
            LOG_ERROR << TimeStamp::now().whenCreate_str() << " RPC client connect RPC server failed";
            close(fd);
        }

        // 直接返回一个默认的值
        std::future<Rtype> _ = std::async(
            std::launch::async,
            []()->Rtype{
                Rtype __;
                return __;
            }
        );
        return _;
    }

    // 发送消息
    if(_pSlz->size() != send(fd, _pSlz->data(), _pSlz->size(), 0)){
        LOG_ERROR << TimeStamp::now().whenCreate_str() << " RPC client send message to RPC server failed";
        close(fd);
    }

    // 接收结果
    auto callable = std::make_shared< std::packaged_task<Rtype(void)> > (
        [fd, _pSlz]()->Rtype{
            char buffer[1024];
            LOG_INFO << TimeStamp::now().whenCreate_str() << " RPC client wait for RPC server response";
            int recv_bytes = recv(fd, buffer, 1024, 0);
            
            CLSerializer slz_response(buffer, recv_bytes);
            CLRPCResult<Rtype> v;

            // 反序列化失败，说明rpc_client使用了错误的类型
            try{
                slz_response >> v;
            }
            catch(CLException e){
                Rtype t;
                return t;
            }
            if(v.get_state_code() != ERR_CODE::RPC_SUCCESS){
                Rtype t;
                return t;
            }
            return v.get_value();
        }
    );
    std::thread(
        [callable](){
            (*callable)();
        }
    ).detach();
    return callable->get_future();

}

// 使用TcpConn连接：不方便，EventLoop中没有实现stop函数
    // EventLoop loop;
    // TcpConn::SptrTcpConn sptr_TcpConn = std::make_shared<TcpConn>(&loop, fd);
    // sptr_TcpConn->setConnectCallBack(std::move(_connect_callback));
    // sptr_TcpConn->setMessageCallBack(std::move(_message_callback));
    // sptr_TcpConn->setCompleteCallBack(std::move(_complete_callback));
    // sptr_TcpConn->setCloseCallBack(std::move(_close_callback));

    // sptr_TcpConn->establish();

// 使用net库下的TcpClient去连接rpc_server，有ambiguous问题，暂未想到好的解决方法
// template<typename Rtype>
// std::future<Rtype> CLRPCClient::rpc_net_call(CLSerializer* _pSlz){

//     // 连接rpc_server
//     if(m_mode == CONNECT_MODE::PORT_SAME){
//         if(m_is_connected == false){
//             std::lock_guard<std::mutex> g_lock(m_mt_uptr);
//             if(m_is_connected == false){

//                 m_uptr_EventLoop.reset(new EventLoop);
//                 m_uptr_TcpClient.reset(new TcpClient(m_uptr_EventLoop.get(), this->m_rpc_server_ip, this->m_rpc_server_port));

//                 m_uptr_TcpClient->setOnConnectCallBack(std::move(_connect_callback));
//                 m_uptr_TcpClient->setOnMessageCallBack(std::move(_message_callback));
//                 m_uptr_TcpClient->setOnCompleteCallBack(std::move(_complete_callback));
//                 m_uptr_TcpClient->setOnCloseCallBack(std::move(_close_callback));


//                 if(m_is_connected = m_uptr_TcpClient->connect()){
//                     // 连接成功
//                     LOG_INFO << TimeStamp::now().whenCreate_str() << " RPC client connect server succeed";
//                 }else{
//                     // LOG
//                     LOG_ERROR << TimeStamp::now().whenCreate_str() << " Author don't implement this function";

//                     // 直接返回一个默认的值
//                     std::future<Rtype> _ = std::async(
//                         std::launch::async,
//                         []()->Rtype{
//                             Rtype __;
//                             return __;
//                         }
//                     );
//                     return _;
//                 }
//             }
//         }else{

//         }
//     }else{
//         // 暂不实现
//         m_uptr_TcpClient->setPort(m_rpc_server_port + random()%1331);
//         // 不使用unique_ptr,要使用新的指针
//         // CLRPCClient需要保存所有连接到的TcpClient指针

//         // LOG
//         LOG_ERROR << TimeStamp::now().whenCreate_str() << " Author don't implement this function";

//         // 直接返回一个默认的值
//         std::future<Rtype> _ = std::async(

//             []()->Rtype{
//                 Rtype __;
//                 return __;
//             }
//         );
//         return _;
//     }

//     // 连接rpc_server成功

//     // 发送序列化之后的消息给rpc_server
//     m_uptr_TcpClient->send(std::string(_pSlz->data(), _pSlz->size()));

//     // 接收rpc_server的response
//     // 如果由多个异步任务阻塞在这里等待rpc_server的回复，ambiguous？

//     auto p = std::make_shared< std::packaged_task<Rtype()> > (

//             [this]()->Rtype

//                 CLRPCResult<Rtype> v;
//                 CLSerializer slz(buffer, recv_bytes);

//                 try{
//                     slz >> v;
//                 }catch(CLException e){
//                     // 函数返回值错误
//                     // slz在CLRPCResult中的Rtype时失败
//                     Rtype _;
//                     return _;
//                 }

//                 // 是否返回整个相应消息
//                 return v.get_value(); 
//             }
        
//     );
//     (*p)();
//     return p->get_future();
// }


#endif