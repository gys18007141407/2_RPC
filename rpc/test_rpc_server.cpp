/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-25 14:21:19
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-25 21:40:08
 */

#include <string>
#include <iostream>
#include "rpc.h"

// 类成员函数

class A{
public:
    size_t add(size_t a, size_t b){
        return a+b;
    }
};

// 普通函数
std::vector<int> get_index(uint32_t n){
    std::vector<int> res;
    for(int i = 0; i < n; ++i) res.push_back(i);
    return res;
}

auto lambda_func = [](std::vector<std::vector<int>> graph, int tl, int tr)->int{
    if(graph.size() == 0 || graph[0].size() == 0) return 0;
    int m = graph.size(), n = graph[0].size();
    int res = 0;
    if(tl >= 0 && tl < m && tr >= 0 && tr < n){
        for(int i = tl; i < m; ++i){
            for(int j = tr; j < n; ++j){
                res += graph[i][j];
            }
        }
    }
    return res;
};

std::vector<std::vector<int>> m{{1,2,3}, {4,5,6}, {7,8,9}};

int main()
{
	RPC server;
	server.as_server(5555);
	
    A a;
    std::function<decltype(lambda_func({},0,0))(std::vector<std::vector<int>>, int, int)> lambda_wrap(lambda_func);


	server.regist("add", &A::add, &a);              //注册类成员函数
	server.regist("get_index", get_index);             //注册普通函数
    // server.regist("matrix_sum", lambda_func);    //注册lambda
    server.regist("matrix_sum", lambda_wrap);       //注册std::function
	server.run();
	return 0;
}