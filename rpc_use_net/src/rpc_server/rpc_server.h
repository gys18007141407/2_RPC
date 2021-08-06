/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-04 16:55:42
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-04 16:55:42
 */

#ifndef RPC_SERVER_H
#define RPC_SERVER_H


/*

    禁止注册返回值为空的方法(无意义)
    若要允许可以注册返回值为空的方法，则可对rpc_server和rpc_client做些许修改，强制返回CLResponse<int8_t>的消息

*/

#include <string>
#include <unordered_map>
#include <functional>

#include "rpc_result.h"
#include "serialize.h"
#include "nocopyable.h"
#include "tuple_serialize.h"

#include "TcpServer.h"
#include "Logging.h"

class CLRPCServer final: public CLNoCopyable{
private:
    const std::string m_rpc_server_ip;
    const int32_t m_rpc_server_port;
    const int32_t m_num_thread;

    TcpServer::UptrTcpServer m_uptr_server;


    // 类比google::protobuf的CallMethod(_pCOntroller, _pRequest, _pResponse, _pDone)
    std::unordered_map<std::string, 
                       std::function<void(const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz)>> m_name2func;


    // rpc_client发送消息过来时的回调函数
    using ON_CONNECTION_CALLBACK = typename TcpServer::ON_CONNECTION_CALLBACK;
    using ON_MESSAGE_CALLBACK = typename TcpServer::ON_MESSAGE_CALLBACK;
    using ON_COMPLETE_CALLBACK = typename TcpServer::ON_COMPLETE_CALLBACK;
    using ON_CLOSE_CALLBACK = typename TcpServer::ON_CLOSE_CALLBACK;

    void defaultOnConnectCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn);
    void defaultOnMessageCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn);
    void defaultOnCompleteCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn);
    void defaultOnCloseCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn);

    ON_CONNECTION_CALLBACK m_connect_callback;
    ON_MESSAGE_CALLBACK m_message_callback;
    ON_COMPLETE_CALLBACK m_complete_callback;
    ON_CLOSE_CALLBACK m_close_callback;

public:
    // ctor
    explicit CLRPCServer(const std::string& _ip = "127.0.0.1", int32_t _port = 8888, int32_t _num_thread = 2);
    ~CLRPCServer();

public:
    std::string getIP() const {
        return m_rpc_server_ip;
    }

    int32_t getPort() const {
        return m_rpc_server_port;
    }

    void setConnectCallBack(ON_CONNECTION_CALLBACK&& _callback){
        this->m_connect_callback = _callback;
    }

    void setMessageCallBack(ON_MESSAGE_CALLBACK&& _callback){
        this->m_message_callback = _callback;
    }

    void setCompleteCallBack(ON_COMPLETE_CALLBACK&& _callback){
        this->m_complete_callback = _callback;
    }

    void setCloseCallBack(ON_CLOSE_CALLBACK&& _callback){
        this->m_close_callback = _callback;
    }

    void run(){
        EventLoop loop;

        m_uptr_server.reset(new TcpServer(&loop, m_rpc_server_ip, m_rpc_server_port));

        m_uptr_server->setConnectCallBack(std::move(m_connect_callback));
        m_uptr_server->setMessageCallBack(std::move(m_message_callback));
        m_uptr_server->setCompleteCallBack(std::move(m_complete_callback));
        m_uptr_server->setCloseCallBack(std::move(m_close_callback));

        m_uptr_server->start();
        loop.loop();
    }


    // 函数注册
    // 普通函数
    template<typename FUNC>
    void regist(FUNC _func, const std::string& _name);
    // 函数注册
    // 成员函数
    template<typename FUNC, typename CLASS>
    void regist(FUNC _func, CLASS* _pObj, const std::string& _name);

private:
    // rpc_server_stub
    // 接收来自rpc_client序列化后的数据构造CLSerializer。反序列化出rpc_client想要调用的方法和参数,进行方法调用并返回序列化结果
    CLSerializer::CLSptrSlz rpc_server_stub(const char* _data, std::size_t _size);
        // 定义返回结果
        // 转为序列化数据
        // 获取函数名称
        // 查找注册方法
        // 调用方法
        // 返回固定结构的消息

    // 为了统一函数类型(返回值和参数类型推导[FUNC] -- [Rtype(Args...)], 固定函数参数为[const char*, std::size_t, CLSerializer*]),将rpc_server的方法封装起来
    // 普通函数
    template<typename FUNC>
    void rpc_server_method_wrap(FUNC _func, const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz){
        this->rpc_server_method_call(_func, _pRequest, _size, _pResponseSlz);
    }
    // 为了统一函数类型(返回值和参数类型推导[FUNC] -- [Rtype(Args...)], 固定函数参数为[const char*, std::size_t, CLSerializer*]),将rpc_server的方法封装起来
    // 成员函数
    template<typename FUNC, typename CLASS>
    void rpc_server_method_warp(FUNC _func, CLASS* _pObj, const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz){
        this->rpc_server_method_call(_func, _pObj, _pRequest, _size, _pResponseSlz);
    }


    // 真正的rpc_server_call
    // 普通函数
    template<typename Rtype, typename... Args>
    void rpc_server_method_call(Rtype(*_pFunc)(Args...), const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz);
    // 真正的rpc_server_call
    // 成员函数
    template<typename Rtype, typename CLASS, typename... Args>
    void rpc_server_method_call(Rtype(CLASS::*_pFunc)(Args...), CLASS* _pObj, const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz);
    // 真正的rpc_server_call
    // std::function
    template<typename Rtype, typename... Args>
    void rpc_server_method_call(std::function<Rtype(Args...)> _func,  const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz);
    


    // 反序列化调用之前获得编译期常数
    template<typename Rtype, typename FUNC, typename Tuple>
    Rtype rpc_call_with_tuple_args(FUNC _func, Tuple _tuple){
        return this->rpc_call_with_tuple_and_index<Rtype>(_func, std::forward<Tuple>(_tuple), std::make_index_sequence<std::tuple_size<Tuple>::value>{});
    }
    // 反序列化调用
    template<typename Rtype, typename FUNC, typename Tuple, std::size_t... Index>
    Rtype rpc_call_with_tuple_and_index(FUNC _func, Tuple _tuple, std::index_sequence<Index...>){
        return _func(std::get<Index>(std::forward<Tuple>(_tuple))...);
    }


};





// 函数注册
// 普通函数
template<typename FUNC>
void CLRPCServer::regist(FUNC _func, const std::string& _name){
    this->m_name2func[_name] = std::bind(
        &CLRPCServer::rpc_server_method_wrap<FUNC>,
        this,
        _func,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    );
}

// 函数注册
// 成员函数
template<typename FUNC, typename CLASS>
void CLRPCServer::regist(FUNC _func, CLASS* _pObj, const std::string& _name){
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


// 真正的rpc_server_call
// 普通函数
template<typename Rtype, typename... Args>
void CLRPCServer::rpc_server_method_call(Rtype(*_pFunc)(Args...), const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz){
    // 转化为std::function
    this->rpc_server_method_call(std::function<Rtype(Args...)>(_pFunc), _pRequest, _size, _pResponseSlz);
}
// 真正的rpc_server_call
// 成员函数
template<typename Rtype, typename CLASS, typename... Args>
void CLRPCServer::rpc_server_method_call(Rtype(CLASS::*_pFunc)(Args...), CLASS* _pObj, const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz){
    // 获取序列化数据
    CLSerializer Slz(_pRequest, _size);

    // 获取函数参数类型
    using Type_In_Args_Tuple = std::tuple<typename std::decay<Args>::type...>;
    Type_In_Args_Tuple args_tuple;

    // 反序列化函数参数
    CLTupleSerialize tslz;
    tslz.serialize_tuple_r(&Slz, args_tuple);

    // 成员函数的特殊性，必须要由成员调用。(转为std::function的话，由于其含隐式参数CLASS*,参数如何bind?若用sizeof...(Args), 末尾处给出尝试，失败!)
    // auto callable = std::function<Rtype(Args...)> (std::bind(_pFunc, _pObj, std::placeholders::_1, std::placeholders::_2));        
    // auto callable = memberFunc2stdFunc(_pFunc, _pObj, std::make_index_sequence<sizeof...(Args)>{});

    auto _lambda = [&_pFunc,_pObj](Args... args)->Rtype{
        return (_pObj->*_pFunc)(args...);
    };

    // 调用函数
    Rtype result = this->rpc_call_with_tuple_args<Rtype>(_lambda, args_tuple);

    
    // 调用函数
    // Rtype result = this->rpc_call_with_tuple_args<Rtype>(callable, args_tuple);

    CLRPCResult<Rtype> v;
    v.set_value(result);

    // 序列化函数执行结果
    *_pResponseSlz << v;
}

// 真正的rpc_server_call
// std::function
template<typename Rtype, typename... Args>
void CLRPCServer::rpc_server_method_call(std::function<Rtype(Args...)> _func,  const char* _pRequest, std::size_t _size, CLSerializer* _pResponseSlz){
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
    *_pResponseSlz << v;
}

// template<std::size_t Index>
// struct CPlaceHolders{
// #if 0 == Index
//     constexpr static auto value = std::placeholders::_1;
// #elif 1 == Index
//     constexpr static auto value = std::placeholders::_2;
// #elif 2 == Index
//     constexpr static auto value = std::placeholders::_3;
// #elif 3 == Index
//     constexpr static auto value = std::placeholders::_4;
// #elif 4 == Index
//     constexpr static auto value = std::placeholders::_5;
// #else
//     exit(EXIT_FAILURE);
// #endif
// };

// template<typename Rtype, typename CLASS, typename... Args, std::size_t... Index>
// std::function<Rtype(Args...)> memberFunc2stdFunc(Rtype(CLASS::*_pFunc)(Args...), CLASS* _pObj, std::index_sequence<Index...>){
//     return std::bind(_pFunc, _pObj, CPlaceHolders<Index>::value...);
// }


#endif