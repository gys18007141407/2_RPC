/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-25 14:21:19
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-27 20:03:53
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

template<typename _Tp>
void ptype(const _Tp& _obj){
    std::cout << typeid(_obj).name() << std::endl;
}

int main()
{
	RPC server;
	server.as_server(5555);
	
    A a;
    std::function<decltype(lambda_func({},0,0))(std::vector<std::vector<int>>, int, int)> lambda_wrap(lambda_func);


	server.regist("add", &A::add, &a);                  // 注册类成员函数
	server.regist("get_index", get_index);              // 注册普通函数
    server.regist("matrix_sum", lambda_wrap);           // 注册std::function


    // server.regist("matrix_sum_lambda", lambda_func);    //注册lambda
    // 为什么不能注册lambda函数：因为就算对于完全相同lambda函数，它们的类型也是不一样的。
    // 更常见的现象是，我们会用auto来声明lambda，却不能显式指定lambda的类型

    auto _ = lambda_func;

    ptype( [](){} );
    ptype( [](){} );
    ptype(lambda_func);
    ptype(_);
    ptype(lambda_wrap);

    ptype(std::function<void(void)>([](){}));
    ptype(std::function<void(void)>([](){}));


    // Z4mainEUlvE_
    // Z4mainEUlvE0_
    // UlSt6vectorIS_IiSaIiEESaIS1_EEiiE_
    // UlSt6vectorIS_IiSaIiEESaIS1_EEiiE_
    // St8functionIFiSt6vectorIS0_IiSaIiEESaIS2_EEiiEE

    // St8functionIFvvEE
    // St8functionIFvvEE
    
	server.run();
	return 0;
}