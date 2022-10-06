#include "stdafx.h"
#include "Exception.h"


namespace Tokio
{

Exception::Exception(const std::string& message, Exception::Type type) :
    m_message(message), m_type(type)
{
    //switch (type)
    //{
    //    case Type::WinAPI:
    //        m_code = GetLastError();

    //}
}

Exception::Exception(const char* fmt, ...) :
    m_type(Type::None)
{
    m_message.resize(1024);

    va_list args;
    va_start(args, fmt);
    
    int len = vsnprintf(m_message.data(), m_message.size(), fmt, args);

    va_end(args);

    m_message.resize(len);
}

Exception::Exception(Type type, const char* fmt, ...) :
    m_type(type)
{
    m_message.resize(1024);

    va_list args;
    va_start(args, fmt);
    
    int len = vsnprintf(m_message.data(), m_message.size(), fmt, args);

    va_end(args);

    m_message.resize(len);
}


void Log(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf_s(fmt, args);
    va_end(args);
}
void Log(const Exception& e)
{
    printf_s("%s\n", e.what());
}
}