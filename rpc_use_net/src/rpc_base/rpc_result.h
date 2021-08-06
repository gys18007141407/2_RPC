/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-31 20:41:20
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-31 20:41:20
 */


#ifndef RPC_RESULT_H
#define RPC_RESULT_H

#include <string>
#include "serialize.h"
#include "state_code.h"

template<typename Rtype>
class CLRPCResult final{
private:
    std::size_t m_state_code;
    std::string m_state_desc;
    Rtype m_value;

public:
    explicit CLRPCResult():m_state_code(ERR_CODE::RPC_SUCCESS){ }
    explicit CLRPCResult(const std::size_t& _code, const std::string& _desc, const Rtype& _value ):
            m_state_code(_code), 
            m_state_desc(_desc), 
            m_value(_value)
    {

    }

    ~CLRPCResult(){ }

    void set_state_code(const std::size_t& _code){
        m_state_code = _code;
    }

    void set_state_desc(const std::string& _desc){
        m_state_desc = _desc;
    }

    void set_value(const Rtype& _value){
        m_value = _value;
    }

    std::size_t get_state_code() const {
        return m_state_code;
    }

    std::string get_state_desc() const {
        return m_state_desc;
    }

    Rtype get_value() const {
        return m_value;
    }

    friend CLSerializer& operator<<(CLSerializer& _slz, const CLRPCResult& _result){
        _slz << _result.m_state_code << _result.m_state_desc << _result.m_value;
        return _slz;
    }

    friend CLSerializer& operator>>(CLSerializer& _slz, CLRPCResult& _result){
        _slz >> _result.m_state_code >> _result.m_state_desc >> _result.m_value;
        return _slz;
    }
    
};


#endif