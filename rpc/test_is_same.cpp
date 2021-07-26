/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-25 16:41:21
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-25 20:19:30
 */

#include <type_traits>
#include <iostream>
#include <tuple>
using namespace std;

int main(){
    int a = 1;
    cout << is_same<const char*, int>::value << endl;

    char g[8] = "12345";
    std::cout << is_trivially_copyable<decltype(g)>::value << endl;      
    cout << is_same<char*,typename std::decay<decltype(g)>::type>::value << endl;
    cout << is_same<char*,decltype(g)>::value << endl;
    cout << is_trivially_copyable<decltype(g)>::value << endl;
    cout << is_same<char*, std::remove_const<const char*>::type>::value << endl;
    std::tuple<int, double, bool> x = std::make_tuple<int, double, bool>(1,2.3,false);
    for(int i = 0; i < std::tuple_size<decltype(x)>::value; ++i){
        cout << std::get<1>(x) << endl;
    }
    return 0;
}