/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-30 10:18:28
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-30 10:18:28
 */

#include <iostream>
#include <string>

#include "user.pb.h"
#include "rpc_application.h"
#include "rpc_provider.h"

class CLRPCUserService: public LoginRegist::Service_User_Login_Regist{
public:

    // local method
    bool Login(const std::string& id, const std::string& pwd){
        puts("RPC client call server local Login...");
        LOG_INFO("RPC client call server local Login, with id=[%s] pwd=[%s]", id.c_str(), pwd.c_str());
        std::cout << "id=" << id << std::endl;
        std::cout << "pwd=" << pwd << std::endl;
        return true;
    }

    bool Regist(const std::string& id, const std::string& pwd, const std::string& name){
        puts("RPC client call server local Regist...");
        LOG_INFO("RPC client call server local Regist, with id=[%s] pwd=[%s] name=[%s]", id.c_str(), pwd.c_str(), name.c_str());
        std::cout << "id=" << id << std::endl;
        std::cout << "pwd=" << pwd << std::endl;
        std::cout << "name=" << name << std::endl;
        return true;
    }



    // rpc method: 重写父类虚函数(固定参数格式，rpc.h中绑定到rpc_proxy_wrap)
    // 参数为： RpcController*, Message_Request*, Message_Response*, Closure*
    // 返回值为void

    // rpc.h中也固定了三个参数:_1返回消息的CLSerializer*,_2函数参数的首地址const char*,_3函数参数长度std::size_t
    // this->str2func[_func_name] = std::bind(
    //             &RPC::call_proxy_wrap<FUNC>,
    //             this,    // call_proxy_wrap是RPC类的成员函数
    //             _func,
    //             std::placeholders::_1,
    //             std::placeholders::_2,
    //             std::placeholders::_3
    //     );

    void Login(
        google::protobuf::RpcController* _pController,
        const LoginRegist::LoginRequest* _pLoginRequest,  // const char*, std::size_t
        LoginRegist::LoginResponse* _pLoginResponse,      // CLSerializer*
        google::protobuf::Closure* _done                  // 回调函数
    ){
        std::string id = _pLoginRequest->id();
        std::string pwd = _pLoginRequest->pwd();

        puts("RPC client call server rpc Login...");
        LOG_INFO("RPC client call server rpc Login, with id=[%s] pwd=[%s]", id.c_str(), pwd.c_str());
        std::cout << "id=" << id << std::endl;
        std::cout << "pwd=" << pwd << std::endl;
        

        // call local method
        bool local_result = this->Login(id, pwd);

        // set response
        LoginRegist::ResultCode* pResultCode = _pLoginResponse->mutable_result();
        pResultCode->set_state_code(0);
        pResultCode->set_desc_msg("RPC client call server rpc Login");
        _pLoginResponse->set_is_success(local_result);

        // callback
        _done->Run();
        
    }

    void Regist(
        google::protobuf::RpcController* _pController,
        const LoginRegist::RegistRequest* _pRegistRequest,
        LoginRegist::RegistResponse* _pRegistResponse,
        google::protobuf::Closure* _done
    ){
        std::string id = _pRegistRequest->id();
        std::string pwd = _pRegistRequest->pwd();
        std::string name = _pRegistRequest->name();

        puts("RPC client call server rpc Regist...");
        LOG_INFO("RPC client call server rpc Regist, with id=[%s] pwd=[%s] name=[%s]", id.c_str(), pwd.c_str(), name.c_str());
        std::cout << "id=" << id << std::endl;
        std::cout << "pwd=" << pwd << std::endl;
        std::cout << "name=" << name << std::endl;

        bool local_result = this->Regist(id, pwd, name);

        _pRegistResponse->mutable_result()->set_state_code(0);
        _pRegistResponse->mutable_result()->set_desc_msg("RPC client call server rpc Regist");
        _pRegistResponse->set_is_success(local_result);

        _done->Run();
    }

};


int main(int argc, char** argv){

    // init config
    CLRPCApplication::init(argc, argv);

    // publish service
    CLRPCProvider rpc_provider("RPC_LOGIN_SERVER_IP", "RPC_LOGIN_SERVER_PORT");
    rpc_provider.notifyService(new CLRPCUserService);

    // start to serve
    rpc_provider.run();

    return 0;
}