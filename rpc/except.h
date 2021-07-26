/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-26 15:37:27
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-26 16:24:55
 */

#ifndef EXCEPT_H
#define EXCEPT_H

#include <exception>
#include <string>

#ifndef RPC_STATUS_CODE
#define RPC_STATUS_CODE
enum RPC_ROLE{ RPC_SERVER, RPC_CLIENT };
enum ERR_CODE{ RPC_SUCCESS, RPC_NOT_FIND, RPC_TIMEOUT, SERIALIZER_ERR };
#endif

class CLException final{
private:
    std::string m_desc;
    std::size_t m_code;
    RPC_ROLE m_role;

public:

    explicit CLException():
        m_code(ERR_CODE::RPC_NOT_FIND), 
        m_role(RPC_ROLE::RPC_SERVER),
        m_desc("ERROR")
    { }

    explicit CLException(const RPC_ROLE& _role, const std::size_t& _code, const std::string& _desc):
        m_code(_code),
        m_role(_role),
        m_desc(_desc)
    { }

    ~CLException(){ }

    void set_err_code(const std::size_t& _code){ m_code = _code; }

    void set_rpc_role(const RPC_ROLE& _role){ m_role = _role; }

    void set_err_desc(const std::string& _desc) { m_desc = _desc; }

    std::size_t get_err_code(){ return m_code; }

    RPC_ROLE get_rpc_role(){ return m_role; }

    std::string get_err_desc() { return m_desc; }

};




#endif