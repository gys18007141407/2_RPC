/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-25 14:40:44
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-26 17:10:22
 */

#include <string>
#include <iostream>
#include "rpc.h"


// 用户必须要传递正确的参数(类型有严格要求)并传递正确的函数返回值类型来进行RPC
// 若返回值为void，那么RPC的意义何在？

int main()
{
	RPC client;
	client.as_client("127.0.0.1", 5555);

    std::vector<std::vector<int>> m{{1,2,3}, {4,5,6}, {7,8,9}};

    // 注意： int和size_t占的字节数不一样，如果传递的参数和server注册时的参数不一致将报错
    size_t add_sum = client.net_call_wrap<size_t>("add", 1ll<<30, 1ll<<30).get_value();
    std::cout << "add_sum = " << add_sum << std::endl;

    std::vector<int> get_index = client.net_call_wrap<std::vector<int>>("get_index", 12).get_value();
    for(auto& id : get_index) std::cout << id << "\t";
    std::cout << std::endl;
    
    int matrix_sum = client.net_call_wrap<int>("matrix_sum", m, 0, 0).get_value();
    std::cout << "matrix_sum = " << matrix_sum << std::endl;
    
	auto msg = client.net_call_wrap<std::string>("add", "wrong_args_and_awrong_return_type", 36);
    std::cout << msg.get_msg() << std::endl;
	return 0;
}
