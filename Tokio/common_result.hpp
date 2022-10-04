#ifndef TOKIO_COMMON_RESULT_HPP
#define TOKIO_COMMON_RESULT_HPP

#include "ThirdParty/Result.hpp"
#include "common_err.hpp"

#define SafeResult(...) cpp::result<__VA_ARGS__, common::err>

#define WINAPI_FAILIFN(result, code) \
if (!(result)) { \
	THROW_CPP_ERROR \
	return cpp::fail(common::err(common::errcode::code, common::errtype::WinAPI, GetLastError())); \
}

#define HRESULT_FAILIFN(result, code) \
if (result != S_OK) { \
	THROW_CPP_ERROR \
	return cpp::fail(common::err(common::errcode::code, common::errtype::HRESULT, static_cast<HRESULT>(result))); \
}

#define RESULT_FAILIFN(result, code) \
if (!(result)) { \
	THROW_CPP_ERROR \
	return cpp::fail(common::err(common::errcode::code)); \
}

#define WINAPI_FAILIFN_NM(result) \
if (!(result)) { \
	return cpp::fail(common::err(common::errcode::NoMessage, common::errtype::WinAPI, GetLastError())); \
}

#define HRESULT_FAILIFN_NM(result) \
if (result != S_OK) { \
	return cpp::fail(common::err(common::errcode::NoMessage, common::errtype::HRESULT, static_cast<HRESULT>(result))); \
}

#define RESULT_FAILIFN_NM(result) \
if (!(result)) { \
	return cpp::fail(common::err(common::errcode::NoMessage)); \
}

#define RESULT_FAILIFN_PASS(result, error) \
if (!(result)) { \
	return cpp::fail(error); \
}

#define RESULT_FORWARD(result) return cpp::fail(common::err(result.error()))

#define RESULT_THROW(code) return cpp::fail(common::err(common::errcode::code))
#define WINAPI_THROW(code) return cpp::fail(common::err(common::errcode::code, common::errtype::WinAPI))
#define HRESULT_THROW(code) return cpp::fail(common::err(common::errcode::code, common::errtype::HRESULT))

#endif // !TOKIO_COMMON_RESULT_HPP