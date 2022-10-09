/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 * 
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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