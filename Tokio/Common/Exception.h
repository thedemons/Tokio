#ifndef TOKIO_COMMON_EXCEPTION_H
#define TOKIO_COMMON_EXCEPTION_H

#include <string>
#include <exception>

#define EXCEPT // the function might throw an exception

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
        m_message += "\n\n" + other.m_message;
        return *this;
    }

    _CONSTEXPR20 Exception& operator+=(const char* message) noexcept
    {
        m_message += "\n\n" + std::string(message);
        return *this;
    }

    _CONSTEXPR20 void Log(const char* fmt, ...) const noexcept
    {
        printf_s("Exception: %s\nMessage:", m_message.c_str());

        va_list args;
        va_start(args, fmt);
        vprintf_s(fmt, args);
        va_end(args);
    }
};


void Log(const char* fmt, ...);
void Log(const Exception& e);
void Log(const Exception& e, const char* fmt, ...);
}


#endif // !TOKIO_COMMON_EXCEPTION_H