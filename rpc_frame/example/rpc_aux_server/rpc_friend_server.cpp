/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-30 16:51:49
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-30 16:51:50
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "friend.pb.h"
#include "rpc_application.h"
#include "rpc_provider.h"
#include "logger.h"


// 假设现在增加服务：好友管理

class CLRPCGetFriendListsService final: public FriendList::RPC_Service_Friend_List{
private:
    std::map<std::string, std::set<std::string>> id2friends;
public:
    CLRPCGetFriendListsService(){
        // 假设经过Regist注册,并通过添加好友后的好友关系如下
        id2friends["zhang_san"].insert("zhang_yi");
        id2friends["zhang_san"].insert("zhang_er");
        id2friends["li_si"].insert("li_yi");
        id2friends["li_si"].insert("li_er");
        id2friends["li_si"].insert("li_san");
    }

    // local method
    std::vector<std::string> GetFriendList(const std::string& id){
        std::vector<std::string> friend_list;
        if(this->id2friends.count(id)){
            for(auto& other_id: this->id2friends[id]){
                friend_list.push_back(other_id);
            }
        }
        return friend_list;
    }

    // rpc method
    void GetFriendList(
        google::protobuf::RpcController* _pController,
        const FriendList::GetFriendListRequest* _pRequest,
        FriendList::GetFriendListResponse* _pResponse,
        google::protobuf::Closure* _done
    ){
        // 获取请求消息中的参数
        std::string id = _pRequest->id();

        // 调用本地方法
        std::vector<std::string> local_result = this->GetFriendList(id);

        // 把结果存入 response 结构
        FriendList::ResultCode* result_code = _pResponse->mutable_result();
        result_code->set_state_code(0);
        result_code->set_desc_msg("query for friends list succeed!!!");

        for(auto& friend_id: local_result){
            _pResponse->add_friends_list(friend_id);
        }

        // callback
        _done->Run();
    }

};

int main(int argc, char** argv){
    LOG_INFO("RPC friend system started!!!");

    // init config
    CLRPCApplication::init(argc, argv);

    // publish service
    CLRPCProvider rpc_provider("RPC_FRIEND_SERVER_IP", "RPC_FRIEND_SERVER_PORT");
    rpc_provider.notifyService(new CLRPCGetFriendListsService);

    // start to provide service
    rpc_provider.run();

    return 0;
}