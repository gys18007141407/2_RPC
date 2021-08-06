/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-02 20:47:28
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 20:50:56
 */
#ifndef LOGGING_H
#define LOGGING_H

#include <cstring>
#include <cstddef>
#include <iostream>
#include "LogStream.h"

class Logging{
public:
    enum LogLevel{
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    class SourceFile{
    public:
        char* _data;
        std::size_t _len;

        template<std::size_t N>
        SourceFile(const char (&filepath)[N]){
            const char* splash = strrchr(filepath, '/');

            if(splash) _data = splash+1;
            else _data = filepath;

            _len = N-1-static_cast<std::size_t>(_data-filepath);
        }

        SourceFile(const char* filename){
            const char* splash = strrchr(filename, '/');
            
            if(splash) _data = const_cast<char*>(splash+1);
            else _data = const_cast<char*>(filename);

            _len = strlen(_data);
        }
    };

    Logging(SourceFile file, std::size_t line);
    Logging(SourceFile file, std::size_t line, LogLevel level);
    Logging(SourceFile file, std::size_t line, LogLevel level, const char* func);
    Logging(SourceFile file, std::size_t line, bool abort);
    ~Logging();

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    LogStream& stream(){
        return _impl._stream;
    }

    typedef void (*outFunc)(const char* msg, std::size_t len);
    typedef void (*flushFunc)();

    static void setOutFunc(outFunc);
    static void setFlushFunc(flushFunc);

private:

    class Impl{
    public:
        typedef Logging::LogLevel LogLevel;
        typedef Logging::SourceFile SourceFile;

        Impl(LogLevel level, int old_errno, SourceFile file, std::size_t line);

        void finish();

        LogLevel _level;
        SourceFile _file;
        std::size_t _line;
        LogStream _stream;
    };

    Impl _impl;


};


#define LOG_TRACE if(Logging::LogLevel::TRACE >= Logging::logLevel()) \
                        Logging(__FILE__, __LINE__, Logging::LogLevel::TRACE, __func__).stream()
#define LOG_DEBUG if(Logging::LogLevel::DEBUG >= Logging::logLevel()) \
                        Logging(__FILE__, __LINE__, Logging::LogLevel::DEBUG, __func__).stream()
#define LOG_INFO if(Logging::LogLevel::INFO >= Logging::logLevel())   \
                        Logging(__FILE__, __LINE__, Logging::LogLevel::INFO).stream()

#define LOG_WARN Logging(__FILE__, __LINE__, Logging::LogLevel::WARN).stream()
#define LOG_ERROR Logging(__FILE__, __LINE__, Logging::LogLevel::ERROR).stream()
#define LOG_FATAL Logging(__FILE__, __LINE__, Logging::LogLevel::FATAL).stream()

#define LOG_SYSERROR Logging(__FILE__, __LINE__, false);
#define LOG_SYSFATAL Logging(__FILE__, __LINE__, true);

const char* strerror_tl(int errno_saved);

#endif