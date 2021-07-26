/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-24 18:34:46
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-25 22:28:22
 */

#ifndef RPC_H
#define RPC_H

#include <iostream>
#include <zmq.hpp>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <functional>
#include "serialize.h"

template<typename Rtype>
struct type_xx{
    using type = Rtype;
};

template<>
struct type_xx<void>{
    using type = int8_t;
};

// 打包帮助模板
template<typename Tuple, std::size_t... Index_Packed>
void pack_args_impl(CLSerializer& _slz, const Tuple& _tuple, std::index_sequence<Index_Packed...>){
    std::initializer_list<int> { (_slz << std::get<Index_Packed>(_tuple), 0)... };
}

template<typename... Args>
void pack_args(CLSerializer& _slz, const std::tuple<Args...>& _tuple){
    pack_args_impl(_slz, _tuple, std::index_sequence_for<Args...>{});
}

// 用tuple做参数来调用函数模板
template<typename FUNC, typename Tuple, std::size_t... Index_Packed>
decltype(auto) invoke_impl(FUNC&& _func, Tuple&& _tuple, std::index_sequence<Index_Packed...>){
    
    // 获取编译时常量来调用get得到tuple元素
    // std::cout << sizeof...(Index_Packed) << "\n";
    // auto x = std::get<0>(_tuple);
    // auto y = _tuple.get<1>();
    // auto z = _tuple.get<2>();
    
    return _func(std::get<Index_Packed>(std::forward<Tuple>(_tuple))...);    
}

template<typename FUNC, typename Tuple>
decltype(auto) invoke(FUNC&& _func, Tuple&& _tuple){
    // 对类型要求不是特别严格的情况下，用decay退化类型为基本形态，去除&及字符数组等避免类型不一致。
    constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return invoke_impl(std::forward<FUNC>(_func), std::forward<Tuple>(_tuple), std::make_index_sequence<size>{});
}

// 直接使用Rtype可行？？不需要type_xx
template<typename Rtype, typename FUNC, typename Tuple>
typename std::enable_if<!std::is_same<Rtype,void>::value, typename type_xx<Rtype>::type>::type
call_helper(FUNC _func, Tuple _tuple){
    return invoke(std::forward<FUNC>(_func), std::forward<Tuple>(_tuple));
}





enum RPC_ROLE{ RPC_SERVER, RPC_CLIENT };
enum ERR_CODE{ RPC_SUCCESS, RPC_NOT_FIND, RPC_TIMEOUT };

template<typename Rtype>
class CLResponse{
private:
    std::size_t m_code;
    std::string m_msg;
    Rtype m_val;

public:
    CLResponse():m_code(ERR_CODE::RPC_SUCCESS){ }
    CLResponse(std::size_t _code, std::string _msg, Rtype _rtype):
        m_code(_code),
        m_msg(_msg),
        m_val(_rtype)
    {

    }
    ~CLResponse(){  }
    
    std::size_t get_code() const { return m_code; }
    std::string get_msg() const { return m_msg; }
    Rtype get_value() const { return m_val; }

    void set_code(const std::size_t& _code){
        m_code = _code;
    }

    void set_msg(const std::string& _msg){
        m_msg = _msg;
    }

    void set_value(const Rtype& _rtype){
        m_val = _rtype;
    }

    friend CLSerializer& operator<<(CLSerializer& _slz, const CLResponse<Rtype>& _ref){
        return _slz << _ref.m_code << _ref.m_msg << _ref.m_val;
    }

    friend CLSerializer& operator>>(CLSerializer& _slz, CLResponse<Rtype>& _ref){
        return _slz >> _ref.m_code >> _ref.m_msg >> _ref.m_val;
    }
};

class RPC final{
private:
    std::size_t m_rpc_role;
    zmq::context_t m_zmq_context;
    std::unique_ptr<zmq::socket_t, 
                    std::function<void(zmq::socket_t*)>> m_up_socket;
    std::unordered_map<std::string, 
                    std::function<void(CLSerializer*,const char*,std::size_t)>> str2func;
    std::ostream& m_ostream;
    
public:
    explicit RPC(std::ostream& _ostream = std::cout):
                m_zmq_context(1),
                m_ostream(_ostream)
    {
    }
    ~RPC(){ m_zmq_context.close(); }

    // ZMQ 提供了三个基本的通信模型，分别是
    // “Request-Reply “
    // ”Publisher-Subscriber“
    // ”Parallel Pipeline”

    void as_client(const std::string& _ip, int _port){
        m_rpc_role = RPC_ROLE::RPC_CLIENT;
        m_up_socket = std::unique_ptr<zmq::socket_t, 
                                      std::function<void(zmq::socket_t*)>> (
                        new zmq::socket_t(m_zmq_context, ZMQ_REQ), 
                        [](zmq::socket_t* _pZmqSock)->void{
                           _pZmqSock->close();
                           delete _pZmqSock;
                           _pZmqSock = nullptr;
                       });
        std::string addr = "tcp://" + _ip + ":" + std::to_string(_port);
        m_up_socket->connect(addr);
    }

    void as_server(int _port){
        m_rpc_role = RPC_ROLE::RPC_SERVER;
        m_up_socket = std::unique_ptr<zmq::socket_t, 
                                      std::function<void(zmq::socket_t*)>> (
                        new zmq::socket_t(m_zmq_context, ZMQ_REP), 
                        [](zmq::socket_t* _pZmqSock)->void{
                           _pZmqSock->close();
                           delete _pZmqSock;
                           _pZmqSock = nullptr;
                       });
        std::string addr = "tcp://*:" + std::to_string(_port);               
        m_ostream << addr << "\n";
        m_up_socket->bind(addr);
    }

    bool send(zmq::message_t& _msg){
        return m_up_socket->send(_msg);
    }

    bool recv(zmq::message_t& _msg){
        return m_up_socket->recv(&_msg);
    }

    void set_timeout(std::size_t _ms){
        if(m_rpc_role == RPC_ROLE::RPC_CLIENT)
            m_up_socket->setsockopt(ZMQ_RCVTIMEO, _ms);
    }

    CLSerializer::CLSptrSlz call_wrap(const std::string& _func_name,
                                      const char* _data,
                                      size_t _size)
    {
        CLSerializer::CLSptrSlz sptrSlz = std::make_shared<CLSerializer>();
        if(!str2func.count(_func_name)){
            std::string err_msg = "function not found! name=" + _func_name;
            m_ostream << err_msg << "\n";
            *sptrSlz.get() << static_cast<std::size_t>(ERR_CODE::RPC_NOT_FIND);
            *sptrSlz.get() << err_msg;
        }else{
            auto func = str2func[_func_name];
            func(sptrSlz.get(), _data, _size);
        }
        return sptrSlz;
    }

    void run(){
        
        if(m_rpc_role == RPC_ROLE::RPC_CLIENT){
            m_ostream << "RPC_CLIENT do not need run!\n";
            return;
        }

        while(1){
            zmq::message_t msg;
            this->recv(msg);
            CLSerializer slz(reinterpret_cast<char*>(msg.data()), msg.size());

            std::string func_name;
            slz >> func_name;

            try{
                CLSerializer::CLSptrSlz sptrSlz = this->call_wrap(func_name, slz.data(), slz.size());
                zmq::message_t response(sptrSlz->data(), sptrSlz->size());
                this->send(response);
                m_ostream << "sucess\n";
            }catch(std::exception e){ 
// 不需要
                m_ostream << e.what() << "\n";
                CLResponse<int8_t> _reponse(ERR_CODE::RPC_NOT_FIND, std::string(e.what(), strlen(e.what())), 0);
                CLSerializer reply_slz;
                reply_slz << _reponse;
                zmq::message_t response(reply_slz.data(), reply_slz.size());
                this->send(response);
            }

        }
    }

//  RPC_CLIENT_CALL
    template<typename Rtype>
    CLResponse<Rtype> net_call(CLSerializer& _slz){
// 为什么多加一个字节
        zmq::message_t request(_slz.size());
        memcpy(request.data(), _slz.data(), _slz.size());
        this->send(request);

        zmq::message_t response;
        CLResponse<Rtype> native_response;

        if(!this->recv(response)){
            native_response.set_code(ERR_CODE::RPC_TIMEOUT);
            native_response.set_msg("RPC RECV TIMEOUT");
        }else{
            CLSerializer slz(reinterpret_cast<char*>(response.data()), response.size());
            slz >> native_response;
        }

        return native_response;
    }

    template<typename Rtype, typename... Args>
    CLResponse<Rtype> net_call_wrap(const std::string& _func_name, Args... _args){
        using Args_Type_Tuple = std::tuple<typename std::decay<Args>::type...>;
        Args_Type_Tuple args_tuple = std::make_tuple(_args...);
        CLSerializer slz;
        slz << _func_name;
        pack_args(slz, args_tuple);
        return net_call<Rtype>(slz);
    }


//  RPC_SERVER_FUNCTION_PROXY
    template<typename FUNC>
    void call_proxy_wrap(FUNC _func, CLSerializer* _pSlz, const char* _data, std::size_t _size){
        this->call_proxy(_func, _pSlz, _data, _size);    
    }

    template<typename FUNC, typename OBJ>
    void call_proxy_wrap(FUNC _func, OBJ* _pObj, CLSerializer* _pSlz, const char* _data, std::size_t _size){
        this->call_proxy(_func, _pObj, _pSlz, _data, _size);
    }

    // 普通函数指针
    template<typename Rtype, typename... Args>
    void call_proxy(Rtype(*_pFunc)(Args...), CLSerializer* _pSlz, const char* _data, std::size_t _size){
        this->call_proxy(std::function<Rtype(Args...)>(_pFunc), _pSlz, _data, _size);
    }

    // 成员函数指针
    template<typename Rtype, typename CLASS, typename OBJ, typename... Args>
    void call_proxy(Rtype(CLASS::*_pFunc)(Args...), OBJ* _pObj, CLSerializer* _pSlz, const char* _data, std::size_t _size){
        using Args_Type_Tuple = std::tuple<typename std::decay<Args>::type...>;

        CLSerializer slz(_data, _size);
        Args_Type_Tuple args_tuple = slz.get_tuple<Args_Type_Tuple>(std::index_sequence_for<Args...>{});

        // 是否可以异步调用返回future对象？
        auto callable = [=](Args... args)->Rtype{
            // 是否可以采用declval绕过构造函数，不需要_pObj?
            return (_pObj->*_pFunc)(args...);
        };

        typename type_xx<Rtype>::type return_val = call_helper<Rtype>(callable, args_tuple);

        CLResponse<Rtype> native_response;
        native_response.set_code(ERR_CODE::RPC_SUCCESS);
        native_response.set_msg("RPC CALL SUCCESSFULLY");
        native_response.set_value(return_val);

        *_pSlz << native_response;
    }

    // std::function
    template<typename Rtype, typename... Args>
    void call_proxy(std::function<Rtype(Args...)> _func, CLSerializer* _pSlz, const char* _data, std::size_t _size){
        using Args_Type_Tuple = std::tuple<typename std::decay<Args>::type...>;

        // 根据tuple的类型来获取对应值
        CLSerializer slz(_data, _size);
        Args_Type_Tuple args_tuple = slz.get_tuple<Args_Type_Tuple>(std::index_sequence_for<Args...>{});

        typename type_xx<Rtype>::type return_val = call_helper<Rtype>(_func, args_tuple);

        CLResponse<Rtype> native_response;
        native_response.set_code(ERR_CODE::RPC_SUCCESS);
        native_response.set_msg("RPC CALL SUCCESSFULLY");
        native_response.set_value(return_val);

        *_pSlz << native_response;
    }

//  RPC_SERVER_FUNCTION_REGIST
    template<typename FUNC>
    void regist(const std::string& _func_name, FUNC _func){
        this->str2func[_func_name] = std::bind(
                &RPC::call_proxy_wrap<FUNC>,
                this,    // call_proxy_wrap是RPC类的成员函数
                _func,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
        );
    }

    template<typename FUNC, typename OBJ>
    void regist(const std::string& _func_name, FUNC _func, OBJ* _pObj){
        this->str2func[_func_name] = std::bind(
                &RPC::call_proxy_wrap<FUNC, OBJ>,
                this,  // call_proxy_wrap是RPC类的成员函数
                _func,
                _pObj,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
        );
    }

};

#endif