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

#ifndef TOKIO_COMMON_EXCEPTION_H
#define TOKIO_COMMON_EXCEPTION_H

#include <string>
#include <exception>
#include <stdarg.h>

#include <iostream>

#define EXCEPT   // the function might throw an exception
#define NULLABLE // the function might return null

namespace Tokio
{
class Exception : public std::exception
{
public:
    enum Type
    {
        None,
        WinAPI,
        HRESULT,
    };

private:
    std::string m_message;
    Type m_type;
    unsigned __int64 m_code = 0ull;

public:
    explicit Exception(const std::string& message, Type type = Type::None);

    Exception(const char* fmt, ...);
    Exception(Type type, const char* fmt, ...);
    
    _NODISCARD _CONSTEXPR20 const char* what() const noexcept override
    {
        return m_message.c_str();
    }

    _NODISCARD _CONSTEXPR20 const std::string& message() const noexcept
    {
        return m_message;
    }

    _CONSTEXPR20 Exception& operator+=(const Exception& other) noexcept
    {
        m_message += "\n" + other.m_message;
        return *this;
    }

    _CONSTEXPR20 Exception& operator+=(const char* message) noexcept
    {
        m_message += "\n" + std::string(message);
        return *this;
    }

    _CONSTEXPR20 void Log(const char* fmt, ...) const noexcept
    {
        printf_s("Exception: %s\nMessage: ", m_message.c_str());

        va_list args;
        va_start(args, fmt);
        vprintf_s(fmt, args);
        va_end(args);
    }
    _CONSTEXPR20 void Log() const noexcept
    {
        printf_s("Exception: %s\n", m_message.c_str());
    }
};


void Log(const char* fmt, ...);
}


#endif // !TOKIO_COMMON_EXCEPTION_H