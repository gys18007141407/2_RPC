/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-30 10:18:37
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-30 10:18:37
 */

#include <iostream>
#include <string>

#include "user.pb.h"
#include "rpc_application.h"
#include "logger.h"

#include "friend.pb.h"


int main(int argc, char** argv){

    // callback
    google::protobuf::Closure* callback = nullptr;
    // init config
    CLRPCApplication::init(argc, argv);


    // RPC_CLIENT_STUB
    LoginRegist::Service_User_Login_Regist_Stub rpc_client_login_stub(new CLRPCChannel);


    // RPC method args of Login
    LoginRegist::LoginRequest request;
    request.set_id("666666666");
    request.set_pwd("123456789");

    
    // RPC method response of Login
    LoginRegist::LoginResponse response;


    // call rpc method
    rpc_client_login_stub.Login(nullptr, &request, &response, callback);


    // check server's response
    auto result_info = response.result();
    if(result_info.state_code() == 0){
        puts("RPC client call Login succeed!!!");
    }else{
        puts("RPC client call Login failed!!!");
    }

    // RPC method args of Regist
    LoginRegist::RegistRequest request2;
    request2.set_id("666666666");
    request2.set_pwd("123456789");
    request2.set_name("zhang_san");

    // RPC method response of Regist
    LoginRegist::RegistResponse response2;


    // call rpc method
    rpc_client_login_stub.Regist(nullptr, &request2, &response2, callback);


    // check server's response
    auto result_info2 = response2.result();
    if(result_info2.state_code() == 0){
        puts("RPC client call Regist succeed!!!");
    }else{
        puts("RPC client call Regist failed!!!");
    }
    

    // RPC_CLIENT_STUB
    FriendList::RPC_Service_Friend_List_Stub rpc_client_friend_stub(new CLRPCChannel);


    // RPC method args of GetFriendList
    FriendList::GetFriendListRequest request3;
    request3.set_id("zhang_san");

    // RPC method response of GetFriendList
    FriendList::GetFriendListResponse response3;

    // call rpc method
    rpc_client_friend_stub.GetFriendList(nullptr, &request3, &response3, callback);


    // check server's response 
    auto result_info3 = response3.result();
    if(result_info3.state_code() == 0){
        puts("RPC client call friend list succeed!!!");
        int friend_nums = response3.friends_list_size();

        for(int i = 0; i < friend_nums; ++i){
            std::cout << "第[" << i+1 << "]个好友是:" << response3.friends_list(i) << std::endl;
        }

    }else{
        puts("RPC client call friend list failed!!!");
    }

    return 0;
}