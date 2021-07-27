/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-24 10:28:18
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-25 18:32:52
 */

#include"serialize.h"
#include<iostream>
#include<unordered_map>
#include<map>
#include<queue>
#include<stack>
#include<list>
using namespace std;

struct A{
    int x;
    double y;
};

struct B{ // 普通类中含有不可平凡复制对象，则需要自己重载<<和>>
    int x;
    std::vector<std::vector<int>> max2d;
    std::string id;

    friend CLSerializer& operator<<(CLSerializer& _slz, B& _b){
        return _slz << _b.x << _b.max2d << _b.id;
    }

    friend CLSerializer& operator>>(CLSerializer& _slz, B& _b){
        return _slz >> _b.x >> _b.max2d >> _b.id;
    }
};

int main(){
    CLSerializer slz;
    int a = 1;
    double b = 3.14;
    bool c = true;
    unsigned long long d = ~0;
    std::string e = "China";
    const char* f = "1234";      
    char g[8] = "abcdefg";      
    

    slz << a << b << c << d << e << f << g;

    int a1 = 0;
    double b1 = 0.0;
    bool c1 = false;
    unsigned long long d1 = 0;
    std::string e1;
    // std::string f1;
    const char* f1 = g; 
    char g1[8];                     // TODO

    slz >> a1 >> b1 >> c1 >> d1 >> e1 >> f1 >> g1;

    cout << a1 << endl;
    cout << b1 << endl;
    cout << c1 << endl;
    cout << d1 << endl;
    cout << e1 << endl;
    cout << f1 << endl;             // TODO
    cout << g1 << endl;             // TODO


    std::vector<std::string> vs = {"I love China! ", " yes "};
    std::vector<std::string> vs1;
    slz << vs;
    slz >> vs1;
    for(auto& _ : vs1) cout << _ << endl;   // OK

    std::list<int> l{1,2,3,4,5,6,7,8};
    std::list<int> l1;
    slz << l;
    slz >> l1;
    for(auto& i : l1) cout << i << "\t";     // OK
    cout << endl;

    A aa{1, 5.2};
    A aa1;
    slz << aa;
    slz >> aa1;
    cout << aa1.x << endl << aa1.y << endl;  // OK

    B bb = {1, {{1, 2, 3}, {3,4,5}}, "Okay"};
    B bb1;
    slz << bb;
    slz >> bb1;
    cout << bb1.x << endl;
    for(auto& i : bb1.max2d){
        for(auto& j : i) cout << j << '\t';
        cout << endl;
    }
    cout << bb1.id << endl;                  // Ok

    unordered_map<int, int> mm;
    unordered_map<int, int> mm1;
    mm[1] = 2, mm[2] = 3, mm[6] = 7;

// TODO： 关联容器没有resize，应该一个一个插入元素.mm.insert(std::pair<key, value> item)
// slz << mm;
// slz >> mm1;
// for(auto& i: mm1) cout << i.first << " : " << i.second << endl; // TODO

/*
std::queue<int> q;
std::queue<int> q1;

std::stack<int> s;
std::stack<int> s1;
*/

    ostream& os(std::cout);
    FILE* of(stdout);
    const char* tp = "12345";
    char jjj[7] = "2356";
    std::string kkk(jjj);
    os << kkk.size() << ":" << kkk << endl;
    os << std::is_same<char*, typename std::decay<decltype(tp)>::type>::value << endl;
    os << std::is_trivially_copyable<decltype("123")>::value << endl;
    os << std::is_trivially_copyable<decltype(tp)>::value << endl; // 1
    os << std::is_same<const char*, typename std::decay<decltype("123")>::type>::value << endl;

    return 0;
}