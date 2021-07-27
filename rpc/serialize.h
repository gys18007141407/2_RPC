/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-07-23 19:57:50
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-07-27 20:06:26
 */

/*

实现顺序容器及普通类型的序列化

TODO：对于关联容器，不用resize，而是一个一个插入实现
TODO：const char*判断是否会越界？越界了又该怎么处理

*/


#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <cassert>
#include <algorithm>
#include <vector>
#include <list>
#include "bufferVec.h"
#include "except.h"


class CLSerializer final{
private:
    CLBuffer::CLSptrBuf m_sptrBuf;
    bool is_little_end;

    bool little_end(){
        uint16_t a = 1;
        return *reinterpret_cast<uint8_t*>(&a) == 1;
    }

    CLSerializer(const CLSerializer&) = delete;
    CLSerializer(CLSerializer&&) = delete;
    const CLSerializer& operator=(const CLSerializer&) = delete;
    const CLSerializer& operator=(CLSerializer&&) = delete;

public:
    using CLSptrSlz = std::shared_ptr<CLSerializer>;

    explicit CLSerializer(){ 
        m_sptrBuf = std::make_shared<CLBuffer>();
        is_little_end = this->little_end();
    }
    explicit CLSerializer(const char* str, std::size_t size){
        m_sptrBuf = std::make_shared<CLBuffer>(str, size);
        is_little_end = this->little_end();
    }

    explicit CLSerializer(CLBuffer::CLSptrBuf sptrBuf):m_sptrBuf(sptrBuf){
        is_little_end = this->little_end();
    }

    void reset(){
        m_sptrBuf->reset();
    }

    void clear(){
        m_sptrBuf->reset();
    }

    void input(const char* str, std::size_t size){
        m_sptrBuf->append(str, size);
    }

    const char* data(){
        return m_sptrBuf->curData();
    }

    std::size_t size() const {
        return m_sptrBuf->curSize();
    }

    std::string to_string(){
        return std::string(m_sptrBuf->data(), m_sptrBuf->size());
    }



    // serialize
    void input_type(const char* _value){  // char* 和 const char*都认为是字符数组
        this->input_type(std::string(_value, strlen(_value)));
    }

    template<typename _Tp, typename std::enable_if<
        std::is_trivially_copyable<_Tp>::value &&                           // const char* 和char* 属于可平凡复制类型
        !std::is_same<char*, typename std::decay<_Tp>::type>::value         // 不复制指针，当成数组！
        , int>::type N = 0>
    void input_type(_Tp _value);

    template<typename _Tp, typename std::enable_if<                         
        !std::is_trivially_copyable<_Tp>::value &&
        std::is_same<typename _Tp::iterator, decltype(std::declval<_Tp>().begin())>::value &&
        std::is_trivially_copyable<typename _Tp::value_type>::value
        , int>::type N = 0>
    void input_type(_Tp _value);

    template<typename _Tp, typename std::enable_if<
        !std::is_trivially_copyable<_Tp>::value &&
        std::is_same<typename _Tp::iterator, decltype(std::declval<_Tp>().begin())>::value &&
        !std::is_trivially_copyable<typename _Tp::value_type>::value
        , int>::type N = 0>
    void input_type(_Tp _value);

    // deserialize
    void output_type(const char* _ref){  // char* 和 const char*都认为是字符数组
        std::string str;
        this->output_type(str);

        char* ptr = const_cast<char*>(_ref);
        memcpy(ptr, str.data(), str.size());
    }

    template<typename _Tp, typename std::enable_if<
        std::is_trivially_copyable<_Tp>::value &&
        !std::is_same<char*, typename std::decay<_Tp>::type>::value
        , int>::type N = 0>
    void output_type(_Tp& _ref);

    template<typename _Tp, typename std::enable_if<
        !std::is_trivially_copyable<_Tp>::value &&
        std::is_same<typename _Tp::iterator, decltype(std::declval<_Tp>().begin())>::value &&
        std::is_trivially_copyable<typename _Tp::value_type>::value
        , int>::type N = 0>
    void output_type(_Tp& _ref);

    template<typename _Tp, typename std::enable_if<
        !std::is_trivially_copyable<_Tp>::value &&
        std::is_same<typename _Tp::iterator, decltype(std::declval<_Tp>().begin())>::value &&
        !std::is_trivially_copyable<typename _Tp::value_type>::value
        , int>::type N = 0>
    void output_type(_Tp& _ref);

    // 从CLSerializer中为Tuple中Index处的变量赋值
    template<typename Tuple, std::size_t Index>
    void get_value(CLSerializer& _ref_slz, Tuple& _ref_tuple){
        _ref_slz >> std::get<Index>(_ref_tuple);
    }

    template<typename Tuple, std::size_t... Index_Packed>
    Tuple get_tuple(std::index_sequence<Index_Packed...>){
        Tuple tuple;
        std::initializer_list<int>{(this->get_value<Tuple, Index_Packed>(*this, tuple), 0)...};
        return tuple;
    }

    // 从CLSerailizer中取出一个_Tp类型的值放到_ref中
    template<typename _Tp>
    CLSerializer& operator>>(_Tp& _ref){
        this->output_type(_ref);
        return *this;
    }

    // 将一个_Tp类型的值_value输入到CLSerializer中
    // 有可能大小端转换，为了不改变源的值，这里传入的是值参数
    template<typename _Tp>
    CLSerializer& operator<<(_Tp _value){
        this->input_type(_value);
        return *this;
    }
};

/*
    serailize
*/

// 一般类型
template<typename _Tp, typename std::enable_if<
        std::is_trivially_copyable<_Tp>::value &&
        !std::is_same<char*, typename std::decay<_Tp>::type>::value
        , int>::type N = 0>
inline void CLSerializer::input_type(_Tp _value){
    
        std::size_t len = sizeof(_Tp);
        char* ptr = reinterpret_cast<char*>(&_value);

        if(this->is_little_end) std::reverse(ptr, ptr+len);
        this->input(ptr, len);
    
}

template<typename _Tp, typename std::enable_if<
        !std::is_trivially_copyable<_Tp>::value &&
        std::is_same<typename _Tp::iterator, decltype(std::declval<_Tp>().begin())>::value &&
        std::is_trivially_copyable<typename _Tp::value_type>::value
        , int>::type N = 0>
inline void CLSerializer::input_type(_Tp _value){
    std::size_t len = _value.size();
    this->input_type(len);
    for(auto& item: _value){
        this->input_type(item);
    }
}

template<typename _Tp, typename std::enable_if<
        !std::is_trivially_copyable<_Tp>::value &&
        std::is_same<typename _Tp::iterator, decltype(std::declval<_Tp>().begin())>::value &&
        !std::is_trivially_copyable<typename _Tp::value_type>::value
        , int>::type N = 0>
inline void CLSerializer::input_type(_Tp _value){
    std::size_t len = _value.size();
    this->input_type(len);

    for(auto& item: _value){
        this->input_type(item);
    }
}

// TODO：const char*判断是否会越界？越界了又该怎么处理


/*
    deserialize
*/


// 一般类型
template<typename _Tp, typename std::enable_if<
        std::is_trivially_copyable<_Tp>::value &&
        !std::is_same<char*, typename std::decay<_Tp>::type>::value
        , int>::type N = 0>
inline void CLSerializer::output_type(_Tp& _ref){
    std::size_t len = sizeof(_Tp);

    // assert(this->size() >= len);    ////////////////////////////////////////////////// 改为抛出异常
    if(this->size() < len){
        CLException e;
        e.set_err_code(ERR_CODE::SERIALIZER_ERR);
        e.set_err_desc("Cannot resize! CLSerializer does not have resource to assign to this type!");
        throw e;
        return;
    }

    memcpy(&_ref, this->data(), len);
    m_sptrBuf->offset(len);

    char* ptr = reinterpret_cast<char*>(&_ref);
    if(this->is_little_end) std::reverse(ptr, ptr+len);
}

template<typename _Tp, typename std::enable_if<
        !std::is_trivially_copyable<_Tp>::value &&
        std::is_same<typename _Tp::iterator, decltype(std::declval<_Tp>().begin())>::value &&
        std::is_trivially_copyable<typename _Tp::value_type>::value
        , int>::type N = 0>
inline void CLSerializer::output_type(_Tp& _ref){

    try{
        std::size_t len = 0;
        this->output_type(len);  // 如果当前CLSerializer中保存的不是字符串，则这里可能会获取到一个很大的值，导致接下来resize失败

        try{
            _ref.resize(len);
        }catch(std::exception std_e){
            CLException e;
            e.set_err_code(ERR_CODE::SERIALIZER_ERR);
            e.set_err_desc("Cannot resize! CLSerializer current data may not be a Container!\n");
            throw e;
        }
        for(auto& item: _ref){
            this->output_type(item);
        }
    }catch(CLException e){
        throw e;
        return;
    }
}

template<typename _Tp, typename std::enable_if<
        !std::is_trivially_copyable<_Tp>::value &&
        std::is_same<typename _Tp::iterator, decltype(std::declval<_Tp>().begin())>::value &&
        !std::is_trivially_copyable<typename _Tp::value_type>::value
        , int>::type N = 0>
inline void CLSerializer::output_type(_Tp& _ref){

    try{
        std::size_t len = 0;
        this->output_type(len);
        try{
            _ref.resize(len);
        }catch(std::exception std_e){
            std::string std_e_desc = std::string(std_e.what(), strlen(std_e.what()));
            CLException e;
            e.set_err_code(ERR_CODE::SERIALIZER_ERR);
            e.set_err_desc( std_e_desc + "\nCLSerializer current data is not a Container!\n");
            throw e;
        }
        for(auto& item: _ref){
        this->output_type(item);
    }
    }catch(CLException e){
        throw e;
        return;
    }
}

// TODO: char*判断是否会越界？越界了又该怎么处理


#endif
