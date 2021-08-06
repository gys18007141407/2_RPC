/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-31 16:06:33
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-31 16:06:33
 */


#ifndef TUPLE_SERIALIZE_H
#define TUPLE_SERIALIZE_H

#include <tuple>
#include "serialize.h"

class CLTupleSerialize final{
public:

    // tuple ---> serailizer
    template<typename... Args>
    void serialize_tuple(CLSerializer* _pSlz, std::tuple<Args...>& _tuple){
        this->serialize_tuple_with_index(_pSlz, _tuple, std::make_index_sequence<sizeof...(Args)>{});
    }

    template<typename Tuple, std::size_t... Index>
    void serialize_tuple_with_index(CLSerializer* _pSlz, Tuple& _tuple, std::index_sequence<Index...>){
        std::initializer_list<int> _{( *_pSlz << std::get<Index>(_tuple), 0 )...};
    }

    // serializer ---> tuple
    template<typename... Args>
    void serialize_tuple_r(CLSerializer* _pSlz, std::tuple<Args...>& _tuple){
        this->serialize_tuple_with_index_r(_pSlz, _tuple, std::make_index_sequence<sizeof...(Args)>{});
    }

    template<typename Tuple, std::size_t... Index>
    void serialize_tuple_with_index_r(CLSerializer* _pSlz, Tuple& _tuple, std::index_sequence<Index...>){
        std::initializer_list<int> _{( *_pSlz >> std::get<Index>(_tuple), 0 )...};
    }

};




#endif